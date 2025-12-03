# The Complete Compiler Walkthrough - Part 2: Parser

**Understanding How Tokens Become a Tree Structure**

---

## What is Parsing?

The parser takes the stream of tokens from the scanner and organizes them into a **tree structure** called an Abstract Syntax Tree (AST).

**Why a tree?** Code has **hierarchy**:
```c
{                          // Scope
    float x = 1.0;         //   Declaration
    if (x < 2.0) {        //   If statement
        x = 3.0;           //     Assignment
    }
}
```

The `if` contains an assignment. The scope contains a declaration and an `if`. This nesting creates a tree!

---

## From Tokens to Tree

**Remember Part 1 output:** Stream of tokens
```
VEC4_T ID("color") ASSIGN VEC4_T LPAREN FLOAT_C(1.0) ...
```

**Parser output:** Tree structure
```
SCOPE_NODE
  ├─ declarations:
  │   └─ DECLARATION_NODE
  │       ├─ type: vec4
  │       ├─ name: "color"
  │       └─ initializer: CONSTRUCTOR_NODE
  │           ├─ type: vec4
  │           └─ arguments: [1.0, 0.5, 0.2, 1.0]
  └─ statements: (empty)
```

---

## The Grammar File: parser.y

**File:** `src/parser.y` (302 lines)

### Structure of a Bison/Yacc File

**Section 1: C Declarations (lines 1-12)**
```yacc
%{
#include <stdio.h>
#include "ast.h"
...
%}
```
C code and #includes that get copied into the generated parser.

**Section 2: Bison Declarations (lines 24-91)**
```yacc
%token FLOAT_T VEC4_T IF ELSE
%type <as_node> expression statement
...
```
Declares tokens and types.

**Section 3: Grammar Rules (lines 93-288)**
```yacc
scope: '{' statement_list '}'
    {
        $$ = ... // build AST node
    }
```
The heart of the parser - defines the language structure.

**Section 4: C Code (lines 290-302)**
```yacc
%%
int yyerror(const char *s) { ... }
```
Helper functions.

---

## Understanding Grammar Rules

A grammar rule has this format:
```yacc
result: pattern1 pattern2 pattern3
    {
        // C action code to build AST
        $$ = the_result;
        // Can reference: $1 (pattern1), $2 (pattern2), $3 (pattern3)
    }
```

### Example Rule: Simple Assignment

**Rule (lines 224-231):**
```yacc
statement: ID ASSIGN expression SEMICOLON
    {
        $$ = ast_allocate(ASSIGNMENT_NODE, 
                         $1,  /* ID */
                         $3); /* expression */
    }
```

**Breaking this down:**

**The pattern:** `ID ASSIGN expression SEMICOLON`
- Matches: `x = 1.0;`  or `color = vec4(...);`

