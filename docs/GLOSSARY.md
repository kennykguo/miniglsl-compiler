# Complete Glossary - All Technical Terms Explained

**Alphabetical reference for every term used in the compiler**

---

## A

**Abstract Syntax Tree (AST)**
A tree representation of source code structure. Each node represents a construct (expression, statement, declaration). Unlike the parse tree, it abstracts away syntactic details like parentheses and semicolons.
- Example: `x + y` becomes BINARY_EXPR_NODE('+') with children VAR(x) and VAR(y)

**Allocate**
Reserve memory or assign an ID for a variable or value.
- In codegen: `%100 = OpVariable ...` allocates variable storage

**Argument**
Value passed to a function.
- Example: In `dp3(a, b)`, both `a` and `b` are arguments

**Assignment**
Storing a value in a variable.
- Example: `x = 5` assigns 5 to x
- SPIR-V: `OpStore %var_id %value_id`

**Associativity**
For operators of same precedence, determines left-to-right or right-to-left evaluation.
- Left-associative: `a - b - c` = `(a - b) - c`
- Right-associative: `a = b = c` = `a = (b = c)`

---

## B

**Binary Expression**
Expression with two operands and an operator.
- Example: `x + y`, `a * b`, `p < q`
- Forms: left-operand operator right-operand

