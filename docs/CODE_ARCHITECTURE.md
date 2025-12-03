# Code Architecture Overview

**Understanding the MiniGLSL Compiler Implementation**

This document explains how the code works, the architecture, and what each component does.

---

## System Architecture

```
Input (.frag file)
    ↓
Scanner (scanner.l) - Tokenizes input
    ↓
Parser (parser.y) - Builds AST
    ↓
Semantic Analyzer (semantic.c) - Type checking, symbol table
    ↓
Code Generator (codegen.c) - Emits SPIR-V
    ↓
Output (.spvasm file)
```

---

## Source Files in `/src`

### Core Compiler Pipeline

#### 1. `scanner.l` (179 lines)
**Purpose:** Lexical analysis - converts text to tokens

**Key Components:**
- **Token patterns:** Matches keywords (`if`, `else`, `float`, `vec4`, etc.)
- **Operators:** `+`, `-`, `*`, `/`, `&&`, `||`, `==`, `!=`, etc.
- **Literals:** Integers, floats, booleans (`true`/`false`)
- **Identifiers:** Variable names
- **Helper functions:**
  - `ParseInt()` - Validates integer range
  - `ParseFloat()` - Validates float range
  - `ParseIdent()` - Extracts identifiers
  - `ParseComment()` - Skips C-style comments

**Important Detail:** Line 1 MUST have `%{` at column 0 (no leading whitespace) - this is a Flex requirement!

#### 2. `parser.y` (302 lines)
**Purpose:** Syntax analysis - builds Abstract Syntax Tree (AST)

**Grammar Structure:**
```yacc
program -> scope
scope -> { statement_list }
statement_list -> (statement | declaration)*
```

**Key Feature - C99-Style Declarations:**
```yacc
// Allows mixed declarations and statements:
{
    vec4 base = ...;     // declaration
    if (x < 10) { ... }  // statement
    vec4 color = base;   // declaration (allowed!)
}
```

**Scope Action (lines 93-116):**
- Takes mixed `statement_list`
- Splits into separate `declarations` and `statements` lists
- Both are needed by later phases

**Why this matters:** Reference shaders have declarations after statements, so we needed to support C99-style.

#### 3. `semantic.c` (723 lines)
**Purpose:** Type checking and semantic validation

**Two-Pass Design:**

**Pass 1 - Symbol Table Population (lines 107-133):**
```c
// Add ALL declarations to symbol table first
for each declaration in scope:
    create symbol_entry
    add to symbol table (skip initializer check)
```

**Pass 2 - Type Checking (lines 136-141):**
```c
// Now check initializers
for each declaration:
    check_expression(initializer)  // all vars now in table!
```

**Why two passes?** Handles forward references:
```c
vec4 base = vec4(1.0, 0.2, 0.2, 1.0);
vec4 color = base;  // 'base' used in initializer
```

**Key Functions:**
- `check_node()` - Dispatches based on AST node type
- `check_declaration()` - Validates declarations and initializers
- `check_statement()` - Validates assignments, if/else, scopes
- `check_expression()` - Type checks expressions recursively
- `get_expression_type()` - Determines result type of expressions

**Symbol Table (symbol.c):**
```c
typedef struct symbol_entry {
    char *name;
    int type_code;      // FLOAT_T, VEC_T, etc.
    int vec_size;       // 0 for scalar, 1-3 for vec2/3/4
    int is_const;       // const qualifier
    int is_write_only;  // gl_FragColor
    int is_read_only;   // gl_Color, uniforms
} symbol_entry;
```

#### 4. `codegen.c` (560 lines) ⭐
**Purpose:** SPIR-V assembly generation

This is the most complex file. Let's break it down:

---

## Code Generation Deep Dive

### Data Structures

#### Constant Pool (lines 13-18)
```c
typedef struct const_entry {
    int id;          // SSA ID
    char *text;      // Full "OpConstant %t_float 1.0" line
    struct const_entry *next;
} const_entry;
```

**Purpose:** Collect all constants during codegen, emit before function

**Why?** SPIR-V requires: `prologue → constants → function`

#### Variable Tracking (lines 49-68)
```c
typedef struct var_entry {
    char *name;      // Variable name
    int id;          // SSA ID for OpVariable
    int type_code;   // FLOAT_T, INT_T, VEC_T
    int vec_size;    // 0, 1, 2, 3 for scalar/vec2/3/4
    struct var_entry *next;
} var_entry;
```

**Purpose:** Remember variable types for correct OpLoad

**Example:**
```c
float x;   // type_code=FLOAT_T, vec_size=0 → OpLoad %t_float
vec4 y;    // type_code=VEC_T, vec_size=3 → OpLoad %t_vec4
```

### Key Algorithms

#### SSA ID Allocation (lines 227-229)
```c
static int next_id = 100;

static int gen_id(void) {
    return next_id++;
}
```

Simple monotonic counter. Every instruction result gets unique ID.

