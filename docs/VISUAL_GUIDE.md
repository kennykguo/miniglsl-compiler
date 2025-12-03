# Visual Learning Guide - Understanding Through Diagrams

**For visual learners - explanations through diagrams and flowcharts**

---

## The Complete Pipeline - Visual Overview

```
┌─────────────────────────────────────────────────────────────┐
│                      YOUR SHADER CODE                        │
│                    { float x = 1.5; }                        │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      ▼
            ┌─────────────────────┐
            │   SCANNER (Flex)    │
            │   scanner.l         │
            │                     │
            │ Character → Tokens  │
            └─────────┬───────────┘
                      │
                      │ [LBRACE] [FLOAT_T] [ID] [ASSIGN] ...
                      ▼
            ┌─────────────────────┐
            │   PARSER (Bison)    │
            │   parser.y          │
            │                     │
            │  Tokens → AST Tree  │
            └─────────┬───────────┘
                      │
                      │ SCOPE_NODE
                      │   └─ DECLARATION_NODE
                      ▼
            ┌─────────────────────┐
            │  SEMANTIC ANALYSIS  │
            │   semantic.c        │
            │                     │
            │ Type Check + Verify │
            └─────────┬───────────┘
                      │
                      │ Validated AST + Symbol Table
                      ▼
            ┌─────────────────────┐
            │  CODE GENERATION    │
            │   codegen.c         │
            │                     │
            │  AST → SPIR-V Code  │
            └─────────┬───────────┘
                      │
                      ▼
            ┌─────────────────────┐
            │    SPIR-V OUTPUT    │
            │   .spvasm file      │
            └─────────────────────┘
```

---

## Token Flow - Detailed View

```
Input: "float x = 1.5;"

Scanner Processing:
┌──────┬──────┬──────┬──────┬──────┬──────┬──────┐
│  f   │  l   │  o   │  a   │  t   │  ' ' │  x   │ ...
└──────┴──────┴──────┴──────┴──────┴──────┴──────┘
   │      │      │      │      │      │      │
   └──────┴──────┴──────┴──────┘      │      │
              │                        │      │
         Matches "float"          Skip space  │
              ↓                              Matches [a-zA-Z_]
        [FLOAT_T]                               ↓
                                            [ID("x")]

Output Stream:
┌──────────┬─────────┬────────┬──────────────┬───────────┐
│ FLOAT_T  │ ID("x") │ ASSIGN │ FLOAT_C(1.5) │ SEMICOLON │
└──────────┴─────────┴────────┴──────────────┴───────────┘
```

---

## AST Tree Structure

### Simple Declaration: `float x = 1.5;`

```
                    DECLARATION_NODE
                    ┌──────┴──────┐
                    │             │
                 type:          identifier:
                 FLOAT_T        "x"
                                │
                         initializer:
                          FLOAT_LITERAL_NODE
                                │
                              value:
                               1.5
```

### Binary Expression: `x + y * z`

```
                    BINARY_EXPR_NODE('+')
                    ┌─────────┴─────────┐
                    │                   │
                left:               right:
             VAR_NODE("x")     BINARY_EXPR_NODE('*')
                               ┌────────┴────────┐
                               │                 │
                           left:             right:
                        VAR_NODE("y")     VAR_NODE("z")
```

**Evaluation order:** Leaves first (y, z, x) → depth-first

### If Statement: `if (x < 2) { y = 10; }`

```
                      IF_STMT_NODE
          ┌──────────────┼──────────────┐
          │              │              │
      condition:     then_stmt:    else_stmt:
  BINARY_EXPR('<')  ASSIGNMENT      NULL
  ┌──────┴──────┐       │
  │             │    variable:
left:         right:  "y"
VAR("x")   LITERAL(2)  │
                    expr:
                  LITERAL(10)
```

---

## Symbol Table - Scoping Visualization

### Code Example:
```c
{                     // Scope 1 (outer)
    float x = 1.0;
    {                 // Scope 2 (inner)
        float y = 2.0;
        // x and y visible here
    }
    // only x visible here
}
```

### Symbol Table Stack:

