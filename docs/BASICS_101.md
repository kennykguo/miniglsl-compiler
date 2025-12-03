# Compiler Basics 101 - Prerequisites

**Read This FIRST Before the Main Walkthrough**

This document explains fundamental concepts you need to understand before reading the detailed compiler walkthrough.

---

## What is a Compiler?

**Simple answer:** A translator from human-readable code to machine instructions.

**Example:**
```
You write:    float x = 1.5;
Computer sees: 10110011 01001010 11010110...
```

The compiler translates from the first to the second.

### Why Do We Need Compilers?

**Humans prefer:**
- Words like `float`, `if`, `while`
- Mathematical notation like `x + y`
- Hierarchical structure with `{...}`

**Computers understand:**
- Binary (0s and 1s)
- Simple operations (load, add, store)
- Linear sequence of instructions

**Compiler bridges the gap!**

---

## The Four Stages (Every Compiler Has These)

### Stage 1: Lexical Analysis (Scanner)

**Job:** Break text into "words" (tokens)

**Analogy:** Reading a book
- You don't read letter-by-letter
- You recognize whole words at a time

**Example:**
```
Input:  "float x = 1.5;"
Tokens: [FLOAT_T] [ID("x")] [ASSIGN] [FLOAT_C(1.5)] [SEMICOLON]
```

**Why separate?** Makes next stages easier (they work with tokens, not characters).

### Stage 2: Syntactic Analysis (Parser)

**Job:** Organize tokens into a tree structure

**Analogy:** Understanding sentence structure
- "The quick brown fox jumps" has structure:
  - Subject: "The quick brown fox"
  - Verb: "jumps"

**Example:**
```
Tokens: float x = 1.5;

Tree:
DECLARATION
  ├─ type: float
  ├─ name: x
  └─ value: 1.5
```

**Why a tree?** Code has hierarchy (nested blocks, precedence).

### Stage 3: Semantic Analysis

**Job:** Check if code makes logical sense

**Analogy:** Fact-checking a sentence
- "Colorless green ideas sleep furiously"
- Grammatically correct ✓
- Logically nonsense ✗

**Example:**
```c
float x = "hello";  // Type mismatch! float can't be string
z = 5;              // Error! z doesn't exist
```

**Why needed?** Parser only checks grammar, not meaning.

### Stage 4: Code Generation

**Job:** Convert tree to target language instructions

**Example:**
```
Tree:              SPIR-V:
DECLARATION        %100 = OpVariable ...
  └─ value: 1.5    %101 = OpConstant %t_float 1.5
                   OpStore %100 %101
```

**Why last?** Can't generate code until we know it's valid.

---

## Key Concepts You MUST Understand

### Concept 1: Tokens

**What:** Smallest meaningful units of code

**Types:**
- **Keywords:** `float`, `if`, `else` (reserved words)
- **Identifiers:** `x`, `myVariable` (names you choose)
- **Literals:** `1.5`, `42`, `true` (constant values)
- **Operators:** `+`, `-`, `*`, `<`, `==`
- **Punctuation:** `{`, `}`, `(`, `)`, `;`, `,`

**Example breakdown:**
```c
if (x < 10) { }
```

Tokens:
```
IF         - keyword
LPAREN     - punctuation (
ID("x")    - identifier
LT         - operator <
INT_C(10)  - literal
RPAREN     - punctuation )
LBRACE     - punctuation {
RBRACE     - punctuation }
```

**Why important:** Parser works with tokens, not text.

### Concept 2: Abstract Syntax Tree (AST)

**What:** Tree representing code structure

**Visual example:**
```c
x + y * z
```

**Tree:**
```
      +
     / \
    x   *
       / \
      y   z
```

**Why this shape?** Operator precedence!
- `*` has higher precedence than `+`
- So `*` is deeper in tree (evaluated first)

**Reading the tree:**
1. Start at leaves (x, y, z)
2. Work up to root
3. Evaluation order: y, z, multiply, x, add

**Another example:**
```c
if (x < 2) {
    y = 10;
}
```

**Tree:**
```
IF_STMT
  ├─ condition: BINARY_EXPR(<)
  │   ├─ left: VAR(x)
  │   └─ right: LITERAL(2)
  └─ then: ASSIGNMENT
      ├─ variable: VAR(y)
      └─ value: LITERAL(10)
```

