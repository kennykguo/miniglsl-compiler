# Complete Compiler Walkthrough - Table of Contents

**A Comprehensive Guide to Understanding the MiniGLSL to SPIR-V Compiler**

Total: **4,321 lines** of detailed explanations across 6 parts

---

## 📖 Reading Order

### Part 1: Scanner (348 lines)
**File:** `EXPLAINED_PART1_SCANNER.md`

**Topics Covered:**
- What is lexical analysis?
- Breaking text into tokens
- Pattern matching (keywords, operators, numbers, identifiers)
- Float and integer validation
- Comment handling
- Why the `%{` marker must be at column 0
- Complete example: scanning a line of code

**Key Concepts:**
- Regular expressions
- Token types
- Whitespace handling
- Scanner generator (Flex)

**Time to read:** ~30 minutes

---

### Part 2: Parser (637 lines)
**File:** `EXPLAINED_PART2_PARSER.md`

**Topics Covered:**
- What is parsing?
- Grammar rules and AST construction
- C99-style mixed declarations (critical feature!)
- Operator precedence
- How scope splitting works
- Vector constructors and function calls
- The AST data structure

**Key Concepts:**
- Context-free grammars
- Abstract Syntax Tree (AST)
- Recursive descent
- Parser generator (Bison)
- Dangling else problem

**Time to read:** ~45 minutes

---

### Part 3: Semantic Analysis (784 lines)
**File:** `EXPLAINED_PART3_SEMANTIC.md`

**Topics Covered:**
- What is semantic analysis?
- Symbol table structure and scoping
- **THE TWO-PASS SOLUTION** (most important concept!)
- Why forward references need two passes
- Type checking expressions
- Checking statements (assignments, if/else)
- Built-in variables (gl_Color, gl_FragColor)

**Key Concepts:**
- Symbol tables
- Scope stacking
- Two-pass processing
- Type checking
- Read-only vs write-only variables

**Time to read:** ~50 minutes

---

### Part 4: Code Generation (1,017 lines) ⭐ LONGEST
**File:** `EXPLAINED_PART4_CODEGEN.md`

**Topics Covered:**
- What is SPIR-V?
- SSA (Static Single Assignment)
- **CONSTANT POOLING** with memory streams
- **OPSELECT PREDICATION** (critical for spec compliance!)
- Variable tracking system
- Expression generation (recursive)
- Indexed assignment (3-step process)
- Complete SPIR-V module structure

**Key Concepts:**
- SPIR-V assembly
- SSA IDs
- OpSelect vs branching
- Memory buffering
- Three-phase generation

**Time to read:** ~60-75 minutes

---

### Part 5: Complete Examples (785 lines)
**File:** `EXPLAINED_PART5_EXAMPLES.md`

**Topics Covered:**
- End-to-end compilation traces
- Simple declaration example (all 4 stages)
- Binary expressions with temporaries
- Nested expressions (recursion in action)
- If statements with OpSelect
- Vector operations (constructor, indexing)
- Function calls (dp3)
- Complete shader example

**Key Concepts:**
- Data flow through pipeline
- How recursion handles nesting
- SSA temporary generation
- Transformation at each stage

**Time to read:** ~45 minutes

---

### Part 6: Design Decisions (750 lines)
**File:** `EXPLAINED_PART6_DECISIONS.md`

**Topics Covered:**
- Why two-pass semantic analysis?
- Why OpSelect instead of branching?
- Why constant pooling with memory streams?
- Why C99-style parser?
- Why linked lists for AST?
- Why union for node types?
- All major design tradeoffs
- What we DIDN'T do (and why)

**Key Concepts:**
- Requirements vs implementation
- Correctness vs performance tradeoffs
- Simplicity vs generality
- Academic project priorities

**Time to read:** ~40 minutes

---

## 🎯 How to Use This Guide

### If You Want to Understand the Code:
1. Start with **Part 1** (Scanner)
2. Read in order through **Part 4** (Code Generation)
3. Read **Part 5** (Examples) to see it all together
4. Read **Part 6** (Decisions) to understand WHY

**Total time:** ~4-5 hours for deep understanding

