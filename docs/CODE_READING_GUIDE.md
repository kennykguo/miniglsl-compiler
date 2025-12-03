# Reading Guide for Code Understanding

**How to understand the MiniGLSL compiler codebase**

This guide helps you understand how the compiler works by directing you through the code in the right order.

---

## Start Here

### 1. CODE_ARCHITECTURE.md (Read First!)
**Why:** Overview of the entire system, data flow, and key algorithms

**What you'll learn:**
- How the 4 phases work (scanner → parser → semantic → codegen)
- Data structures (AST, symbol table, variable tracking)
- Key algorithms (OpSelect, constant pooling, two-pass analysis)
- Complete example of input → output transformation

**Time:** 20-30 minutes

### 2. Understanding the AST (ast.h)
**Why:** The AST is the central data structure

**Read in order:**
1. View `/src/ast.h` lines 1-60 (node kinds)
2. View `/src/ast.h` lines 60-157 (node structures)

**Focus on:**
- `node_kind` enum - what kinds of nodes exist
- Union structure - how different node types are represented
- `scope` structure - declarations + statements

**Time:** 10 minutes

---

## Compilation Pipeline (Read in Order)

### 3. Scanner - Tokenization (scanner.l)
**Path:** `/src/scanner.l`

**Read sections:**
1. **Lines 35-86:** Token patterns and rules
   - Keywords: `if`, `else`, `float`, `vec4`
   - Operators: `+`, `-`, `*`, `/`, `&&`, `||`
   - Literals and identifiers
   
2. **Lines 96-177:** Helper functions
   - `ParseComment()` - Skip comments
   - `ParseInt()` - Validate integers
   - `ParseFloat()` - Validate floats
   - `ParseIdent()` - Extract identifiers

**Key insight:** Line 1 MUST have `%{` at column 0 (Flex requirement)

**Time:** 15 minutes

### 4. Parser - AST Building (parser.y)
**Path:** `/src/parser.y`

**Read sections:**
1. **Lines 93-116:** Scope grammar (critical!)
   ```yacc
   scope: '{' statement_list '}'
   ```
   - Allows mixed declarations and statements
   - Splits into separate lists for semantic analysis

2. **Lines 119-146:** Statement list grammar
   - `statement_list -> statement | declaration`
   - This enables C99-style declarations

3. **Lines 148-288:** Individual rules (skim)
   - See how each grammar rule creates AST nodes

**Key insight:** The scope action (lines 95-116) is what enables C99-style mixed declarations

**Time:** 20 minutes

### 5. Semantic Analysis - Type Checking (semantic.c)
**Path:** `/src/semantic.c`

**Read sections in order:**

**Part 1: Two-Pass Design**
1. **Lines 99-141:** `check_node()` for SCOPE_NODE
   - **Pass 1 (107-133):** Add all declarations to symbol table
   - **Pass 2 (136):** Check initializers
   
   **Why two passes?** Handles forward references:
   ```c
   vec4 base = ...;
   vec4 color = base;  // uses 'base' in initializer
   ```

**Part 2: Declaration Checking**
2. **Lines 150-188:** `check_declaration()`
   - Now just checks initializers (symbol already added in pass 1)
   - Validates const initialization rules

**Part 3: Statement Checking**
3. **Lines 190-286:** `check_statement()`
   - Line 194: `DECLARATION_NODE` - handles mixed declarations
   - Line 194-257: `ASSIGNMENT_NODE` - type checking assignments
   - Line 262-275: `IF_STMT_NODE` - validates condition is bool

**Part 4: Expression Type Checking**
4. **Lines 288-597:** `check_expression()` and `get_expression_type()`
   - Recursive type checking
   - Type inference

**Time:** 30 minutes

### 6. Code Generation - SPIR-V Emission (codegen.c)
**Path:** `/src/codegen.c`

This is the most complex file. Read in sections:

**Part 1: Infrastructure (Lines 1-96)**
1. **Lines 13-18:** Constant pool - collects constants for later emission
2. **Lines 49-68:** Variable tracking - remembers types for correct OpLoad
3. **Lines 227-235:** SSA ID allocation - simple counter

**Part 2: Main Generation Loop (Lines 559-586)**
```c
generate_code():
    1. Open memory stream for function body
    2. Generate: OpLabel → variables → statements → OpReturn
    3. Close memory stream
    4. Emit: prologue → constants → function header → body
```

**Key insight:** Memory stream lets us emit constants before function body

**Part 3: Variable Processing (Lines 237-265)**
- Pass 1: Allocate all OpVariable
- Pass 2: Initialize with OpStore

**Part 4: OpSelect Predication (Lines 494-527)** ⭐
```c
IF_STMT_NODE:
    cond = gen_expression(condition)
    then_val = gen_expression(then_expr)
    else_val = gen_expression(else_expr)
    result = OpSelect(cond, then_val, else_val)
    OpStore(var, result)
```

**This is the critical feature!** No branching, pure SSA.

**Part 5: Expression Generation (Lines 267-428)**
- Returns SSA ID of result
- Recursive for nested expressions
- Emits appropriate SPIR-V instruction