**Why tree?** Shows relationships (if contains condition and body).

### Concept 3: Symbol Table

**What:** Dictionary mapping variable names to information

**Like a phone book:**
- Name → phone number
- Variable name → type, location, properties

**Example:**
```c
float x = 1.0;
vec4 color = ...;
```

**Symbol table:**
```
Name     | Type   | Const? | Read-only?
---------|--------|--------|------------
x        | float  | No     | No
color    | vec4   | No     | No
gl_Color | vec4   | No     | Yes
```

**Why needed?** To check if variables exist and what type they are.

### Concept 4: Scope

**What:** Region where a variable is visible

**Example:**
```c
{                    // Outer scope
    float x = 1.0;
    {                // Inner scope
        float y = 2.0;
        // Can see both x and y here
    }
    // Can only see x here (y out of scope)
}
```

**Visual:**
```
Outer scope: {x}
   └── Inner scope: {x, y}
```

**Why important?** Variables hide when you leave their scope.

### Concept 5: Types

**What:** Category of a value (float, int, bool, vector)

**Example types:**
- `float` - decimal number (1.5, 3.14)
- `int` - whole number (1, 42, -5)
- `bool` - true or false
- `vec4` - 4-component vector ([1.0, 0.5, 0.2, 1.0])

**Why types matter:**
```c
float x = 1.5;     // OK
float y = "hello"; // ERROR! Can't put string in float
```

**Type checking:** Compiler verifies types match.

### Concept 6: Expressions vs Statements

**Expression:** Something that produces a value
```c
x + y          // Produces result of addition
dp3(a, b)      // Produces dot product
x < 10         // Produces true or false
```

**Statement:** Something that does an action
```c
x = 5;         // Assignment - stores value
if (x < 10) {} // Conditional - controls flow
```

**Key difference:**
- Expression: "What is the value?"
- Statement: "What do I do?"

### Concept 7: SSA (Static Single Assignment)

**What:** Each variable/value gets assigned exactly once

**Normal code (NOT SSA):**
```c
x = 1;
x = x + 2;  // x assigned twice!
```

**SSA version:**
```c
x1 = 1;
x2 = x1 + 2;  // Each x gets unique ID
```

**SPIR-V uses SSA:**
```spirv
%100 = OpConstant %t_float 1.0    ; ID 100 assigned once
%101 = OpFAdd %100 %c_2           ; ID 101 assigned once
```

**Why SSA?** Simpler analysis and optimization.

### Concept 8: Control Flow

**What:** Order of execution

**Linear (no control flow):**
```c
x = 1;
y = 2;
z = 3;
```
Executes: line 1 → line 2 → line 3 (always)

**With control flow:**
```c
if (x < 10) {
    y = 1;  // Maybe executed
} else {
    y = 2;  // Maybe executed
}
```
Execution depends on condition!

**Our compiler:** Uses OpSelect to AVOID branching
```spirv
; Evaluate both possibilities
val1 = 1
val2 = 2
result = select(condition, val1, val2)  ; Pick one
```

---

## Technical Terms You'll See Often

### Parser/Grammar Terms

**Terminal:** A token that appears in the source code
- Example: `float`, `+`, `123`

**Non-terminal:** A grammar rule that expands to other rules
- Example: `expression`, `statement`

**Production rule:** A grammar rule
```
expression → expression '+' expression
```
Means: "An expression can be two expressions with + between them"

**Recursion:** A rule that refers to itself
```
statement_list → statement_list statement
```
Allows unlimited statements!

### Tree Terms

**Root:** Top of tree (the whole program)

**Leaf:** Bottom of tree (literals, variables)

**Parent/Child:** Relationship between nodes
```
BINARY_EXPR(+)    ← parent
  ├─ VAR(x)       ← child
  └─ LITERAL(2)   ← child
```

**Depth:** How many levels deep
```
      +           depth 0 (root)
     / \
    x   *         depth 1
       / \
      y   z       depth 2 (leaves)
```

### Memory Terms

**Pointer:** Address/location in memory

**Dereference:** Get value at a pointer

**Load:** Read value from memory

**Store:** Write value to memory

**SPIR-V example:**
```spirv
%100 = OpVariable ...     ; %100 is a POINTER (address)
%101 = OpLoad %100        ; %101 is the VALUE at that address
OpStore %100 %102         ; Put %102's value at address %100
```

