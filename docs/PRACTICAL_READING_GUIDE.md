# Practical Reading Guide - How to Actually Read the Source Code

**Step-by-step guide to understanding the implementation**

---

## Before You Start

**Prerequisites:**
1. Read **BASICS_101.md** (compiler fundamentals)
2. Skim **GLOSSARY.md** (have it open for reference)
3. Read **EXPLAINED_INDEX.md** (know what's in the walkthrough)

**Tools you'll use:**
- Text editor (VSCode, Vim, etc.)
- This guide
- GLOSSARY.md (for term lookup)
- Part 5 of walkthrough (for examples)

---

## Reading Order (Source Code)

### Phase 1: Understand the Data Structures (30 minutes)

**File:** `src/ast.h`

**What to read:**
1. **Lines 21-45:** `node_kind` enum
   - See all the different AST node types
   - Notice: expressions, statements, declarations

2. **Lines 66-157:** `struct node_` definition
   - The union structure
   - Each node kind has different fields
   - Find: `binary_expr`, `variable`, `assignment`, `if_stmt`, `scope`

**Questions to answer:**
- How many expression node types are there?
- What fields does BINARY_EXPR_NODE have?
- What fields does IF_STMT_NODE have?
- Where is the next pointer for lists?

**Tips:**
- Draw a diagram of the union structure
- Pick one node type (like BINARY_EXPR), understand it fully

---

### Phase 2: Follow a Token (45 minutes)

**Trace:** How `float x = 1.5;` becomes tokens

#### Step 1: Scanner Patterns

**File:** `src/scanner.l`

**Read in order:**
1. **Lines 35-58:** Keyword patterns
   - Find: `"float"`  returns `FLOAT_T`
   - Find: `"if"` returns `IF`

2. **Lines 87-99:** Number patterns
   - Find: `[0-9]+\.[0-9]+` (float pattern)
   - Notice: calls `ParseFloat(yytext)`

3. **Lines 101-103:** Identifier pattern
   - Find: `[a-zA-Z_][a-zA-Z0-9_]*`
   - Notice: calls `ParseIdent(yytext)`

**Exercise:** Trace `float x = 1.5;`
- `float` → matches line 44 → retur

ns FLOAT_T
- ` ` → matches line 109 → skip
- `x` → matches line 101 → returns ID("x")
- ` ` → skip
- `=` → matches line 60 → returns ASSIGN
- ` ` → skip
- `1.5` → matches line 87 → returns FLOAT_C(1.5)
- `;` → matches line 79 → returns SEMICOLON

**Result:** `[FLOAT_T, ID("x"), ASSIGN, FLOAT_C(1.5), SEMICOLON]`

#### Step 2: Parser Rule

**File:** `src/parser.y`

**Find:** Declaration rule (around line 148)
```yacc
declaration: 
    type ID ';'
    | type ID ASSIGN expression ';'
```

**Match our tokens:**
- `FLOAT_T` matches `type`
- `ID` matches `ID`
- `ASSIGN` matches `ASSIGN`
- `FLOAT_C(1.5)` matches `expression` (which matches `FLOAT_LITERAL`)
- `SEMICOLON` matches `;`

**Action (lines 149-162):** Creates DECLARATION_NODE

**Exercise:**
- Find where the AST node is allocated
- What fields get set?
- Where does `"x"` come from? (Hint: `$2`)
- Where does `1.5` come from? (Hint: `$4`)

---

### Phase 3: Follow Semantic Analysis (45 minutes)

**File:** `src/semantic.c`

**The two-pass section:**

#### Pass 1: Lines 107-133

**What it does:** Adds declarations to symbol table

**Key code:**
```c
node *curr_decl = ast->scope.declarations;
while (curr_decl) {
    node *decl = curr_decl->list.item;
    if (decl && decl->kind == DECLARATION_NODE) {
        symbol_entry *entry = malloc(...);
        entry->name = strdup(decl->declaration.identifier);
        entry->type_code = ...;
        symbol_table_insert(current_scope, entry);
    }
    curr_decl = curr_decl->list.next;
}
```

**Exercise:**
- What happens to `float x = 1.5;`?
- What gets stored in the symbol entry?
- When does it check the initializer? (Hint: not here!)

#### Pass 2: Line 136

**What it does:** Checks initializers

**Key code:**
```c
process_declarations_in_order(ast->scope.declarations);
```

**Find this function:** Lines 150-188

**What it does now:**
```c
void check_declaration(node *decl) {
    // Symbol already in table (from pass 1)
    
    // Check initializer if present
    if (decl->declaration.initializer) {
        int init_type = check_expression(decl->declaration.initializer);
        int var_type = decl->declaration.type->type.type_code;
        
        if (init_type != var_type) {
            error("type mismatch");
        }
    }
}
```

**Exercise:**
- For `float x = 1.5;`, what is `init_type`?
- What is `var_type`?
- Do they match?

---

### Phase 4: Follow Code Generation (60 minutes)

**File:** `src/codegen.c`

**This is the longest - break it into pieces**

#### Part A: Infrastructure (lines 1-96)

**Read:**
1. **Lines 13-18:** Constant pool structure
2. **Lines 49-68:** Variable entry structure
3. **Lines 227-229:** ID allocation

**Understand:**
- How constants are collected
- How variables are tracked
- How SSA IDs are generated

#### Part B: Main Generation (lines 559-586)

**The memory stream pattern:**
```c
char *func_body = NULL;
size_t body_size = 0;
FILE *body_stream = open_memstream(&func_body, &body_size);

// Generate into memory
fprintf(body_stream, "%%entry = OpLabel\n");
process_decls_allocate(..., body_stream);
...
fclose(body_stream);

// Emit in order
emit_prologue(output);
emit_constants(output);
fprintf(output, "%%main = ...");
fprintf(output, "%s", func_body);
free(func_body);
```

**Understand:**
- Why use memory stream?
- What order things are emitted?

#### Part C: Variable Processing (lines 237-265)

**Phase 1: Allocate**
```c
int var_id = gen_id();
fprintf(out, "%%%-3d = OpVariable ...\n", var_id);
add_local_var(name, var_id, type_code, vec_size);
```

**Phase 2: Initialize**
```c
int var_id = lookup_var(name);
int init_id = gen_expression(initializer, out);
fprintf(out, "OpStore %%%-3d %%%-3d\n", var_id, init_id);
```

**Exercise:** For `float x = 1.5;`:
- What var_id is allocated?
- What constant is generated?
- What OpStore is emitted?

#### Part D: Expression Generation (lines 267-428)

**Most important function:** `gen_expression()`

**For FLOAT_LITERAL (lines 272-277):**
```c
int const_id = gen_id();
char buf[100];
sprintf(buf, "%%%-3d = OpConstant %%t_float %f\n", const_id, value);
add_const(const_id, buf);
return const_id;
```

**Exercise:** For literal `1.5`:
- What ID is generated?
- What gets added to constant pool?
- What ID is returned?

**For BINARY_EXPR (lines 302-336):**
```c
int left_id = gen_expression(expr->binary_expr.left, out);
int right_id = gen_expression(expr->binary_expr.right, out);
int result = gen_id();
fprintf(out, "%%%-3d = %s %s %%%-3d %%%-3d\n",
        result, op_name, type, left_id, right_id);
return result;
```

**Exercise:** For `x + y`:
- What are the recursive calls?
- What OpLoads are generated?
- What OpFAdd is generated?

#### Part E: OpSelect (lines 494-527)

**The critical code:**
```c
int cond_id = gen_expression(stmt->if_stmt.condition, out);
int then_val = gen_expression(then_expr, out);
int else_val = gen_expression(else_expr, out);
int result = gen_id();
fprintf(out, "%%%-3d = OpSelect %s %%%-3d %%%-3d %%%-3d\n",
        result, type, cond_id, then_val, else_val);
OpStore %var %result;
```

**Exercise:** For `if (x < 2) { y = 10; } else { y = 20; }`:
- What is cond_id? (from `x < 2`)
- What is then_val? (from `10`)
- What is else_val? (from `20`)
- What OpSelect is emitted?

---

## Practical Exercises

### Exercise 1: Trace a Simple Declaration

**Code:** `{ float x = 1.5; }`

**Do:**
1. Identify tokens (use scanner.l)
2. Find grammar rules (use parser.y)
3. Draw the AST
4. Show symbol table after semantic pass 1
5. Show what semantic pass 2 checks
6. Write the generated SPIR-V (use codegen.c)

**Check your answer:** Look at Part 5, Example 1

---

### Exercise 2: Trace a Binary Expression

**Code:** `{ float z = x + y; }`

**Do:**
1. Tokens
2. AST (focus on the `x + y` subtree)
3. Semantic checks (are x, y declared? types match?)
4. SPIR-V generation (show OpLoads, OpFAdd)

**Check your answer:** Part 5, Example 2

---

### Exercise 3: Trace an If Statement

**Code:** `{ if (x < 2.0) { y = 10.0; } }`

**Do:**
1. AST for IF_STMT_NODE
2. Semantic checks on condition
3. Code generation with OpSelect
4. Show ALL SPIR-V instructions

**Check your answer:** Part 5, Example 4

---

## Reading Strategies

### Strategy 1: Top-Down

Start with high-level (main function), drill down:

1. `main()` in `compiler467.c`
2. Calls scanner, parser, semantic, codegen
3. For each phase, read the main entry point
4. Then read helper functions

**Good for:** Understanding overall flow

---

### Strategy 2: Bottom-Up

Start with data structures, build up:

1. Read `ast.h` (understand nodes)
2. Read `symbol.h` (understand symbol table)
3. Read simple functions first
4. Combine into complex functions

**Good for:** Understanding details

---

### Strategy 3: Example-Driven

Pick an example, trace it completely:

1. Choose shader (e.g., `flat.frag`)
2. Manually trace through scanner
3. Manually trace through parser
4. Check symbol table
5. Generate SPIR-V by hand
6. Compare to actual output

**Good for:** Seeing the whole picture

---

### Strategy 4: Question-Driven

Have a specific question, find the answer:

1. "How does OpSelect work?" → Read codegen.c lines 494-527
2. "Why two-pass?" → Read semantic.c lines 99-141
3. "How are floats parsed?" → Read scanner.l lines 87-99

**Good for:** Targeted learning

---

## Debugging Your Understanding

### Test Yourself

**After reading scanner:**
- Can you identify all tokens in `vec4 x = vec4(1,0,0,1);`?

**After reading parser:**
- Can you draw the AST for `if (x < 10) { y = 5; }`?

**After reading semantic:**
- Can you explain why two-pass is needed?

**After reading codegen:**
- Can you write SPIR-V for `z = x + y;` by hand?

**If you can answer YES to all → you understand!**

---

## Common Reading Mistakes

### Mistake 1: Reading linearly

**Don't:** Read files line 1 to line N

**Do:** Read by functionality (all expression generation together)

---

### Mistake 2: Not drawing diagrams

**Don't:** Just read text

**Do:** Draw:
- AST trees
- Call graphs
- Data flow diagrams

---

### Mistake 3: Not tracing examples

**Don't:** Read code abstractly

**Do:** Pick concrete example, trace through

---

### Mistake 4: Skipping the "boring" parts

**Don't:** Skip data structure definitions

**Do:** Understand data first, then algorithms

---

## Helpful Commands While Reading

### See generated SPIR-V
```bash
cd /home/kennykguo/ece467/src
./compiler467 ../tests/shaders/flat.frag -O flat.spvasm
cat flat.spvasm
```

### See tokens (add debug to scanner)
```c
// In scanner.l, add to each rule:
printf("TOKEN: %s\n", yytext);
```

### See AST (use ast_print)
```c
// In compiler467.c, after parsing:
ast_print(ast);
```

### Run with test input
```bash
echo '{ float x = 1.5; }' | ./compiler467 /dev/stdin -O test.spvasm
cat test.spvasm
```

---

## Study Plan

### Week 1: Foundations
- Day 1-2: BASICS_101.md + GLOSSARY.md
- Day 3-4: Part 1 (Scanner) + Part 2 (Parser)
- Day 5: Trace examples manually
- Day 6-7: Read scanner.l and parser.y

### Week 2: Core Concepts
- Day 1-3: Part 3 (Semantic) + read semantic.c
- Day 4-5: Part 4 (Codegen) + start reading codegen.c
- Day 6-7: Finish codegen.c, trace examples

### Week 3: Integration
- Day 1-2: Part 5 (Complete examples)
- Day 3-4: Part 6 (Design decisions)
- Day 5: Trace all 5 reference shaders
- Day 6-7: Try modifying the compiler

**Total time:** ~3 weeks for deep understanding

---

## Quick Reference

**When you're stuck on:**
- **Tokens** → scanner.l + Part 1
- **Grammar** → parser.y + Part 2  
- **Types** → semantic.c + Part 3
- **SPIR-V** → codegen.c + Part 4
- **Examples** → Part 5
- **Why?** → Part 6
- **Terms** → GLOSSARY.md
- **Common issues** → FAQ.md

---

## Final Checklist

Before you say "I understand the compiler", make sure you can:

- [ ] Identify all tokens in a shader
- [ ] Draw the AST for any code snippet
- [ ] Explain why two-pass semantic is needed
- [ ] Trace an expression through codegen
- [ ] Explain how OpSelect works
- [ ] Describe constant pooling
- [ ] Write simple SPIR-V by hand
- [ ] Modify the compiler (add a feature)
- [ ] Debug a problem in generated SPIR-V
- [ ] Explain design decisions

**All checked? Congratulations - you understand the compiler!** 🎉

---

**Start reading with Phase 1 (ast.h) when you're ready!**