### If You're Debugging an Issue:
- **Scanner error?** → Part 1
- **Parse error?** → Part 2, focus on grammar rules
- **Undeclared variable?** → Part 3, focus on two-pass section
- **Wrong SPIR-V output?** → Part 4, find the relevant expression/statement type
- **Need example?** → Part 5

### If You're Learning Compiler Design:
- Read all 6 parts in order
- Pay special attention to:
  - **Part 2:** How grammar rules build AST
  - **Part 3:** Two-pass solution for forward references
  - **Part 4:** OpSelect predication and constant pooling
  - **Part 6:** Design tradeoffs

### If You're Reviewing for an Exam:
- **Part 5** has the best examples
- **Part 6** explains the "why" questions
- Each part's summary section is a good review

---

## 📊 Content Breakdown

### By Topic:

**Lexical Analysis (Scanner):**
- Part 1: 348 lines

**Syntax Analysis (Parser):**
- Part 2: 637 lines

**Semantic Analysis:**
- Part 3: 784 lines

**Code Generation:**
- Part 4: 1,017 lines
- Part 5: 785 lines (with examples)

**Design & Architecture:**
- Part 6: 750 lines

### By Difficulty:

**Beginner-Friendly:**
- Part 1 (Scanner) - concrete patterns
- Part 5 (Examples) - see it in action

**Intermediate:**
- Part 2 (Parser) - grammar rules
- Part 6 (Decisions) - tradeoffs

**Advanced:**
- Part 3 (Semantic) - two-pass is subtle
- Part 4 (Code Generation) - most complex

---

## 🔑 Most Important Sections

If you're short on time, focus on these:

1. **Part 2, lines 1-116:** C99-style parser (why we split scope)
2. **Part 3, lines 1-141:** Two-pass semantic analysis (forward references)
3. **Part 4, lines 1-96:** Infrastructure (constant pool, variable tracking)
4. **Part 4, lines 494-527:** OpSelect predication (the critical feature!)
5. **Part 5, Example 4:** Complete if statement trace
6. **Part 6, Decisions 2 & 3:** OpSelect and constant pooling rationale

---

## ✨ What Makes This Guide Different

**Not just "what" and "how", but "WHY":**
- Every design decision explained
- Alternatives considered
- Tradeoffs discussed

**Comprehensive examples:**
- 7 complete end-to-end traces in Part 5
- Shows exact data transformations
- Recursive call trees visualized

**Educational focus:**
- Assumes no prior compiler knowledge
- Explains standard techniques
- Points out subtle details

**Code-specific:**
- Exact line numbers referenced
- Shows actual data structures
- Traces through real code paths

---

## 🎓 Learning Outcomes

After reading this guide, you will understand:

✅ How text becomes tokens (lexical analysis)
✅ How tokens become trees (syntactic analysis)  
✅ How to check if code makes sense (semantic analysis)
✅ How to generate assembly (code generation)
✅ Why two-pass analysis is needed
✅ How OpSelect enables branchless if statements
✅ How constant pooling works with memory streams
✅ Why every design decision was made
✅ How all the pieces fit together

**You will be able to:**
- Trace any shader through the entire pipeline
- Explain why the code works
- Modify the compiler intelligently
- Apply these techniques to other compilers

---

## 📝 Summary Statistics

- **Total lines:** 4,321
- **Total parts:** 6
- **Code examples:** 30+
- **Complete traces:** 7
- **Design decisions explained:** 12
- **Time to read thoroughly:** 4-5 hours
- **Time to skim:** 1-2 hours

---

## 🚀 Next Steps After Reading

1. **Read the actual source code** with this guide alongside
2. **Trace a shader yourself** using Part 5 as a template
3. **Modify the compiler** (add a feature, fix a bug)
4. **Explain it to someone else** (best way to verify understanding)

---

## ✅ This is the Most Comprehensive Documentation

**More detailed than:**
- Standard textbooks (too general)
- Code comments (too terse)
- Academic papers (too abstract)
- Online tutorials (incomplete)

**This guide:**
- Explains THIS specific compiler
- At both high and low levels
- With complete examples
- And design rationale

**If you read all 6 parts, you will understand EVERYTHING.** 🎉

---

**Happy Learning!** 📚✨