#### Memory Streaming (lines 559-586)
```c
// Buffer function body in memory
char *func_body = NULL;
size_t body_size = 0;
FILE *body_stream = open_memstream(&func_body, &body_size);

// Generate all function code into buffer
fprintf(body_stream, "%%entry = OpLabel\n");
process_decls_allocate(..., body_stream);
...
fclose(body_stream);

// Now emit in correct order:
emit_prologue(output);           // 1. Capabilities, types
emit_constants(output);          // 2. Constants
fprintf(output, "%%main = ...");  // 3. Function header
fprintf(output, "%s", func_body); // 4. Function body
free(func_body);
```

**Why?** Can't emit constants until we've seen all expressions, but constants must come before function in SPIR-V.

#### OpSelect Predication (lines 494-527) ⭐⭐⭐

**The Critical Feature:**

```c
case IF_STMT_NODE: {
    // Evaluate condition
    int cond_id = gen_expression(stmt->if_stmt.condition, out);
    
    // Evaluate BOTH branches
    int then_val = gen_expression(then_expr, out);
    int else_val = gen_expression(else_expr, out);
    
    // Select based on condition
    int result = gen_id();
    fprintf(out, "%%%-3d = OpSelect %s %%%-3d %%%-3d %%%-3d\n",
            result, type, cond_id, then_val, else_val);
    
    // Store result
    fprintf(out, "OpStore %%%-3d %%%-3d\n", var_id, result);
}
```

**What this does:**
- Computes both `then` and `else` values
- Uses `OpSelect` to choose one based on condition
- No branching instructions!

**SPIR-V output:**
```spirv
%cond = OpFOrdLessThan %t_bool %x %y
%val1 = OpFAdd %t_float %a %b      // then value
%val2 = OpFSub %t_float %a %b      // else value  
%result = OpSelect %t_float %cond %val1 %val2
OpStore %var %result
```

**Why OpSelect?** Spec forbids `OpBranch`, `OpBranchConditional`, `OpPhi` - must use linear SSA.

#### Indexed Assignment (lines 441-459)

**Pattern:**
```c
temp[0] = value;
```

**Generated SPIR-V:**
```spirv
%loaded = OpLoad %t_vec4 %temp           // Load entire vector
%updated = OpCompositeInsert %t_vec4     // Insert new component
           %value %loaded 0               //   at index 0
OpStore %temp %updated                    // Store back
```

**Why 3 steps?** SPIR-V doesn't have direct indexed store - must load/modify/store.

### Code Generation Phases

#### Phase 1: Allocate Variables (lines 237-249)
```c
// First pass - allocate ALL variables
for each declaration:
    int var_id = gen_id();
    fprintf(out, "%%%-3d = OpVariable %%t_ptr_func_%s Function\n",
            var_id, type);
    add_local_var(name, var_id, type_code, vec_size);
```

**Output:**
```spirv
%100 = OpVariable %t_ptr_func_float Function
%101 = OpVariable %t_ptr_func_vec4 Function
```

#### Phase 2: Initialize Variables (lines 251-265)
```c
// Second pass - initialize
for each declaration with initializer:
    int var_id = lookup_local_var(name);
    int init_id = gen_expression(initializer, out);
    fprintf(out, "OpStore %%%-3d %%%-3d\n", var_id, init_id);
```

**Output:**
```spirv
%102 = OpConstant %t_float 1.0
OpStore %100 %102
```

#### Phase 3: Process Statements (lines 430-557)
- Assignments → OpStore
- If/else → OpSelect
- Scopes → Recurse

### Expression Generation (lines 267-428)

**Returns SSA ID of result**

**Variable Reference:**
```c
case VAR_NODE:
    if (is_builtin) {
        int result = gen_id();
        fprintf(out, "%%%-3d = OpLoad %%t_vec4 %builtin\n",
                result, builtin_name);
        return result;
    } else {
        int var_id = lookup_var(name);
        const char *type = lookup_var_type(name);
        int result = gen_id();
        fprintf(out, "%%%-3d = OpLoad %s %%%-3d\n",
                result, type, var_id);
        return result;
    }
```

**Binary Expression:**
```c
case '*':
    int left = gen_expression(left_expr, out);
    int right = gen_expression(right_expr, out);
    int result = gen_id();
    fprintf(out, "%%%-3d = OpFMul %s %%%-3d %%%-3d\n",
            result, type, left, right);
    return result;
```

**Function Call (dp3):**
```c
case FUNCTION_NODE with DP3:
    int arg1 = gen_expression(arg1_expr, out);
    int arg2 = gen_expression(arg2_expr, out);
    int result = gen_id();
    fprintf(out, "%%%-3d = OpExtInst %%t_float %%ext Dot %%%-3d %%%-3d\n",
            result, arg1, arg2);
    return result;
```

---

## AST Structure (ast.h, ast.c)

