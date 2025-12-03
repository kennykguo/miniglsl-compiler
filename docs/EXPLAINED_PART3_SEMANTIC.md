# The Complete Compiler Walkthrough - Part 3: Semantic Analysis

**Understanding Type Checking and the Two-Pass Solution**

---

## What is Semantic Analysis?

The parser built an AST, but it didn't check if the code **makes sense**.

**Things the parser DOESN'T check:**
- Does variable `x` exist?
- Is `x + y` valid? (Are types compatible?)
- Is `if (5)` valid? (Should be boolean!)
- Can you read from `gl_FragColor`? (It's write-only!)

**Semantic analysis checks all of this.**

**Analogy:**
- **Parser says:** "This is a grammatically correct sentence"
- **Semantic analyzer says:** "This sentence makes logical sense"

Example:
- "Colorless green ideas sleep furiously" - grammatical, but nonsense!
- `float x = "hello";` - parses fine, but semantically wrong!

---

## The Symbol Table

**File:** `src/symbol.c`

Before checking if code makes sense, we need to **remember what variables exist**.

### What's in a Symbol Table?

**Symbol entry structure (symbol.h):**
```c
typedef struct symbol_entry {
    char *name;           // Variable name like "x" or "color"
    int type_code;        // FLOAT_T, INT_T, VEC_T, BOOL_T
    int vec_size;         // 0 for scalar, 1 for vec2, 2 for vec3, 3 for vec4
    int is_const;         // true if declared with 'const'
    int is_read_only;     // true for gl_Color, env1, etc.
    int is_write_only;    // true for gl_FragColor
    struct symbol_entry *next;  // Linked list
} symbol_entry;
```

**Example entries:**
```c
// float x = 1.0;
{ name="x", type_code=FLOAT_T, vec_size=0, is_const=0, ... }

// const vec4 c = vec4(1,0,0,1);
{ name="c", type_code=VEC_T, vec_size=3, is_const=1, ... }

// Built-in: gl_Color (read-only attribute)
{ name="gl_Color", type_code=VEC_T, vec_size=3, is_read_only=1, ... }
```

### Scoping: Nested Symbol Tables

Variables have **scope** - where they're visible:

```c
{                          // Outer scope
    float x = 1.0;
    {                      // Inner scope
        float y = 2.0;
        // Can see both x and y here
    }
    // Can only see x here (y is out of scope)
}
```

**Implementation:** Stack of symbol tables

```c
struct scope_table {
    struct symbol_entry *symbols;   // Linked list of symbols
    struct scope_table *parent;     // Outer scope
};
```

**Lookup process:**
1. Look in current scope
2. If not found, look in parent scope
3. Repeat until found or reach global scope
4. If still not found → error: "undeclared variable"

**Example:**
```
Global scope: { gl_Color, gl_FragCoord, ... }
   ↑
Outer scope: { x }
   ↑
Inner scope: { y }
```

Looking up `y`: Found in inner scope ✓
Looking up `x`: Not in inner, check outer → Found ✓
Looking up `z`: Not in inner, not in outer, not in global → ERROR

---

## The Critical Problem: Forward References

**File:** `src/semantic.c`

### The Problem

Consider this code:
```c
{
    vec4 base = vec4(1.0, 0.2, 0.2, 1.0);
    vec4 color = base;  // Uses 'base' in initializer!
}
```

**Single-pass attempt:**

1. Process `vec4 base = vec4(...)`:
   - Add "base" to symbol table ✓
   - Check initializer `vec4(...)` ✓
   
2. Process `vec4 color = base`:
   - Add "color" to symbol table
   - Check initializer `base`
   - Look up "base" in symbol table...
   
**Problem:** When does "base" get added to the symbol table?

**Old approach:**
```c
process_declaration(decl) {
    check_initializer(decl->initializer);  // Check first
    add_to_symbol_table(decl);             // Add second
}
```

This **fails** because checking the initializer happens BEFORE the variable is added!

**Why?** When processing `vec4 color = base`:
- Check initializer: lookup "base" → NOT FOUND ❌
- Add "color" to table (too late!)

### The Two-Pass Solution

**Key insight:** Declarations can reference OTHER declarations from the same scope.

**Solution:** Process all declarations in TWO PASSES:

**Pass 1: Add all variables to symbol table (SKIP initializers)**
```c
for each declaration:
    // Just add the name and type, don't check initializer yet
    symbol_entry *entry = create_entry(decl->identifier, decl->type);
    add_to_symbol_table(entry);
```

**Pass 2: Check all initializers**
```c
for each declaration:
    // Now all variables are in the table, safe to check initializers
    if (decl->initializer) {
        check_expression(decl->initializer);
    }
```

**Example with two-pass:**

**Pass 1:**
```c
{
    vec4 base = ...;    // Add "base" to table (skip initializer)
    vec4 color = base;  // Add "color" to table (skip initializer)
}
```

**Symbol table after pass 1:**
```
{ "base" -> vec4, "color" -> vec4 }
```

**Pass 2:**
```c
{
    vec4 base = vec4(...);  // Check vec4(...) ✓
    vec4 color = base;      // Lookup "base" → FOUND ✓
}
```

Both initializers check successfully!

---

## Implementation: The Two-Pass Code

**Location:** `src/semantic.c` lines 99-141

### check_node() for SCOPE_NODE

```c
case SCOPE_NODE: {
    // Create new scope
    symbol_table *new_scope = symbol_table_create();
    symbol_table *saved_scope = current_scope;
    current_scope = new_scope;
    new_scope->parent = saved_scope;
    
    // PASS 1: Add all declarations to symbol table
    node *curr_decl = ast->scope.declarations;
    while (curr_decl) {
        node *decl = curr_decl->list.item;
        if (decl && decl->kind == DECLARATION_NODE) {
            // Create symbol entry
            symbol_entry *entry = (symbol_entryp*)malloc(sizeof(symbol_entry));
            entry->name = strdup(decl->declaration.identifier);
            entry->type_code = decl->declaration.type->type.type_code;
            entry->vec_size = decl->declaration.type->type.vec_size;
            entry->is_const = decl->declaration.is_const;
            
            // Add to table (DON'T check initializer yet!)
            symbol_table_insert(current_scope, entry);
        }
        curr_decl = curr_decl->list.next;
    }
    
    // PASS 2: Check initializers
    process_declarations_in_order(ast->scope.declarations);
    
    // Process statements
    process_statements_in_order(ast->scope.statements);
    
    // Restore parent scope
    current_scope = saved_scope;
    symbol_table_free(new_scope);
    break;
}
```

**Breaking this down:**

**Setup:**
```c
symbol_table *new_scope = symbol_table_create();
current_scope = new_scope;
new_scope->parent = saved_scope;
```
Create a new scope for this `{...}` block, link it to parent scope.

**Pass 1 loop:**
```c
while (curr_decl) {
    node *decl = curr_decl->list.item;
    ...
    symbol_table_insert(current_scope, entry);
    curr_decl = curr_decl->list.next;
}
```
Walk the linked list of declarations, adding each to symbol table.

**Pass 2:**
```c
process_declarations_in_order(ast->scope.declarations);
```
This function calls `check_declaration()` which NOW checks initializers (variables already in table!).

**Cleanup:**
```c
current_scope = saved_scope;
symbol_table_free(new_scope);
```
Restore parent scope, free the local scope (variables go out of scope).

---

## Type Checking Expressions

### check_expression() - The Recursive Type Checker

**Purpose:** Verify an expression is type-correct and return its type.

**Location:** `src/semantic.c` lines 288-597

### Example: Binary Expression

```c
x + y
```

**Code (simplified):**
```c
case BINARY_EXPR_NODE: {
    int left_type = check_expression(expr->binary_expr.left);
    int right_type = check_expression(expr->binary_expr.right);
    
    if (left_type != right_type) {
        fprintf(errorFile, "error - type mismatch in binary operation\n");
        return -1;
    }
    
    return left_type;  // Result has same type as operands
}
```

**Why recursive?** Expressions nest!

```c
(x + y) * z
```

**Checking tree:**
```
BINARY_EXPR('*')
  ├─ left: BINARY_EXPR('+')
  │   ├─ left: VAR("x")
  │   └─ right: VAR("y")
  └─ right: VAR("z")
```

**Checking process:**
1. Check `BINARY_EXPR('*')`:
   - Check left child `BINARY_EXPR('+')`:
     - Check left child `VAR("x")`  → float
     - Check right child `VAR("y")` → float
     - Return float
   - Check right child `VAR("z")` → float
   - Both float → valid!
   - Return float

### Example: Variable Reference

```c
x
```

Must check:
1. Does `x` exist?
2. What's its type?

**Code (simplified):**
```c
case VAR_NODE: {
    symbol_entry *entry = symbol_table_lookup(current_scope, expr->variable.identifier);
    
    if (!entry) {
        fprintf(errorFile, "error - undeclared variable '%s'\n", 
                expr->variable.identifier);
        return -1;
    }
    
    return entry->type_code;
}
```

**What `symbol_table_lookup` does:**
```c
symbol_table_lookup(scope, "x") {
    // Search current scope
    for (entry in scope->symbols) {
        if (strcmp(entry->name, "x") == 0)
            return entry;  // Found!
    }
    
    // Not in current scope, try parent
    if (scope->parent)
        return symbol_table_lookup(scope->parent, "x");
    
    return NULL;  // Not found anywhere
}
```

### Example: Constructor

```c
vec4(1.0, 2.0, 3.0, 4.0)
```

Must check:
1. Are there 4 arguments? (vec4 needs 4)
2. Are all arguments floats?

**Code (simplified):**
```c
case CONSTRUCTOR_NODE: {
    int expected_count = expr->constructor.type->type.vec_size + 1;
    
    // Count and check arguments
    int arg_count = 0;
    node *arg = expr->constructor.arguments;
    while (arg) {
        int arg_type = check_expression(arg->list.item);
        if (arg_type != FLOAT_T) {
            fprintf(errorFile, "error - constructor argument type mismatch\n");
            return -1;
        }
        arg_count++;
        arg = arg->list.next;
    }
    
    if (arg_count != expected_count) {
        fprintf(errorFile, "error - wrong number of constructor arguments\n");
        return -1;
    }
    
    return expr->constructor.type->type.type_code;
}
```

**Why count + check?**
- `vec3(1.0, 2.0)` - too few arguments ❌
- `vec3(1.0, 2.0, 3.0)` - just right ✓
- `vec3(1.0, 2.0, 3.0, 4.0)` - too many ❌

### Example: Function Call (dp3)

```c
dp3(normal, lightDir)
```

Must check:
1. Are both arguments vec3 or vec4?
2. Are they the same type?

**Code (simplified):**
```c
case FUNCTION_NODE with DP3: {
    node *arg1 = expr->function.arguments->list.item;
    node *arg2 = expr->function.arguments->list.next->list.item;
    
    int type1 = check_expression(arg1);
    int type2 = check_expression(arg2);
    
    if (type1 != type2) {
        fprintf(errorFile, "error - dp3 arguments must have same type\n");
        return -1;
    }
    
    if (type1 != VEC_T) {
        fprintf(errorFile, "error - dp3 requires vector arguments\n");
        return -1;
    }
    
    return FLOAT_T;  // dp3 always returns float
}
```

**Why dp3 is special:**
- `dp3(vec3, vec3)` → valid, returns float
- `dp3(vec4, vec4)` → valid, returns float
- `dp3(float, float)` → INVALID, dp3 needs vectors
- `dp3(vec3, vec4)` → INVALID, types don't match

---

## Checking Statements

### Assignment Statement

```c
x = expr;
```

Must check:
1. Does `x` exist?
2. Does `expr` type match `x` type?
3. Is `x` writable? (Not const, not read-only)

**Code (lines 194-257):**
```c
case ASSIGNMENT_NODE: {
    // Look up variable
    symbol_entry *entry = symbol_table_lookup(current_scope, 
                                               stmt->assignment.variable->variable.identifier);
    if (!entry) {
        fprintf(errorFile, "error - undeclared variable\n");
        return -1;
    }
    
    // Check if writable
    if (entry->is_const) {
        fprintf(errorFile, "error - cannot assign to const variable\n");
        return -1;
    }
    
    if (entry->is_read_only) {
        fprintf(errorFile, "error - cannot assign to read-only variable\n");
        return -1;
    }
    
    // Check expression type matches variable type
    int expr_type = check_expression(stmt->assignment.expr);
    if (expr_type != entry->type_code) {
        fprintf(errorFile, "error - assignment type mismatch\n");
        return -1;
    }
    
    return 0;
}
```

**Example violations:**

```c
const float pi = 3.14;
pi = 3.0;  // ERROR: cannot assign to const ❌
```

```c
gl_Color = vec4(1,0,0,1);  // ERROR: gl_Color is read-only ❌
```

```c
float x;
x = vec4(1,0,0,1);  // ERROR: type mismatch (float vs vec4) ❌
```

### If Statement

```c
if (condition) {
    ...
}
```

Must check:
1. Is `condition` a boolean?
2. Are statements in body valid?

**Code (lines 262-275):**
```c
case IF_STMT_NODE: {
    // Check condition is boolean
    int cond_type = check_expression(stmt->if_stmt.condition);
    if (cond_type != BOOL_T) {
        fprintf(errorFile, "error - if condition must be boolean\n");
        return -1;
    }
    
    // Check then branch
    check_statement(stmt->if_stmt.then_stmt);
    
    // Check else branch if present
    if (stmt->if_stmt.else_stmt) {
        check_statement(stmt->if_stmt.else_stmt);
    }
    
    return 0;
}
```

**Example violations:**

```c
if (5) { ... }  // ERROR: 5 is int, not bool ❌
```

```c
if (x + y) { ... }  // ERROR: x+y is float, not bool ❌
```

```c
if (x < y) { ... }  // OK: x < y is bool ✓
```

---

## Special Case: Mixed Declarations in Statements

Remember from Part 2, the parser allows:
```c
{
    float x = 1.0;
    if (x < 2.0) { }
    float y = 2.0;  // Declaration after statement!
}
```

**How semantic analysis handles this:**

In `check_statement()` (line 194):
```c
case DECLARATION_NODE:
    // Handle declarations that appear mixed with statements
    check_declaration(stmt);
    break;
```

When processing the statements list, we might encounter `DECLARATION_NODE` items. We just check them like normal declarations.

**But wait!** Didn't we already add them to the symbol table in Pass 1?

**Yes!** The Pass 1 loop (lines 107-133) processed the **declarations list** (split by parser).

**This case** handles declarations that somehow ended up in the **statements list** (shouldn't happen with our parser, but defensive programming).

---

## Built-in Variables: Predefined Symbols

MiniGLSL has built-in variables like `gl_Color`, `gl_FragColor`, etc.

**Initialization (lines 57-96):**
```c
symbol_entry *gl_color = malloc(sizeof(symbol_entry));
gl_color->name = strdup("gl_Color");
gl_color->type_code = VEC_T;
gl_color->vec_size = 3;  // vec4
gl_color->is_read_only = 1;  // Attribute - can't write
symbol_table_insert(global_scope, gl_color);

symbol_entry *gl_frag_color = malloc(sizeof(symbol_entry));
gl_frag_color->name = strdup("gl_FragColor");
gl_frag_color->type_code = VEC_T;
gl_frag_color->vec_size = 3;  // vec4
gl_frag_color->is_write_only = 1;  // Result - can't read
symbol_table_insert(global_scope, gl_frag_color);
```

**Why predefined?** So they're always available, don't need to be declared.

**Why is_read_only / is_write_only?**
- `gl_Color` is an **input** (attribute) - can read, can't write
- `gl_FragColor` is an **output** (result) - can write, can't read
- `env1` is a **uniform** - can read, can't write

**Enforced by semantic analysis:**
```c
if (entry->is_read_only && we're_writing) {
    ERROR("can't write to read-only variable");
}
if (entry->is_write_only && we're_reading) {
    ERROR("can't read from write-only variable");
}
```

---

## Complete Example: Checking a Program

**Input code:**
```c
{
    float x = 1.0;
    float y = x + 2.0;
    if (y < 5.0) {
        gl_FragColor = vec4(y, y, y, 1.0);
    }
}
```

**Semantic analysis steps:**

**1. Enter scope, create new symbol table**

**2. Pass 1 - Add declarations:**
- Add "x" (float) to table
- Add "y" (float) to table

**Symbol table:**
```
{ "x" -> float, "y" -> float }
+ parent (global scope with gl_FragColor, etc.)
```

**3. Pass 2 - Check declarations:**

**Check `float x = 1.0;`:**
- Initializer: `1.0` is FLOAT_LITERAL → float ✓
- Matches variable type (float) ✓

**Check `float y = x + 2.0;`:**
- Initializer: `x + 2.0`
  - Check `x`: lookup in table → found, type float ✓
  - Check `2.0`: FLOAT_LITERAL → float ✓
  - Check `+`: both operands float → result float ✓
- Matches variable type (float) ✓

**4. Check statements:**

**Check `if (y < 5.0) { ... }`:**
- Condition: `y < 5.0`
  - Check `y`: lookup → found, float ✓
  - Check `5.0`: → float ✓
  - Check `<`: both float → result bool ✓
- Condition is bool ✓
- Check then branch:
  - New scope created for `{ ... }`
  - Check assignment `gl_FragColor = vec4(...)`
    - Lookup `gl_FragColor`: found (in global scope) ✓
    - Check is_write_only: yes, and we're writing ✓
    - Check expression `vec4(y, y, y, 1.0)`:
      - Constructor type: vec4
      - Args: y (float), y (float), y (float), 1.0 (float) ✓
      - Count: 4 (correct for vec4) ✓
      - Result: vec4 ✓
    - Type matches gl_FragColor (vec4) ✓

**5. Exit scope, free local symbol table**

**Result:** Program is semantically valid! ✓

---

## Why Two-Pass is Necessary: A Concrete Example

**Consider this code:**
```c
{
    vec4 a = b;  // Uses b
    vec4 b = c;  // Uses c
    vec4 c = vec4(1,0,0,1);  // Finally, a concrete value!
}
```

**Single-pass would fail:**

Step 1: Process `vec4 a = b`
- Add "a" to table
- Check initializer: lookup "b" → NOT FOUND ❌

**Two-pass succeeds:**

**Pass 1:**
- Add "a" to table
- Add "b" to table
- Add "c" to table

**Pass 2:**
- Check `a = b`: lookup "b" → FOUND ✓
- Check `b = c`: lookup "c" → FOUND ✓
- Check `c = vec4(...)`: valid constructor ✓

All three initializers check successfully!

**Key insight:** In a given scope, declarations can reference each other in any order, as long as they're all in the same scope.

---

## Summary: Semantic Analysis (semantic.c)

**Purpose:** Verify code is semantically correct (types match, variables exist, etc.)

**Input:** AST from parser

**Output:** Type-checked AST (or errors if invalid)

**Key Data Structure: Symbol Table**
- Maps variable names to types and properties
- Organized in scope stack (nested scopes)
- Includes built-in variables (gl_Color, gl_FragColor, etc.)

**Two-Pass Declaration Processing (Critical!):**
- **Pass 1:** Add all declarations to symbol table
- **Pass 2:** Check all initializers
- **Why:** Handles forward references (`vec4 color = base;`)

**Responsibilities:**
1. **Variable lookup** - Does variable exist? What's its type?
2. **Type checking** - Do types match in operations?
3. **Qualifier checking** - Can't write to read-only, can't read from write-only
4. **Scope management** - Create/destroy scopes, nest properly
5. **Expression type inference** - What type does this expression produce?

**Key Checks:**
- Assignment: types match, variable is writable
- Binary operations: operand types compatible
- If statements: condition is boolean
- Constructors: right number and types of arguments
- Function calls: argument types valid for overloaded function

**Symbol Table Operations:**
- `symbol_table_create()` - New scope
- `symbol_table_insert()` - Add variable
- `symbol_table_lookup()` - Find variable (search up scope chain)
- `symbol_table_free()` - Destroy scope

**Next Stage:** Type-checked AST goes to Code Generator to produce SPIR-V

---

**Continue to Part 4 for Code Generation explanation...**
