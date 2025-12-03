# Frequently Asked Questions (FAQ)

**Common Questions and Confusion Points Addressed**

---

## General Understanding

### Q: What exactly does this compiler do?

**A:** It translates MiniGLSL fragment shader code into SPIR-V assembly that GPUs can execute.

**Input:** Text file like `flat.frag` with shader code
**Output:** SPIR-V assembly file `.spvasm` with GPU instructions

**Real example:**
```c
// Input: shader code
{
    gl_FragColor = gl_Color;
}

// Output: SPIR-V
%1 = OpLoad %t_vec4 %mglsl_Color
OpStore %mglsl_FragColor %1
```

---

### Q: Why are there 4 stages? Can't we do it in fewer?

**A:** Each stage has a distinct job. Combining them makes code more complex.

**Why separate scanner and parser?**
- Scanner: recognize patterns (is "123.45" a valid number?)
- Parser: understand structure (is `float x = 1.0;` valid syntax?)
- Different problems, different solutions

**Why separate semantic and codegen?**
- Semantic: check correctness (does variable exist? types match?)
- Codegen: produce output (how to emit SPIR-V?)
- Don't generate code if it's wrong!

**Analogy:** Building a house
1. Blueprint (scanner/parser - understand what's needed)
2. Permits (semantic - verify it's legal)
3. Construction (codegen - actually build it)

You wouldn't start building before checking the blueprint is legal!

---

### Q: What's the difference between a token and an AST node?

**A:** Tokens are "words", AST nodes are "concepts".

**Token:** Flat list
```
[FLOAT_T] [ID("x")] [ASSIGN] [FLOAT_C(1.5)] [SEMICOLON]
```

**AST Node:** Structured tree
```
DECLARATION_NODE
  ├─ type: float
  ├─ name: "x"  
  └─ value: 1.5
```

**Key difference:**
- Tokens preserve EVERY character (even semicolons)
- AST captures meaning (semicolon is syntax, not meaning)

**Analogy:**
- Tokens = letters in a word
- AST = the concept the word represents

---

## Two-Pass Semantic Analysis

### Q: Why do we need two passes? This seems overcomplicated!

**A:** Because initializers can reference other variables from the same scope.

**Problem code:**
```c
{
    vec4 base = vec4(1,0,0,1);
    vec4 color = base;  // Uses 'base'!
}
```

**Single-pass attempt:**
1. Process `base` declaration: add to table ✓, check initializer ✓
2. Process `color` declaration: check initializer `base`... 
   - Wait, is `base` in the symbol table yet?
   - It is! ✓ (because we added it in step 1)

**Hmm, that worked! So why two passes?**

**This is the real problem:**
```c
{
    vec4 a = b;  // Uses 'b' before it's processed!
    vec4 b = vec4(1,0,0,1);
}
```

**Single-pass:**
1. Process `a` declaration: check initializer `b`...
   - Is `b` in symbol table? NO! ❌ Error!

**Two-pass:**
- Pass 1: Add BOTH `a` and `b` to table
- Pass 2: Check initializers (both now in table) ✓

**The order doesn't matter with two-pass!**

---

### Q: But the reference shaders don't use this pattern (vec4 a = b). Why implement it?

**A:** They DO use it, just more subtly:

**From phong.frag:**
```c
{
    vec4 base = ...;
    vec4 color = base;  // Later variable referencing earlier
}
```

If there were 10 declarations and the 8th referenced the 2nd, single-pass would work. But if the 2nd referenced the 8th? Break!

**Two-pass handles ALL patterns, regardless of order.**

---

### Q: Doesn't adding to symbol table in Pass 1 duplicate work?

**A:** Small price for correctness.

**Alternative:** Require declarations in dependency order
- User would have to manually order declarations
- Compiler would be simpler
- **But:** Lab spec says accept reference shaders as-is!

**Our choice:** Extra work in compiler, flexibility for user.

---

## OpSelect vs Branching

### Q: Why use OpSelect? Branching seems more natural!

**A:** Spec forbids it!

**From spec:**
> "uses linear SSA only (no branches, no phi)"
> "Not allowed: OpBranch, OpBranchConditional, OpPhi"

**No choice - it's a requirement.**

---

### Q: Doesn't OpSelect waste work by evaluating both branches?

**A:** Yes, but GPUs are good at this.

**Example:**
```c
if (x < 10)
    y = expensive_computation_1();
else
    y = expensive_computation_2();
```

**OpSelect approach:** Compute BOTH, then select.

**Is this bad?**
- On CPU: Yes, wasteful
- On GPU: Not as bad (massively parallel)
- **For simple expressions:** Negligible cost

**Our shaders:** Mostly simple assignments, not expensive computations.

---

### Q: What if both branches have side effects?

**A:** MiniGLSL doesn't have side effects in expressions!

**Side effect:** Action that changes state
- Example: `x++` (modifies x)
- Example: `printf("hello")` (I/O)

**MiniGLSL expressions:** Pure (no side effects)
- Can safely evaluate both
- Selecting between two values, not two actions

---

## Constant Pooling

### Q: Why can't we just emit constants when we encounter them?

**A:** SPIR-V requires constants BEFORE the function.

**If we emit as we go:**
```spirv
%main = OpFunction ...
%entry = OpLabel
%100 = OpVariable ...
%101 = OpConstant %t_float 1.0  ← TOO LATE! Must be before function
OpStore %100 %101
```

**spirv-val would reject this:** Constants in wrong place.

---

### Q: Why not scan the AST first to collect constants?

**A:** We'd traverse the AST twice (more complex).

**Two-scan approach:**
```
Scan 1: Walk AST, collect constants
Scan 2: Walk AST, generate code
```

**Our approach:**
```
Single scan: Generate code to memory buffer, collect constants
Emit: Constants first, then buffered code
```

**Trade-off:** Memory buffer complexity vs two scans

**We chose:** One scan (less duplication, same result)

---

### Q: What's `open_memstream` doing?

**A:** Creates a "file" that writes to RAM instead of disk.

**Normal file:**
```c
FILE *f = fopen("output.txt", "w");
fprintf(f, "Hello");
fclose(f);
// Text goes to disk file
```

**Memory stream:**
```c
char *buffer = NULL;
size_t size = 0;
FILE *f = open_memstream(&buffer, &size);
fprintf(f, "Hello");
fclose(f);
// Now 'buffer' contains "Hello" in RAM!
```

**Why useful:** Collect output in memory, emit it later in different order.

---

## C99-Style Parser

### Q: Why allow declarations mixed with statements?

**A:** Reference shaders (phong, rings, grid) use this style.

**Lab requirement (Section 5.1):**
> "ensure your MiniGLSL parser accepts it"

Must accept shaders **as-is**, can't require modification.

---

### Q: Isn't the old way (declarations first) cleaner?

**A:** Matter of preference.

**Old way (C89):**
```c
{
    float x;
    float y;
    
    x = 1.0;
    y = 2.0;
}
```

**New way (C99/modern):**
```c
{
    float x = 1.0;
    if (...) { }
    float y = 2.0;  // Declaration after statement
}
```

**C99 style:**
- Declare near use
- Initialize immediately
- More flexible

**Our compiler:** Supports C99 for spec compliance.

---

### Q: How does the parser "split" the mixed list?

**A:** During the scope action, it categorizes each item.

**Code (parser.y lines 95-116):**
```yacc
scope: '{' statement_list '}'
{
    node *decls = NULL;
    node *stmts = NULL;
    
    // Walk the mixed list
    node *curr = $2;
    while (curr) {
        node *item = curr->list.item;
        
        // Check the node kind
        if (item->kind == DECLARATION_NODE) {
            decls = ast_allocate(DECLARATIONS_NODE, item, decls);
        } else {
            stmts = ast_allocate(STATEMENTS_NODE, item, stmts);
        }
        curr = curr->list.next;
    }
    
    // Create scope with separated lists
    $$ = ast_allocate(SCOPE_NODE, decls, stmts);
}
```

**Process:**
1. Parser builds single mixed list during parsing
2. Scope action separates by checking `node->kind`
3. Result: Two separate lists

---

## SPIR-V Specific

### Q: What does the % symbol mean in SPIR-V?

**A:** Marks an SSA ID (identifier).

**Example:**
```spirv
%100 = OpVariable ...
%101 = OpConstant %t_float 1.0
%102 = OpFAdd %t_float %100 %101
```

- `%100`, `%101`, `%102` are IDs
- Each ID assigned exactly once (SSA)
- Like variable names, but numbered

---

### Q: Why do some IDs start with % and some with %t_ or %c_?

**A:** Naming convention (not a rule).

**Patterns:**
- `%t_...` - Types (e.g., `%t_float`, `%t_vec4`)
- `%c_...` - Common constants (e.g., `%c_float_0`, `%c_float_1`)
- `%mglsl_...` - MiniGLSL built-ins (e.g., `%mglsl_Color`)
- `%100`, `%101` - Generated IDs

**SPIR-V doesn't care about names** - only IDs matter. Naming just helps readability.

---

### Q: What's the difference between OpLoad and OpVariable?

**A:**
- **OpVariable:** Create storage (allocate memory)
- **OpLoad:** Read value from storage

**Analogy:**
- OpVariable = "Create a box labeled 'x'"
- OpLoad = "Look inside the box and read the value"

**Example:**
```spirv
%100 = OpVariable %t_ptr_func_float Function  ; Create box
%101 = OpConstant %t_float 1.0                ; Value 1.0
OpStore %100 %101                             ; Put 1.0 in box
%102 = OpLoad %t_float %100                   ; Read from box → get 1.0
```

**Why separate?** SPIR-V uses pointers. Variables are addresses, loads dereference.

---

### Q: Why do we need OpCompositeInsert for indexed assignment?

**A:** SSA = values are immutable. Can't modify in place.

**Problem:**
```c
color[1] = 0.8;  // Change component 1
```

**Can't do:**
```spirv
color[1] = 0.8  ← SPIR-V doesn't have this!
```

**Must do:**
```spirv
%old = OpLoad %t_vec4 %color        ; Load [a, b, c, d]
%new = OpCompositeInsert %old 0.8 1  ; Create [a, 0.8, c, d]
OpStore %color %new                  ; Store new vector
```

**SSA rule:** Each ID assigned once, so create NEW vector with modified component.

---

## Implementation Details

### Q: Why use linked lists instead of arrays for the AST?

**A:** Don't know size ahead of time, lists grow easily.

**Array problem:**
```c
node* children[???];  // How many children?
```

Would need to:
1. Count children first
2. Allocate array
3. Fill array

Or:
1. Start with small array
2. Reallocate when full
3. Copy to new array

**Linked list:**
```c
list = ast_allocate(LIST_NODE, new_item, list);  // Just prepend!
```

**Trade-off:**
- Arrays: Fast access, fixed size
- Lists: Easy growth, sequential access

**Our case:** Build once, traverse once → lists are perfect.

---

### Q: Why does SSA ID allocation start at 100?

**A:** IDs 1-99 reserved for prologue (types, built-ins, etc.).

**Prologue uses:**
- %1-%50: Type definitions
- %51-%99: Built-in variables, common constants

**Our generated code:**
- %100+: Variables, temporaries, constants

**Clean separation:** Low IDs = prologue, high IDs = generated.

---

### Q: What if we run out of IDs?

**A:** Won't happen in practice.

**ID space:** Typically 32-bit integers = 2 billion IDs

**Our shaders:** Use a few hundred IDs at most

**Even grid.frag (most complex):** ~180 lines of SPIR-V ≈ 200 IDs

**2 billion >> 200** - we're safe!

---

## Debugging

### Q: My shader doesn't parse. How do I debug?

**A:**

1. **Check scanner first:**
   ```bash
   ./compiler467 shader.frag 2>&1 | head -20
   ```
   Look for "syntax error" - might be tokenization issue

2. **Check for common mistakes:**
   - Missing semicolon
   - Unmatched braces `{}`
   - Typo in keyword (`flaot` instead of `float`)

3. **Try minimal example:**
   ```c
   { float x = 1.0; }
   ```
   If this works, add back code piece by piece

---

### Q: I get "undeclared variable" error but the variable IS declared!

**A:** Check:

1. **Spelled correctly?**
   ```c
   float color = ...;
   result = colour;  // Typo! Different name
   ```

2. **In scope?**
   ```c
   {
       float x = 1.0;
   }
   y = x;  // x out of scope!
   ```

3. **Declared before use?** (shouldn't matter with two-pass, but check)

4. **Try small example:**
   ```c
   { float x = 1.0; float y = x; }
   ```

---

### Q: spirv-val says my SPIR-V is invalid. What's wrong?

**A:** Common issues:

1. **Constants after function:**
   - Should be: prologue → constants → function
   - Bug in constant pooling

2. **Variables not at function start:**
   - All OpVariable must be right after OpLabel
   - Check Phase 1 of codegen

3. **Type mismatch:**
   - OpLoad type doesn't match variable type
   - Bug in variable tracking

**Debug:** Look at the generated `.spvasm` file, find the line spirv-val complains about.

---

### Q: The compiler crashes! What do I do?

**A:**

1. **Run in debugger:**
   ```bash
   gdb ./compiler467
   run shader.frag -O out.spvasm
   ```

2. **Check for:**
   - NULL pointer dereference
   - Infinite recursion (stack overflow)
   - Memory corruption

3. **Common causes:**
   - Forgot to check if node is NULL
   - Recursive call without base case
   - Wrong union field access

---

## Conceptual

### Q: What's the difference between parsing and compiling?

**A:** Parsing is ONE STEP of compiling.

**Compiling:** Entire process (all 4 stages)
**Parsing:** Just stage 2 (tokens → AST)

**Analogy:**
- Compiling = baking a cake (entire process)
- Parsing = mixing ingredients (one step)

---

### Q: Is SPIR-V like assembly language?

**A:** Similar concept, different target.

**CPU assembly:**
- x86, ARM, etc.
- Runs on CPU

**SPIR-V:**
- "GPU assembly"
- Runs on GPU
- Higher level than CPU assembly (has types, vectors)

---

### Q: Why learn about compilers? When will I use this?

**A:** Compiler techniques apply broadly:

**Direct uses:**
- Writing DSLs (domain-specific languages)
- Code analysis tools
- Syntax highlighters
- Formatters, linters

**Concepts that transfer:**
- Parsing (JSON, XML, configs)
- Tree structures (any hierarchical data)
- Symbol tables (any name→info mapping)
- Pattern matching (regex, templating)

**Even if you never write a compiler, understanding them makes you a better programmer!**

---

## Meta Questions

### Q: Which part should I focus on most?

**A:** Depends on your goal:

**For understanding this specific compiler:**
- Part 3 (Semantic - two-pass) ⭐
- Part 4 (Codegen - OpSelect) ⭐

**For learning compiler design generally:**
- Part 2 (Parser - grammar)
- Part 3 (Semantic - type checking)

**For debugging issues:**
- Part 5 (Examples - see it in action)

---

### Q: How long does it take to understand everything?

**A:** Varies by background:

**If you know programming well:**
- Skim: 2-3 hours
- Understand: 5-6 hours  
- Master: 10-15 hours (including reading code)

**If new to compilers:**
- Start with BASICS_101.md
- Budget 8-10 hours for thorough understanding
- Re-read confusing sections

**Tips:**
- Don't rush
- Try the examples yourself
- Draw the trees/diagrams
- Explain it to someone else

---

### Q: What if I still don't understand something?

**A:** Resources:

1. **Re-read relevant section** - Sometimes makes sense second time

2. **Check GLOSSARY.md** - Define unfamiliar terms

3. **Read Part 5 examples** - Seeing it in action helps

4. **Look at actual code** - Sometimes concrete code clearer than abstract explanation

5. **Search specific topic** - Many compiler concepts have good online resources

---

## Summary: Most Common Confusions

1. **Two-pass semantic:** Needed for forward references in initializers
2. **OpSelect vs branching:** Spec requires OpSelect (no choice)
3. **Constant pooling:** SPIR-V wants constants before function
4. **C99 parser:** Reference shaders require it
5. **OpLoad/OpStore:** Variables are pointers, must load/store
6. **OpCompositeInsert:** SSA = immutable, must create new vector
7. **Tokens vs AST:** Tokens are words, AST is concepts
8. **Four stages:** Each has distinct job, separation simplifies

---

**Still confused? Check the specific part of the walkthrough that covers your question!**