---

## Common Analogies That Help

### Compiler = Translation Service

```
English text → Translator → Spanish text
Source code → Compiler → Machine code
```

### AST = Family Tree

```
Grandparent (root)
  ├─ Parent 1
  │   ├─ Child 1
  │   └─ Child 2
  └─ Parent 2
```

Each person is a node, relationships shown by tree structure.

### Symbol Table = Contact List

```
Name          | Phone
John Smith    | 555-1234
Jane Doe      | 555-5678
```

Just like:
```
Variable | Type
x        | float
color    | vec4
```

### Tokens = Words in a Sentence

```
Sentence: "The quick brown fox jumps"
Words:    [The] [quick] [brown] [fox] [jumps]

Code:     "float x = 1.5;"
Tokens:   [float] [x] [=] [1.5] [;]
```

### Scope = Rooms in a House

```
House (global scope)
  └─ Kitchen (local scope)
      └─ Pantry (nested scope)
```

Variables in pantry can see kitchen and house.
Variables in house can't see pantry.

---

## Simple Example: Complete Pipeline

**Input code:**
```c
{
    float x = 1.5;
}
```

### Step 1: Scanner

**Input:** `{ f l o a t   x   =   1 . 5 ; }`

**Output:** `LBRACE FLOAT_T ID("x") ASSIGN FLOAT_C(1.5) SEMICOLON RBRACE`

**What happened:** Recognized patterns (float = keyword, x = identifier, etc.)

### Step 2: Parser

**Input:** Tokens from scanner

**Output:**
```
SCOPE
  └─ DECLARATION
      ├─ type: float
      ├─ name: "x"
      └─ value: 1.5
```

**What happened:** Organized tokens into structure

### Step 3: Semantic

**Input:** AST from parser

**Actions:**
1. Create symbol table
2. Add "x" → (float, not const)
3. Check initializer 1.5 is float ✓

**Output:** Validated AST + symbol table

**What happened:** Verified it makes sense

### Step 4: Code Generation

**Input:** Validated AST

**Output:**
```spirv
%100 = OpVariable %t_ptr_func_float Function
%101 = OpConstant %t_float 1.5
OpStore %100 %101
```

**What happened:** Converted to SPIR-V instructions

**Final result:** GPU can execute this!

---

## Practice: Can You Identify These?

**Code:**
```c
if (x < 10) {
    y = 5;
}
```

**Questions:**

1. **What are the tokens?**
   <details><summary>Answer</summary>
   IF, LPAREN, ID("x"), LT, INT_C(10), RPAREN, LBRACE, ID("y"), ASSIGN, INT_C(5), SEMICOLON, RBRACE
   </details>

2. **What is the root of the AST?**
   <details><summary>Answer</summary>
   IF_STMT_NODE (the if statement contains everything)
   </details>

3. **What is checked during semantic analysis?**
   <details><summary>Answer</summary>
   - Does x exist?
   - Is x < 10 a boolean? (comparison produces bool ✓)
   - Does y exist?
   - Can we assign to y? (not const, not read-only)
   </details>

4. **Is this an expression or statement?**
   <details><summary>Answer</summary>
   Statement (it does something - controls flow)
   </details>

---

## What You DON'T Need to Know

You don't need to understand:
- ❌ How CPUs work internally
- ❌ Assembly language (though SPIR-V helps)
- ❌ Memory management in detail
- ❌ Operating systems
- ❌ Complex math

You DO need to understand:
- ✅ Basic programming (variables, if statements)
- ✅ Trees and lists (data structures)
- ✅ Recursive thinking
- ✅ The four compiler stages
- ✅ Concept of types

---

## Summary: The Big Picture

**Compilation is transformation in stages:**

```
Text
  ↓ Scanner
Tokens
  ↓ Parser
Tree (AST)
  ↓ Semantic
Validated Tree + Symbol Table
  ↓ Code Gen
Target Code (SPIR-V)
```

**Each stage:**
- Has a specific job
- Uses output of previous stage
- Produces input for next stage

**Compiler = Chain of transformations**

**Now you're ready to read the detailed walkthrough!**

---

## Next Steps

1. **Read this document again** if anything is unclear
2. **Read EXPLAINED_INDEX.md** for overview
3. **Start with Part 1 (Scanner)** when ready

**You now have the foundation!** 🎓