**The action:** Build an `ASSIGNMENT_NODE`
- `$1` refers to the ID token (the variable name `"x"`)
- `$3` refers to the expression node (what's being assigned)
- `$$` is what this rule produces (the assignment node)

**In memory, this creates:**
```
ASSIGNMENT_NODE
  ├─ variable: "x"
  └─ expr: <whatever the expression node is>
```

### Example Rule: If Statement

**Rule (lines 262-275):**
```yacc
statement: IF LPAREN expression RPAREN statement %prec WITHOUT_ELSE
    {
        $$ = ast_allocate(IF_STMT_NODE,
                         $3,   /* condition */
                         $5,   /* then statement */
                         NULL); /* no else */
    }
         | IF LPAREN expression RPAREN statement ELSE statement
    {
        $$ = ast_allocate(IF_STMT_NODE,
                         $3,   /* condition */
                         $5,   /* then statement */
                         $7);  /* else statement */
    }
```

**Two patterns for if statements:**

**Pattern 1:** `if (cond) stmt` (no else)
```c
if (x < 2.0)
    x = 3.0;
```
Creates: `IF_STMT_NODE(condition, then_stmt, NULL)`

**Pattern 2:** `if (cond) stmt else stmt`
```c
if (x < 2.0)
    x = 3.0;
else
    x = 4.0;
```
Creates: `IF_STMT_NODE(condition, then_stmt, else_stmt)`

**The `%prec WITHOUT_ELSE` trick:**

This resolves the "dangling else" ambiguity. Consider:
```c
if (a)
    if (b)
        x();
    else    // Which 'if' does this belong to?
        y();
```

The `%prec` tells the parser: "the else belongs to the nearest if."

---

## The Critical Feature: Mixed Declarations

**The Problem We Solved**

Original grammar (old version):
```yacc
scope: '{' declarations statements '}'
```

This forced ALL declarations before ALL statements:
```c
{
    // declarations section
    float x;
    float y;
    
    // statements section
    x = 1.0;
    y = 2.0;
}
```

But reference shaders do this (C99 style):
```c
{
    float x = 1.0;     // declaration
    if (x < 2.0) {}    // statement
    float y = x;       // declaration AFTER statement!
}
```

### Our Solution: Statement List

**New grammar (lines 93-116):**
```yacc
scope: '{' statement_list '}'
    {
        node *decls = NULL;
        node *stmts = NULL;
        node *curr = $2;
        
        // Split statement_list into declarations and statements
        while (curr) {
            node *item = curr->list.item;
            if (item && item->kind == DECLARATION_NODE) {
                decls = ast_allocate(DECLARATIONS_NODE, item, decls);
            } else if (item) {
                stmts = ast_allocate(STATEMENTS_NODE, item, stmts);
            }
            curr = curr->list.next;
        }
        
        $$ = ast_allocate(SCOPE_NODE, decls, stmts);
    }

statement_list:
    /* empty */ { $$ = NULL; }
    | statement_list statement { $$ = ast_allocate(LIST_NODE, $2, $1); }
    | statement_list declaration { $$ = ast_allocate(LIST_NODE, $2, $1); }
```

**How this works:**

**Step 1:** Parse everything into a mixed list
```c
{
    float x = 1.0;     // added to statement_list
    if (x < ...) {}    // added to statement_list
    float y = x;       // added to statement_list
}
```

The `statement_list` becomes a linked list:
```
LIST_NODE → LIST_NODE → LIST_NODE → NULL
    ↓           ↓           ↓
   DECL(x)    IF_STMT    DECL(y)
```

**Step 2:** Split the list during scope action

The `while (curr)` loop walks the list and checks `item->kind`:
- If `DECLARATION_NODE` → add to `decls` list
- Otherwise → add to `stmts` list

**Result:**
```
SCOPE_NODE
  ├─ declarations: DECL(x) → DECL(y) → NULL
  └─ statements: IF_STMT → NULL
```

**Why split?** Later stages (semantic analysis, codegen) expect:
- All declarations in one place (for symbol table)
- All statements in another (for code generation)

This splitting gives them what they need while allowing C99-style source code!

---

## Expression Parsing: Handling Operator Precedence

**Challenge:** How do we parse `1 + 2 * 3` correctly?

Answer: Should be `1 + (2 * 3)` not `(1 + 2) * 3`

**Precedence rules (lines 70-74):**
```yacc
%left OR
%left AND
%left EQ NEQ
%left '<' '>' LEQ GEQ
%left '+' '-'
%left '*' '/'
%right UMINUS '!'
```

**Reading this:**
- `%left` means operator is left-associative
- Lines listed from **lowest** to **highest** precedence

**Example:** `*` has higher precedence than `+`

So `1 + 2 * 3` is parsed as:
```
BINARY_EXPR_NODE('+')
  ├─ left: INT_LITERAL(1)
  └─ right: BINARY_EXPR_NODE('*')
              ├─ left: INT_LITERAL(2)
              └─ right: INT_LITERAL(3)
```

The `*` is evaluated first (deeper in tree), then `+`.

### Unary Minus: The UMINUS Trick

**Problem:** `-` can mean two things:
```c
x - y    // Binary minus (subtraction)
-x       // Unary minus (negation)
```

**Solution (lines 78-82):**
```yacc
%right UMINUS '!'
```

`UMINUS` is a phantom token - it's never actually emitted by the scanner! It's used only for precedence.

**Rule for unary minus (lines 339-343):**
```yacc
expression: '-' expression %prec UMINUS
    {
        $$ = ast_allocate(UNARY_EXPR_NODE, '-', $2);
    }
```

The `%prec UMINUS` says: "this minus has UMINUS precedence" (higher than binary minus).

---

## Special Case: Vector Constructors

**Parsing this:**
```c
vec4(1.0, 0.5, 0.2, 1.0)
```

**Rule (lines 358-361):**
```yacc
expression: VEC4_T LPAREN arguments_opt RPAREN
    {
        $$ = ast_allocate(CONSTRUCTOR_NODE, 
                         VEC4_ype,  /* type */
                         $3);       /* arguments */
    }
```

**Arguments list (lines 376-385):**
```yacc
arguments_opt:
    /* empty */ { $$ = NULL; }
    | arguments

arguments:
    expression
    | arguments COMMA expression 
        { $$ = ast_allocate(LIST_NODE, $3, $1); }
```

**Parsing `vec4(1.0, 0.5, 0.2, 1.0)`:**

1. See `VEC4_T` → know it's a vec4 constructor
2. See `LPAREN`
3. Parse first argument: `1.0` → FLOAT_LITERAL_NODE
4. See `COMMA`, parse second: `0.5` → FLOAT_LITERAL_NODE
5. Build list: `LIST_NODE(0.5, LIST_NODE(1.0, NULL))`
6. Continue for `,  0.2` and `, 1.0`
7. Final: Constructor with 4 arguments in a linked list

**Why linked lists?** We don't know ahead of time how many arguments there are. Linked lists grow dynamically.

---

## Function Calls: Handling Overloading

MiniGLSL has `dp3` which is overloaded:
```c
float dp3(vec3, vec3);
float dp3(vec4, vec4);
int dp3(ivec3, ivec3);
int dp3(ivec4, ivec4);
```

**Parsing (lines 362-367):**
```yacc
expression: DP3 LPAREN arguments_opt RPAREN
    {
        $$ = ast_allocate(FUNCTION_NODE,
                         DP3,  /* function ID */
                         $3);  /* arguments */
    }
```

**Parser doesn't resolve which version!** It just creates a `FUNCTION_NODE(DP3, args)`.

**Semantic analyzer's job:** Look at argument types and pick the right version:
- If args are `vec3` → it's the `float dp3(vec3, vec3)` version
- If args are `vec4` → it's the `float dp3(vec4, vec4)` version

Parser's job is simpler: just recognize the syntax and build the tree.

---

## The AST Data Structure

**Defined in:** `src/ast.h`

**Node kinds (lines 21-45):**
```c
typedef enum {
    VAR_NODE,
    BINARY_EXPR_NODE,
    UNARY_EXPR_NODE,
    INT_LITERAL_NODE,
    FLOAT_LITERAL_NODE,
    CONSTRUCTOR_NODE,
    FUNCTION_NODE,
    ASSIGNMENT_NODE,
    IF_STMT_NODE,
    SCOPE_NODE,
    DECLARATION_NODE,
    ...
} node_kind;
```

**Node structure (lines 66-157):**
```c
struct node_ {
    node_kind kind;
    
    union {
        struct { char *identifier; bool is_array; ... } variable;
        struct { int op; node *left; node *right; } binary_expr;
        struct { int op; node *operand; } unary_expr;
        struct { int val; } int_literal;
        struct { float val; } float_literal;
        struct { node *variable; node *expr; } assignment;
        struct { node *condition; node *then_stmt; node *else_stmt; } if_stmt;
        struct { node *declarations; node *statements; } scope;
        ...
    };
};
```

**Why a union?** Different node types need different fields:
- `BINARY_EXPR_NODE` needs `left` and `right` child nodes
- `INT_LITERAL_NODE` needs just an `int val`
- `IF_STMT_NODE` needs `condition`, `then_stmt`, `else_stmt`

A union lets us have different fields for different cases while keeping the node structure compact.

---

## Complete Example: Parsing a Declaration

**Input code:**
```c
{
    vec4 color = vec4(1.0, 0.5, 0.2, 1.0);
}
```

**Token stream from scanner:**
```
LBRACE VEC4_T ID("color") ASSIGN VEC4_T LPAREN
FLOAT_C(1.0) COMMA FLOAT_C(0.5) COMMA 
FLOAT_C(0.2) COMMA FLOAT_C(1.0) RPAREN SEMICOLON RBRACE
```

**Parsing steps:**

**1. Parse scope:**
```yacc
scope: '{' statement_list '}'
```
Matches `LBRACE`, now parse `statement_list`

**2. Parse statement_list:**
```yacc
statement_list: statement_list declaration
```
First `statement_list` is empty, now parse `declaration`

**3. Parse declaration:**
```yacc
declaration: VEC4_T ID ASSIGN expression SEMICOLON
```
- `VEC4_T` matches
- `ID` matches (`"color"`)
- `ASSIGN` matches
- Now parse `expression`...

**4. Parse expression (constructor):**
```yacc
expression: VEC4_T LPAREN arguments_opt RPAREN
```
- `VEC4_T` matches
- `LPAREN` matches
- Parse arguments...

**5. Parse arguments:**
```yacc
arguments: expression COMMA expression COMMA ...
```
Each `expression` matches a `FLOAT_C`

**6. Build list:**
```
arguments = LIST(1.0, LIST(0.5, LIST(0.2, LIST(1.0, NULL))))
```

**7. Build constructor:**
```
CONSTRUCTOR_NODE(vec4, arguments)
```

**8. Build declaration:**
```
DECLARATION_NODE("color", vec4, CONSTRUCTOR_NODE(...))
```

**9. Build statement_list:**
```
LIST_NODE(DECLARATION_NODE(...), NULL)
```

**10. Build scope (with splitting action):**
```
SCOPE_NODE
  declarations: DECLARATION_NODE(...)
  statements: NULL
```

**Final AST:**
```
SCOPE_NODE
  ├─ declarations:
  │   └─ DECLARATION_NODE
  │       ├─ type: vec4
  │       ├─ identifier: "color"
  │       └─ initializer: CONSTRUCTOR_NODE
  │           ├─ type: vec4
  │           └─ arguments: LIST_NODE chain [1.0, 0.5, 0.2, 1.0]
  └─ statements: NULL
```

This tree structure is what the **semantic analyzer** receives!

---

## Why We Need the Parser

**Question:** Can't we go straight from tokens to SPIR-V?

**Answer:** No! We need structure first.

**Example problem:**
```c
if (x < 2.0)
    if (y < 3.0)
        a = 1.0;
    else
        a = 2.0;
```

**As tokens:** Just a flat stream
```
IF LPAREN ID LT FLOAT_C RPAREN IF LPAREN ID LT FLOAT_C RPAREN ...
```

**As AST:** Shows the nesting relationship
```
IF_STMT
  ├─ condition: x < 2.0
  └─ then: IF_STMT
            ├─ condition: y < 3.0
            ├─ then: a = 1.0
            └─ else: a = 2.0
```

The tree makes it clear: the `else` belongs to the inner `if`, not the outer one!

Without the tree, we wouldn't know the structure.

---

## Summary: Parser (parser.y)

**Purpose:** Convert token stream into Abstract Syntax Tree (AST)

**Input:** Stream of tokens (from scanner)

**Output:** Tree structure (AST) representing program structure

**Key Responsibilities:**
1. **Match grammar patterns** - Recognize valid MiniGLSL syntax
2. **Build AST nodes** - Create tree structure
3. **Handle precedence** - Ensure `2 * 3 + 1` is parsed correctly
4. **Support mixed declarations** - Allow C99-style declaration anywhere
5. **Split declarations/statements** - Separate for later processing

**Critical Feature: C99-Style Grammar**
- Allows declarations after statements: `{ stmt1; float x; stmt2; }`
- Splits mixed list during scope action
- Maintains compatibility with semantic analysis expectations

**Important Data Structure:**
- **AST Node** - Union of different node types
- **Linked lists** - Used for unbounded-length lists (arguments, statements)

**Precedence Levels (low to high):**
1. `||` (logical OR)
2. `&&` (logical AND)
3. `==`, `!=` (equality)
4. `<`, `>`, `<=`, `>=` (comparison)
5. `+`, `-` (addition/subtraction)
6. `*`, `/` (multiplication/division)
7. `-`, `!` (unary minus/not)

**Next Stage:** AST goes to Semantic Analyzer for type checking and validation

---

**Continue to Part 3 for Semantic Analysis explanation...**