**Binary Operator**
Operator that takes two operands.
- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`
- Logical: `&&`, `||`

**Bison**
Parser generator tool. Takes grammar file (`.y`), produces C parser code.
- Input: `parser.y` (grammar + actions)
- Output: `parser.c` (C code)

**Boolean (bool)**
Type with two values: true or false.
- Used for conditions: `if (x < 10)` - the `x < 10` is boolean

**Branching**
Changing execution flow based on a condition.
- Traditional: `if (cond) goto label1; else goto label2;`
- **Our compiler:** Uses OpSelect instead (no branching!)

**Built-in Variable**
Predefined variable provided by the system.
- Examples: `gl_Color`, `gl_FragColor`, `gl_FragCoord`
- Don't need to be declared

---

## C

**Call Stack**
Stack of function calls (who called whom).
- In recursive functions, each call adds a frame to the stack
- Example: `gen_expression` calling itself for nested expressions

**Cast**
Convert value from one type to another.
- MiniGLSL doesn't have explicit casts

**Code Generation (Codegen)**
Final compiler phase: convert AST to target code (SPIR-V).
- Input: Validated AST
- Output: SPIR-V assembly text

**Comparison Operator**
Operators that compare values.
- `<`, `>`, `<=`, `>=`, `==`, `!=`
- Produce boolean result

**Compiler**
Program that translates source code to target code.
- Our case: MiniGLSL → SPIR-V

**Composite**
Value made of multiple components.
- Example: `vec4` is composite of 4 floats
- Operations: OpCompositeConstruct, OpCompositeExtract, OpCompositeInsert

**Constant**
Value that doesn't change.
- Literals: `1.5`, `42`, `true`
- SPIR-V: `OpConstant`

**Constant Folding**
Optimization: evaluate constants at compile time.
- Example: `2 + 3` → `5`
- **Our compiler:** Doesn't do this

**Constant Pool**
Collection of constants to emit before function.
- We collect constants during code generation, emit them later
- Solves: SPIR-V requires constants before function

**Constructor**
Creates a composite value from components.
- Example: `vec4(1.0, 0.5, 0.2, 1.0)`
- SPIR-V: `OpCompositeConstruct`

**Context-Free Grammar (CFG)**
Set of production rules defining language syntax.
- Format: `nonterminal → pattern`
- Example: `expression → expression '+' expression`

**Control Flow**
Order of statement execution.
- Linear: execute top to bottom
- Conditional: `if/else` changes flow
- Loops: repeat execution (not in MiniGLSL)

---

## D

**Declaration**
Introduction of a variable with its type.
- Example: `float x = 1.0;`
- Consists of: type, name, optional initializer

**Dereference**
Get value at a pointer.
- SPIR-V: `OpLoad` dereferences (reads value from address)

**Dot Product**
Mathematical operation on vectors.
- Formula: `dot(p[a,b,c], [d,e,f]) = a*d + b*e + c*f`
- MiniGLSL: `dp3(vec, vec)` function

---

## E

**Expression**
Code that produces a value.
- Examples: `x + y`, `sin(x)`, `5`, `x < 10`
- Contrast with statement (which does an action)

**Extended Instruction**
SPIR-V instruction from GLSLstd.450 library.
OpExtInst calls these.
- Example: `Dot` (dot product), `InverseSqrt`, `Normalize`

---

## F

**Flex**
Scanner generator tool. Takes patterns (`.l` file), produces C scanner code.
- Input: `scanner.l` (patterns + actions)
- Output: `scanner.c` (C code)

**Float (floating-point)**
Type representing decimal numbers.
- Example values: `1.5`, `3.14`, `-0.5`
- SPIR-V type: `%t_float` (32-bit)

**Forward Declaration**
Declaring something before it's fully defined.
- C example: `float x;` then later `x = 1.0;`
- **Our compiler:** Two-pass avoids needing this

**Forward Reference**
Using a name before it's declared.
- Example: `vec4 color = base;` where `base` comes after
- **Two-pass semantic analysis** handles this

**Function**
Reusable piece of code.
- MiniGLSL functions: `dp3`, `rsq`, `lit`
- SPIR-V: `OpFunction`

---

## G

**Global Scope**
Outermost scope containing built-ins.
- Variables: `gl_Color`, `gl_FragColor`, etc.
- Always accessible

**Grammar**
Formal rules defining language syntax.
- Specified in `parser.y`
- Format: production rules

**Grammar Rule**
Single rule in grammar.
- Example: `statement → IF '(' expression ')' statement`

---

## I

**ID / SSA ID**
Unique identifier for a value in SSA form.
- SPIR-V: `%100`, `%101`, `%102`, ...
- Each ID assigned exactly once

**Identifier**
Name chosen by programmer.
- Variable names: `x`, `color`, `baseColor`
- Pattern: starts with letter/underscore, contains letters/digits/underscores

**If Statement**
Conditional execution.
- Syntax: `if (condition) { ... } else { ... }`
- **Our compiler:** Uses OpSelect (no branching)

**Immutable**
Cannot be changed.
- SSA values are immutable (assigned once)

**Initializer**
Initial value for a variable.
- Example: In `float x = 1.5`, the `1.5` is the initializer

**Instruction**
Single operation in target code.
- SPIR-V example: `OpFAdd`, `OpLoad`, `OpStore`

**Integer (int)**
Whole number type.
- Example values: `1`, `42`, `-5`
- No decimals

---

## K

**Keyword**
Reserved word in the language.
- Examples: `if`, `else`, `float`, `vec4`, `const`
- Cannot be used as variable names

---

## L

**Label**
Named point in code (for jumps/branches).
- SPIR-V: `%entry = OpLabel`
- **Our compiler:** Only one label (`%entry`), no branching

**Leaf Node**
Node with no children (bottom of tree).
- Examples: literals, variable references
- AST leaves: INT_LITERAL_NODE, FLOAT_LITERAL_NODE, VAR_NODE

**Lexical Analysis**
First compiler phase: text → tokens.
- Also called "scanning"
- File: `scanner.l`

**Linked List**
Data structure: chain of nodes.
- Each node has data + pointer to next
- Used for: AST lists (statements, arguments)

**Literal**
Value written directly in code.
- Integer: `42`
- Float: `1.5`
- Boolean: `true`, `false`

**Load**
Read value from memory.
- SPIR-V: `OpLoad %type %pointer`
- Returns value at pointer

**Local Scope**
Scope inside a block `{...}`.
- Variables declared here only visible in block

**Lookup**
Search for something (variable, type, etc.).
- Symbol table lookup: find variable by name

---

## M

**Memory Stream**
Write to memory buffer like a file.
- C function: `open_memstream(...)`
- **Our use:** Buffer function body, emit constants first

**MiniGLSL**
Subset of GLSL (OpenGL Shading Language).
- Fragment shader language
- Simpler than full GLSL

---

## N

**Nested**
Contained inside another.
- Nested expressions: `(x + y) * z`
- Nested scopes: `{ { } }`

**Node**
Element in a tree or list.
- AST node: represents code construct
- List node: element in linked list

**Non-terminal**
Grammar symbol that expands to other symbols.
- Example: `expression`, `statement`
- Appears on left side of production rules

---

## O

**OpBranch**
SPIR-V instruction: unconditional jump.
- **FORBIDDEN** in our compiler (spec requirement)

**OpBranchConditional**
SPIR-V instruction: conditional jump.
- **FORBIDDEN** in our compiler

**OpCompositeConstruct**
SPIR-V: build composite from components.
- Example: `vec4(1,0,0,1)` → `OpCompositeConstruct %t_vec4 %a %b %c %d`

**OpCompositeExtract**
SPIR-V: get component from composite.
- Example: `color[0]` → `OpCompositeExtract %t_float %color 0`

**OpCompositeInsert**
SPIR-V: create new composite with component replaced.
- Example: `color[1] = 0.8` uses OpCompositeInsert
- 3 steps: Load, Insert, Store

**OpConstant**
SPIR-V: declare constant value.
- Example: `%100 = OpConstant %t_float 1.5`

**Operand**
Value an operator works on.
- In `x + y`: operands are `x` and `y`

**Operator**
Symbol performing an operation.
- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `<`, `>`, `==`
- Logical: `&&`, `||`, `!`

**OpExtInst**
SPIR-V: call extended instruction.
- Example: `OpExtInst %t_float %ext Dot %a %b` (dot product)

**OpFAdd, OpFSub, OpFMul, OpFDiv**
SPIR-V: floating-point arithmetic.
- FAdd: addition
- FSub: subtraction
- FMul: multiplication
- FDiv: division

**OpFOrdLessThan, OpFOrdGreaterThan**
SPIR-V: floating-point ordered comparisons.
- Returns boolean

**OpFunction, OpFunctionEnd**
SPIR-V: function definition boundaries.
- OpFunction: start of function
- OpFunctionEnd: end of function

**OpLabel**
SPIR-V: marks a basic block.
- **Our compiler:** Only `%entry = OpLabel`

**OpLoad**
SPIR-V: load value from variable (pointer).
- Syntax: `%result = OpLoad %type %variable_pointer`

**OpPhi**
SPIR-V: merge values from different control flow paths.
- **FORBIDDEN** in our compiler

**OpReturn**
SPIR-V: return from function.
- Our shaders: `OpReturn` (void return)

**OpSelect**
SPIR-V: select value based on condition.
- Syntax: `%result = OpSelect %type %condition %true_value %false_value`
- **Key feature of our compiler!**

**OpStore**
SPIR-V: store value to variable (pointer).
- Syntax: `OpStore %variable_pointer %value`

**OpVariable**
SPIR-V: declare variable.
- Syntax: `%id = OpVariable %pointer_type Storage_class`
- Our compiler: all at function start

---

## P

**Parameter**
Variable in function definition.
- Example: In `float dp3(vec3 a, vec3 b)`, `a` and `b` are parameters

**Parse Tree**
Tree showing exact structure of parsing.
- More detailed than AST (includes every grammar rule)
- We build AST directly, skip parse tree

**Parser**
Compiler phase 2: tokens → AST.
- File: `parser.y`
- Tool: Bison

**Pass**
Single traversal of the data.
- **Two-pass semantic:** traverse declarations twice

**Pattern**
Regular expression to match tokens.
- Example: `[0-9]+\.[0-9]+` matches floats

**Phi Node**
SSA construct for merging values.
- **Not used in our compiler** (OpSelect instead)

**Pointer**
Memory address.
- SPIR-V variables are pointers
- Must OpLoad to get value

**Precedence**
Priority of operators.
- Higher precedence = evaluated first
- Example: `*` before `+`, so `2 + 3 * 4` = `2 + 12` = `14`

**Predication**
Technique: evaluate both branches, select result.
- **Our approach:** OpSelect predication
- Avoids branching instructions

**Production Rule**
Grammar rule defining how non-terminal expands.
- Format: `left → right`
- Example: `expression → expression '+' expression`

**Prologue**
Fixed header of SPIR-V module.
- Contains: capabilities, types, built-ins
- **Our compiler:** Uses provided prologue

---

## Q

**Qualifier**
Keyword modifying a declaration.
- Example: `const` in `const float pi = 3.14`

---

## R

**Read-only**
Cannot be written to.
- Example: `gl_Color` (input attribute)
- Semantic analysis enforces this

**Recursion**
Function calling itself.
- **Our use:** `gen_expression` calls itself for nested expressions
- Matches tree structure naturally

**Regular Expression (Regex)**
Pattern for matching text.
- Example: `[0-9]+` matches one or more digits
- Used in scanner

**Reserved Word**
See Keyword

**Root**
Top node of a tree.
- AST root: usually SCOPE_NODE (the program)

---

## S

**Scalar**
Single value (not vector/composite).
- Types: float, int, bool
- Contrast with vec4 (4 components)

**Scanner**
See Lexical Analysis

**Scope**
Region where names are valid.
- Global scope: entire program
- Local scope: inside `{...}`

**Semantic Analysis**
Compiler phase 3: check code makes sense.
- File: `semantic.c`
- Checks: types compatible, variables exist, etc.

**SPIR-V**
Standard Portable Intermediate Representation - Vulkan.
- Binary format for GPU shaders
- We generate assembly text (`.spvasm`)

**SSA**
Static Single Assignment form.
- Each variable assigned exactly once
- SPIR-V uses SSA

**Statement**
Code that performs an action.
- Examples: assignment, if/else, scope
- Contrast with expression (produces value)

**Store**
Write value to memory.
- SPIR-V: `OpStore %pointer %value`

**Symbol**
Named entity (variable, function, type).

**Symbol Table**
Map from names to information.
- Maps: variable name → type, properties
- Organized by scope

**Syntactic Analysis**
See Parser

**Syntax Tree**
See Abstract Syntax Tree

---

## T

**Terminal**
Token that appears in source code.
- Example: `FLOAT_T`, `PLUS`, `ID`
- Leaf of parse tree

**Token**
Meaningful unit from scanner.
- Types: keyword, identifier, literal, operator, punctuation
- Output of scanner, input to parser

**Tree**
Hierarchical data structure.
- Root, children, leaves
- **Our use:** AST represents code

**Two-Pass**
Processing data twice.
- **Our semantic analysis:**
  - Pass 1: Add all declarations to symbol table
  - Pass 2: Check initializers

**Type**
Category of values.
- Scalar: float, int, bool
- Vector: vec2, vec3, vec4

**Type Checking**
Verifying types are used correctly.
- Done in semantic analysis
- Example checks: `float x = 5.0` (OK), `float x = "hello"` (ERROR)

**Type Code**
Internal representation of a type.
- Constants: `FLOAT_T`, `INT_T`, `VEC_T`, `BOOL_T`

---

## U

**Unary Expression**
Expression with one operand.
- Examples: `-x` (negation), `!flag` (logical not)

**Union**
C construct: multiple fields share memory.
- **Our AST:** Each node type has different fields, union saves space

**Uniform**
Read-only variable set from CPU.
- Examples: `env1`, `env2`, `env3`

---

## V

**Validation**
Checking if output is correct.
- Tools: `spirv-as` (assembler), `spirv-val` (validator)

**Value**
Result of evaluating an expression.
- Can be: number, boolean, vector

**Variable**
Named storage location.
- Declaration creates variable
- Assignment stores value

**Variable Entry**
Record in our variable tracking system.
- Fields: name, ID, type, vec_size

**Vector**
Composite of 2, 3, or 4 values.
- Types: `vec2`, `vec3`, `vec4`
- Example: `vec4(1.0, 0.5, 0.2, 1.0)`

**vec2, vec3, vec4**
Vector types.
- vec2: 2 components
- vec3: 3 components
- vec4: 4 components

---

## W

**Write-only**
Cannot be read from.
- Example: `gl_FragColor` (output)
- Semantic analysis enforces this

---

## Y

**Yacc**
Classic parser generator (Bison is GNU version).
- Stands for "Yet Another Compiler Compiler"
- Input: `.y` file with grammar

**yytext**
Scanner variable holding matched text.
- Set by Flex when pattern matches
- Example: matching `"1.5"` sets `yytext = "1.5"`

**yylval**
Variable for passing token values to parser.
- Example: `yylval.as_float = 1.5` passes float value

---

## Summary By Category

### Compiler Phases
- Lexical Analysis (Scanner)
- Syntactic Analysis (Parser)
- Semantic Analysis
- Code Generation

### Tools
- Flex (scanner generator)
- Bison (parser generator)
- spirv-as (SPIR-V assembler)
- spirv-val (SPIR-V validator)

### Data Structures
- Token
- AST (Abstract Syntax Tree)
- Symbol Table
- Linked List
- Variable Entry
- Constant Pool

### SPIR-V Instructions
- OpConstant, OpVariable
- OpLoad, OpStore
- OpFAdd, OpFMul, etc.
- OpSelect ⭐ (our key instruction)
- OpCompositeConstruct/Extract/Insert
- OpExtInst

### Types
- Scalar: float, int, bool
- Vector: vec2, vec3, vec4

### Concepts
- SSA (Static Single Assignment)
- Scope
- Two-Pass Processing
- Predication
- Constant Pooling

---

**This glossary covers every technical term used in the compiler documentation!**