```
┌─────────────────────────────────────┐
│      GLOBAL SCOPE (always)          │
│  gl_Color, gl_FragColor, env1, ...  │
└─────────────────────────────────────┘
              ▲
              │ parent
┌─────────────┴───────────────────────┐
│         SCOPE 1 (outer)              │
│      { "x" → (float, 1.0) }          │
└─────────────────────────────────────┘
              ▲
              │ parent
┌─────────────┴───────────────────────┐
│         SCOPE 2 (inner)              │
│      { "y" → (float, 2.0) }          │
└─────────────────────────────────────┘

Lookup process for "y":
1. Check SCOPE 2 → FOUND! Return entry
   
Lookup process for "x":
1. Check SCOPE 2 → Not found
2. Check SCOPE 2's parent (SCOPE 1) → FOUND!

Lookup process for "gl_Color":
1. Check SCOPE 2 → Not found
2. Check SCOPE 1 → Not found
3. Check GLOBAL → FOUND!
```

---

## Two-Pass Semantic Analysis - Visual

### Problem Code:
```c
{
    vec4 a = b;   // References b
    vec4 b = vec4(1,0,0,1);
}
```

### ❌ Single-Pass (Fails):

```
Step 1: Process 'a'
┌──────────────────┐
│  Symbol Table:   │
│  (empty)         │
└──────────────────┘
   Check: "vec4 a = b"
   Look up 'b' → NOT FOUND ❌ ERROR!
```

### ✅ Two-Pass (Works):

```
PASS 1: Add all declarations
┌──────────────────┐        ┌──────────────────┐
│  Symbol Table:   │   →    │  Symbol Table:   │
│  (empty)         │        │  a → vec4        │
└──────────────────┘        │  b → vec4        │
                            └──────────────────┘
   Process 'a': Add "a"
   Process 'b': Add "b"
   (Skip initializers)

PASS 2: Check initializers
┌──────────────────┐
│  Symbol Table:   │
│  a → vec4  ✓     │
│  b → vec4  ✓     │
└──────────────────┘
   Check 'a = b': Lookup 'b' → FOUND ✓
   Check 'b = vec4(...)': Valid ✓
```

---

## OpSelect Predication - Flow Diagram

### Traditional Branching (FORBIDDEN):

```
         ┌─[Condition]─┐
         │  x < 2.0?   │
         └─────┬───────┘
               │
        ┌──────┴──────┐
        │             │
     TRUE          FALSE
        │             │
        ▼             ▼
   ┌────────┐    ┌────────┐
   │ y=10.0 │    │ y=20.0 │
   └───┬────┘    └───┬────┘
       │             │
       └──────┬──────┘
              │
              ▼
         [Continue]

Uses: OpBranchConditional ❌
```

### OpSelect Predication (OUR APPROACH):

```
    ┌─[Condition]─┐
    │  x < 2.0?   │
    └──────┬──────┘
           │
     Evaluate BOTH!
           │
    ┌──────┴──────┐
    │             │
    ▼             ▼
┌────────┐    ┌────────┐
│ val=10.0│    │ val=20.0│
└───┬────┘    └───┬────┘
    │             │
    └──────┬──────┘
           │
           ▼
    ┌────────────────┐
    │   OpSelect:    │
    │ Pick val based │
    │ on condition   │
    └───────┬────────┘
            │
            ▼
       [y = result]

Linear flow! No branches!
```

### SPIR-V Code Comparison:

**Traditional (Forbidden):**
```spirv
%cond = OpFOrdLessThan %x %c_2
OpBranchConditional %cond %then %else  ❌

%then = OpLabel
  %val1 = OpConstant 10.0
  OpStore %y %val1
  OpBranch %merge

%else = OpLabel
  %val2 = OpConstant 20.0
  OpStore %y %val2
  OpBranch %merge

%merge = OpLabel
```

**OpSelect (Our Implementation):**
```spirv
%cond = OpFOrdLessThan %x %c_2
%val1 = OpConstant 10.0
%val2 = OpConstant 20.0
%result = OpSelect %t_float %cond %val1 %val2  ✓
OpStore %y %result
```

---

## Constant Pooling - Memory Stream Pattern

### The Problem:

```
Order we discover things:
1. Start generating function
2. Find: need constant 1.0
3. Find: need constant 2.5
4. End function

Order SPIR-V requires:
1. All constants
2. Then function

We can't emit constants as we find them!
```