**Focus on these patterns:**
- **Lines 271-300:** Variable reference → OpLoad
- **Lines 302-336:** Binary operators → OpFAdd, OpFMul, etc.
- **Lines 371-383:** Constructors → OpCompositeConstruct
- **Lines 384-410:** Function calls → OpExtInst

**Part 6: Indexed Assignment (Lines 441-459)**
```c
temp[0] = value:
    loaded = OpLoad(temp)
    updated = OpCompositeInsert(value, loaded, 0)
    OpStore(temp, updated)
```

**Time:** 45-60 minutes (most complex file)

---

## How Features Work Together

### Example: Understanding OpSelect

**Input code:**
```c
{
    float x = 1.0;
    if (x < 2.0) {
        x = 3.0;
    }
}
```

**Scanner (`scanner.l`):**
- Tokenizes: `LBRACE FLOAT_T ID ASSIGN FLOAT_C ...`

**Parser (`parser.y`):**
- Builds AST:
  ```
  SCOPE_NODE
    declarations: [DECLARATION_NODE("x", float, 1.0)]
    statements: [IF_STMT_NODE]
  ```

**Semantic Analyzer (`semantic.c`):**
- Pass 1: Add "x" to symbol table (type=float)
- Pass 2: Check `1.0` is float ✓, check `x < 2.0` is bool ✓

**Code Generator (`codegen.c`):**
```spirv
%100 = OpVariable %t_ptr_func_float Function
%101 = OpConstant %t_float 1.0
OpStore %100 %101
%102 = OpLoad %t_float %100
%103 = OpConstant %t_float 2.0
%104 = OpFOrdLessThan %t_bool %102 %103
%105 = OpConstant %t_float 3.0
%106 = OpLoad %t_float %100
%107 = OpSelect %t_float %104 %105 %106  ← THE KEY LINE
OpStore %100 %107
```

**The OpSelect line:** "Select 3.0 if condition is true, else current value of x"

### Example: Understanding Two-Pass Semantic

**Input code:**
```c
{
    vec4 base = vec4(1.0, 0.2, 0.2, 1.0);
    vec4 color = base;  // uses 'base' in initializer
}
```

**Single-pass would fail:**
```
Processing Declaration("color", init=VAR_NODE("base"))
  check_expression(VAR_NODE("base"))
    lookup "base" in symbol table
    ❌ ERROR: "base" not found!
```

**Two-pass succeeds:**
```
PASS 1:
  Add "base" to symbol table (skip initializer)
  Add "color" to symbol table (skip initializer)

PASS 2:
  Check "base" initializer: vec4(...) ✓
  Check "color" initializer: lookup "base" ✓ found!
```

---

## Understanding Key Files

### ast.c - AST Operations
**Functions:**
- `ast_allocate()` - Create new AST node
- `ast_free()` - Free AST tree
- `ast_print()` - Debug print AST

**Usage:** Parser calls `ast_allocate()` to build tree

### symbol.c - Symbol Table
**Data Structure:**
```c
symbol_entry {
    name, type_code, vec_size
    is_const, is_read_only, is_write_only
}
```

**Functions:**
- `symbol_table_create()` - New scope
- `symbol_table_insert()` - Add variable
- `symbol_table_lookup()` - Find variable
- `symbol_table_free()` - Clean up

**Usage:** Semantic analyzer maintains stack of scopes

---

## Code Reading Checklist

After reading, you should understand:

**Scanner:**
- [ ] How text becomes tokens
- [ ] What token types exist
- [ ] How comments are handled

**Parser:**
- [ ] How tokens become AST
- [ ] Why scope allows mixed declarations
- [ ] How grammar rules map to AST nodes

**Semantic:**
- [ ] Why two-pass is necessary
- [ ] How symbol table works
- [ ] How type checking validates code

**Codegen:**
- [ ] Why constant pooling is needed
- [ ] How OpSelect replaces branching
- [ ] How SSA IDs are allocated
- [ ] How expressions generate SPIR-V

**Overall:**
- [ ] Data flow from text → tokens → AST → SPIR-V
- [ ] Why each design decision was made
- [ ] How the pieces fit together

---

## Recommended Reading Order Summary

1. **CODE_ARCHITECTURE.md** (overview)
2. **ast.h** (data structures)
3. **scanner.l** (tokenization)
4. **parser.y** (AST building, focus on scope)
5. **semantic.c** (two-pass type checking)
6. **codegen.c** (SPIR-V generation, focus on OpSelect)

**Total time:** ~2.5-3 hours for deep understanding

---

## Quick Reference

**To understand a specific feature:**
- Mixed declarations → `parser.y` lines 93-146
- Two-pass analysis → `semantic.c` lines 99-141
- OpSelect predication → `codegen.c` lines 494-527
- Constant pooling → `codegen.c` lines 559-586
- Indexed assignment → `codegen.c` lines 441-459
- Type tracking → `codegen.c` lines 49-96

**To trace a compilation:**
1. Start with input code
2. Trace through scanner.l (tokens)
3. Trace through parser.y (AST)
4. Trace through semantic.c (symbol table)
5. Trace through codegen.c (SPIR-V)