**Node Kinds:**
```c
typedef enum {
    // Expressions
    VAR_NODE,
    BINARY_EXPR_NODE,
    UNARY_EXPR_NODE,
    INT_LITERAL_NODE,
    FLOAT_LITERAL_NODE,
    CONSTRUCTOR_NODE,
    FUNCTION_NODE,
    
    // Statements
    ASSIGNMENT_NODE,
    IF_STMT_NODE,
    SCOPE_NODE,
    
    // Declarations
    DECLARATION_NODE,
    
    // Lists
    DECLARATIONS_NODE,
    STATEMENTS_NODE,
} node_kind;
```

**Node Structure:**
```c
struct node_ {
    node_kind kind;
    union {
        struct { ... } variable;
        struct { ... } binary_expr;
        struct { ... } assignment;
        struct { ... } if_stmt;
        struct { ... } scope;
        struct { ... } declaration;
        ...
    };
};
```

**Scope Node Structure:**
```c
struct {
    node *declarations;  // Linked list of DECLARATION_NODEs
    node *statements;    // Linked list of statement nodes
} scope;
```

---

## Build System (Makefile)

**Build Process:**
```makefile
scanner.l → (flex) → scanner.c
parser.y  → (bison) → parser.c + parser.tab.h
*.c       → (g++) → *.o
*.o       → (linker) → compiler467
```

**Clean Target (lines 52-54):**
```makefile
clean:
    rm -f *.o *~ compiler467 parser.c scanner.c ...
    rm -f *.spvasm *.spv  # Also clean generated SPIR-V
```

---

## Key Design Decisions

### 1. Why Two-Pass Semantic Analysis?
**Problem:** `vec4 color = base;` - `base` used before fully processed

**Solution:** Pass 1 adds all variables to symbol table, Pass 2 checks initializers

**Alternative considered:** Single pass with forward declaration requirements - rejected because reference shaders don't use forward declarations

### 2. Why OpSelect Instead of Branching?
**Requirement:** Spec forbids `OpBranch`, `OpBranchConditional`, `OpPhi`

**Implementation:** Evaluate both branches, select result

**Tradeoff:** May do unnecessary work (evaluating unused branch), but simpler code and spec-compliant

### 3. Why Constant Pooling?
**Requirement:** SPIR-V module structure: types → constants → function

**Problem:** Don't know all constants until generating expressions

**Solution:** Buffer function body in memory, emit constants first, then emit buffered function

**Alternative considered:** Pre-scan AST for constants - rejected as more complex

### 4. Why C99-Style Parser?
**Requirement:** Lab Section 5.1 - must accept ref shaders as-is

**Problem:** `phong.frag` has `vec4 color = base;` after an `if` statement

**Solution:** Allow mixed declarations/statements, split during parsing

**Alternative:** Require manual declaration hoisting - rejected because violates spec

---

## Data Flow Example

**Input:**
```c
{
    float x = 1.0;
    if (x < 2.0) {
        x = 3.0;
    }
}
```

**Scanner Output:**
```
LBRACE, FLOAT_T, ID("x"), ASSIGN, FLOAT_C(1.0), SEMICOLON,
IF, LPAREN, ID("x"), LT, FLOAT_C(2.0), RPAREN, ...
```

**Parser Output (AST):**
```
SCOPE_NODE
  declarations: DECLARATION_NODE("x", float, init=1.0)
  statements: IF_STMT_NODE
                condition: BINARY_EXPR_NODE('<', VAR_NODE("x"), LITERAL(2.0))
                then: ASSIGNMENT_NODE("x", LITERAL(3.0))
```

**Semantic Analysis:**
- Pass 1: Add "x" to symbol table (type=float)
- Pass 2: Check initializer (1.0 is float ✓), check condition types ✓

**Code Generation:**
```spirv
%100 = OpVariable %t_ptr_func_float Function  ; allocate x
%101 = OpConstant %t_float 1.0                 ; literal
OpStore %100 %101                              ; x = 1.0
%102 = OpLoad %t_float %100                    ; load x
%103 = OpConstant %t_float 2.0                 ; literal
%104 = OpFOrdLessThan %t_bool %102 %103       ; x < 2.0
%105 = OpConstant %t_float 3.0                 ; then value
%106 = OpLoad %t_float %100                    ; else value (current x)
%107 = OpSelect %t_float %104 %105 %106       ; select based on condition
OpStore %100 %107                              ; store result
```

---

## Summary

**Pipeline:**
1. **Scanner** - Text → Tokens
2. **Parser** - Tokens → AST (with mixed declaration support)
3. **Semantic** - AST → Type-checked AST (two-pass)
4. **Codegen** - AST → SPIR-V (with OpSelect, constant pooling, type tracking)

**Key Features:**
- C99-style mixed declarations
- Two-pass semantic analysis  
- Linear SSA with OpSelect
- Constant pooling via memory streams
- Type tracking for correct operations

**Files to understand in order:**
1. `ast.h` - Data structures
2. `scanner.l` - Tokenization
3. `parser.y` - Grammar and AST building
4. `semantic.c` - Type checking
5. `codegen.c` - SPIR-V generation