### The Solution:

```
┌─────────────────────────────────────────────────┐
│              Code Generation Start               │
└────────────────────┬────────────────────────────┘
                     │
         ┌───────────┴───────────┐
         │                       │
         ▼                       ▼
┌──────────────────┐    ┌──────────────────┐
│  Constant Pool   │    │   Memory Stream  │
│   (collect)      │    │  (buffer func)   │
├──────────────────┤    ├──────────────────┤
│                  │    │ %entry = OpLabel │
│ %101: 1.0        │    │ %100 = OpVar...  │
│ %102: 2.5        │    │ OpStore %100...  │
│                  │    │ ...              │
└──────────────────┘    └──────────────────┘
         │                       │
         │                       │
         └───────────┬───────────┘
                     │
                     ▼
         ┌───────────────────────┐
         │    Emit in Order:     │
         ├───────────────────────┤
         │ 1. Prologue (types)   │
         │ 2. Constants (pool)   │
         │ 3. Function (buffer)  │
         └───────────────────────┘
                     │
                     ▼
         ┌───────────────────────┐
         │   Final SPIR-V File   │
         └───────────────────────┘
```

---

## Complete Example: Data Flow

### Input: `{ float x = 1.5; }`

**Step 1: Scanner**
```
Characters: { float x = 1.5; }
            ↓
Tokens:     [LBRACE, FLOAT_T, ID("x"), ASSIGN, 
             FLOAT_C(1.5), SEMICOLON, RBRACE]
```

**Step 2: Parser**
```
Tokens → Grammar Rules → AST

SCOPE_NODE
  └─ declarations: [DECLARATION_NODE]
      ├─ type: float
      ├─ name: "x"
      └─ init: FLOAT_LITERAL(1.5)
```

**Step 3: Semantic (Pass 1)**
```
Symbol Table (before): {}
Process declaration: Add "x"
Symbol Table (after): {x → float}
```

**Step 4: Semantic (Pass 2)**
```
Check initializer: FLOAT_LITERAL(1.5)
Type: float ✓
Matches variable type ✓
```

**Step 5: Codegen (Allocate)**
```
Allocate SSA ID: 100
Emit: %100 = OpVariable %t_ptr_func_float Function
Track: x → ID 100, type float
```

**Step 6: Codegen (Initialize)**
```
Generate constant:
  ID: 101
  Add to pool: %101 = OpConstant %t_float 1.5

Emit: OpStore %100 %101
```

**Step 7: Final SPIR-V**
```spirv
; Constants (from pool)
%101 = OpConstant %t_float 1.5

; Function
%main = OpFunction ...
%entry = OpLabel

; Variables
%100 = OpVariable %t_ptr_func_float Function

; Code
OpStore %100 %101

OpReturn
OpFunctionEnd
```

---

## Comparison: Different Approaches

### Our Compiler vs Alternatives

```
┌────────────────────┬──────────────┬────────────────┐
│                    │ Our Compiler │   Alternative  │
├────────────────────┼──────────────┼────────────────┤
│ If/Else            │  OpSelect    │  OpBranch      │
│ Efficiency         │  Linear SSA  │  More optimal  │
│ Complexity         │  Simple      │  Complex       │
│ Spec compliance    │  ✓ Yes       │  ❌ Violates   │
├────────────────────┼──────────────┼────────────────┤
│ Declarations       │  C99 style   │  C89 strict    │
│ Flexibility        │  Mixed OK    │  Must separate │
│ Ref shader compat  │  ✓ All work  │  ❌ Need edit  │
├────────────────────┼──────────────┼────────────────┤
│ Semantic analysis  │  Two-pass    │  Single-pass   │
│ Forward refs       │  ✓ Handles   │  ❌ Errors     │
│ Complexity         │  Higher      │  Simpler       │
└────────────────────┴──────────────┴────────────────┘
```

---

## Mental Model: Think of It Like This

### Scanner = Reading
```
You don't read letter-by-letter: T-h-e-c-a-t
You recognize whole words: [The] [cat] [sat]
Scanner does same with code!
```

### Parser = Understanding Structure
```
Sentence: "The quick brown fox jumps"
Not just words, but structure:
  [Subject: The quick brown fox]
  [Verb: jumps]

Parser finds code structure!
```

### Semantic = Fact-Checking
```
"Colorless green ideas sleep furiously"
Grammar: ✓ Correct
Meaning: ❌ Makes no sense!

Semantic analysis checks meaning!
```

### Codegen = Translation
```
English: "The cat sat"
Spanish: "El gato se sentó"

Source code → Target code translation!
```

---

## Decision Tree: When Things Happen

```
Start Compilation
    │
    ├─ Scanner runs
    │   └─ Finds tokens? → Yes → Continue
    │                   → No → Syntax error
    │
    ├─ Parser runs
    │   └─ Valid grammar? → Yes → Build AST
    │                     → No → Parse error
    │
    ├─ Semantic Pass 1
    │   └─ Add all declarations to symbol table
    │
    ├─ Semantic Pass 2
    │   ├─ Variables exist? → Yes → Continue
    │   │                   → No → Undeclared variable
    │   │
    │   ├─ Types match? → Yes → Continue
    │   │               → No → Type mismatch
    │   │
    │   └─ Valid operations? → Yes → Continue
    │                        → No → Semantic error
    │
    └─ Code Generation
        ├─ Allocate variables (OpVariable)
        ├─ Generate expressions
        ├─ Emit statements
        └─ Output SPIR-V
```

---

## Memory Layout: How Data is Stored

### AST Node (Union):

```
┌─────────────────────────────────────┐
│         node_kind = BINARY_EXPR     │  ← Discriminator
├─────────────────────────────────────┤
│                                     │
│  ┌─ binary_expr {                  │
│  │    op: '+'                       │  ← Active
│  │    left: pointer                 │     union
│  │    right: pointer                │     member
│  │  }                               │
│  │                                  │
│  ├─ variable { ... }                │  ← Unused
│  ├─ literal { ... }                 │     (memory
│  └─ ...                             │      overlaid)
│                                     │
└─────────────────────────────────────┘
      Same memory, different views!
```

### Symbol Table:

```
┌───────────────────────────────────────┐
│  Hash Table or Linked List            │
├───────┬──────────┬────────┬───────────┤
│ Name  │   Type   │ Const? │ Read-only?│
├───────┼──────────┼────────┼───────────┤
│ "x"   │  float   │   No   │    No     │
│ "col" │  vec4    │   No   │    No     │
│"gl_C" │  vec4    │   No   │    Yes    │
└───────┴──────────┴────────┴───────────┘
```

---

## Summary: The Big Picture in One Diagram

```
                    SOURCE CODE
                        │
                        ▼
            ┌───────────────────────┐
            │  SCANNER (scanner.l)  │
            │  Pattern Recognition  │
            └───────────┬───────────┘
                        │ Tokens
                        ▼
            ┌───────────────────────┐
            │   PARSER (parser.y)   │
            │   Grammar Rules       │
            └───────────┬───────────┘
                        │ AST  
                        ▼
            ┌───────────────────────┐
            │ SEMANTIC (semantic.c) │
            │ Two-Pass Analysis     │
            ├───────────────────────┤
            │ Pass 1: Add to table  │
            │ Pass 2: Check types   │
            └───────────┬───────────┘
                        │ Validated AST
                        ▼
            ┌───────────────────────┐
            │ CODEGEN (codegen.c)   │
            │ SPIR-V Generation     │
            ├───────────────────────┤
            │ • Constant pooling    │
            │ • OpSelect predication│
            │ • SSA generation      │
            └───────────┬───────────┘
                        │
                        ▼
                    SPIR-V CODE
                        │
                        ▼
                ┌───────────────┐
                │   spirv-as    │
                │   (assemble)  │
                └───────┬───────┘
                        │
                        ▼
                ┌───────────────┐
                │   spirv-val   │
                │   (validate)  │
                └───────┬───────┘
                        │
                        ▼
                  READY FOR GPU!
```

---

## This Visual Guide Complements:

- **BASICS_101.md** - If you prefer text explanations
- **EXPLAINED_PART1-6** - For detailed walkthroughs
- **GLOSSARY.md** - For term definitions
- **FAQ.md** - For specific questions

**Some people learn better with diagrams - this guide is for you!** 📊
