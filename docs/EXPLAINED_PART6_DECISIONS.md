# The Complete Compiler Walkthrough - Part 6: Design Decisions & Why Things Are The Way They Are

**Understanding the Rationale Behind Implementation Choices**

---

## Purpose of This Part

Parts 1-5 explained **what** the code does and **how** it works. This part explains **why** it was designed this way.

---

## Decision 1: Why Two-Pass Semantic Analysis?

### The Alternative: Single-Pass with Forward Declarations

**Alternative approach:** Require forward declarations (like C):
```c
{
    float x;  // Declare first
    float y;  // Declare first
    
    x = 1.0;  // Then initialize
    y = x + 2.0;
}
```

**Pros:**
- Simpler implementation (one pass)
- Clear separation of declaration and initialization

**Cons:**
- Forces specific code style
- Reference shaders don't use this style!
- More verbose

### Why We Chose Two-Pass

**Problem:** Lab requirement (Section 5.1) states we must accept the reference shaders **as-is**.

**Reference shaders use C99 style:**
```c
{
    vec4 base = vec4(1.0, 0.2, 0.2, 1.0);  // Declare + initialize
    vec4 color = base;  // Uses 'base' in initializer!
}
```

**Two-pass allows this!**

**Pass 1:** Add both "base" and "color" to symbol table
**Pass 2:** Check initializers (both variables now exist)

**Decision:** Lab compliance required two-pass, even though it's more complex.

---

## Decision 2: Why OpSelect Instead of Branching?

### The Alternative: Traditional Branching

**Standard SPIR-V approach:**
```spirv
OpSelectionMerge %merge None
OpBranchConditional %cond %then %else

%then = OpLabel
  ...
  OpBranch %merge

%else = OpLabel
  ...
  OpBranch %merge

%merge = OpLabel
```

**Pros:**
- Only execute selected branch (more efficient)
- Standard SPIR-V pattern
- Matches reference SPIR-V from glslangValidator

**Cons:**
- Requires OpPhi nodes to merge values
- More complex control flow
- **FORBIDDEN BY LAB SPEC!**

### Why We Chose OpSelect

**Spec requirement (from spirv-spec-v2.org):**
> "uses linear SSA only (no branches, no phi)"
> "Not allowed: OpBranch, OpBranchConditional, OpPhi"

**Decision:** No choice! Spec explicitly forbids branching. Must use OpSelect.

**OpSelect approach:**
```spirv
%cond = OpFOrdLessThan %t_bool %x %y
%val_then = OpConstant %t_float 10.0
%val_else = OpConstant %t_float 20.0
%result = OpSelect %t_float %cond %val_then %val_else
OpStore %var %result
```

**Tradeoff:**
- **Cost:** Both branches evaluated (wasted work if condition known)
- **Benefit:** Simpler code, no control flow graph, spec-compliant

**Why acceptable?**
1. Spec requires it (no choice)
2. GPUs are good at parallel execution
3. Simpler to generate and verify
4. Predictable performance (no branch misprediction)

---

## Decision 3: Why Constant Pooling with Memory Streams?

### The Problem

**SPIR-V requirement:** Constants must come BEFORE function.

**But:** We don't know what constants we need until we generate expressions!

**Example:**
```c
float x = compute_something();  // What constants does this need?
```

We only discover constants AS we generate the code!

### Alternative 1: Two-Pass Code Generation

**Approach:**
- Pass 1: Scan AST to collect constants
- Pass 2: Generate actual code

**Pros:**
- Straightforward
- Separate concerns

**Cons:**
- Duplicates work (traverse AST twice)
- More code
- Harder to maintain

### Alternative 2: Pre-Allocate Known Constants

**Approach:**
-Emit common constants (0.0, 1.0) upfront
- Hope we don't need others

**Pros:**
- Simple

**Cons:**
- Wrong! We DO need other constants (1.5, 2.0, 3.14, etc.)
- Wastes constants we don't use

### Why We Chose Memory Streams

**Approach:**
1. Buffer function body in memory (`open_memstream`)
2. Collect constants in a list AS we generate
3. Emit: prologue → constants → function header → buffered body

**Pros:**
- Single pass through AST
- Only emit constants we actually use
- Clean separation (collect vs emit)

**Cons:**
- Requires understanding `open_memstream`
- More complex than naive approach

**Decision:** Best balance of correctness and efficiency. Standard technique in compilers!

---

## Decision 4: Why C99-Style Parser?

### The Alternative: Strict Declaration Blocks

**Original grammar:**
```yacc
scope: '{' declarations statements '}'
```

Forces:
```c
{
    // All declarations first
    float x;
    float y;
    
    // All statements second
    x = 1.0;
    y = 2.0;
}
```

**Pros:**
- Simpler grammar
- Easier to implement
- Clear phases

**Cons:**
- **Rejects reference shaders!**
- phong.frag, rings.frag, grid.frag all have mixed declarations

### Why We Chose Mixed Declarations

**Lab Section 5.1:**
> "Examine each shader in shaders/ and ensure your MiniGLSL parser accepts it."

**Key word: "accepts it"** - must parse as-is!

**Reference shaders have:**
```c
{
    vec4 base = ...;      // declaration
    if (r2 < outer) { }   // statement
    vec4 color = base;    // declaration AFTER statement!
}
```

**Solution:** Allow declarations in statement list, split during parsing.

**Decision:** Lab compliance required this. Had to modify grammar.

---

## Decision 5Why SSA IDs Start at 100?

### The Alternative: Start at 1

**Simple approach:**
```c
static int next_id = 1;
```

**Problem:** IDs 1-99 used by prologue!

**Prologue reserves:**
- Types: `%t_void`, `%t_float`, `%t_vec4`, etc.
- Pointers: `%t_ptr_func_float`, etc.
- Built-ins: `%mglsl_Color`, `%gl_FragCoord`, etc.
- Common constants: `%c_float_0`, `%c_float_1`

If these are assigned IDs 1-99, our generated code can't use those IDs!

### Why We Chose Starting at 100

**Simple rule:** Prologue uses 1-99, generated code uses 100+

**Pros:**
- Clear separation
- No ID conflicts
- Easy to debug (low IDs = prologue, high IDs = generated)

**Cons:**
- Wastes IDs 1-99 if we emit our own prologue

**Decision:** Since we use a fixed prologue (provided), starting at 100 avoids conflicts.

---

## Decision 6: Why Variable Tracking? (Type Information)

### The Problem

**SPIR-V requires correct types in OpLoad:**
```spirv
%result = OpLoad %t_float %var_id     ; If var is float
%result = OpLoad %t_vec4 %var_id      ; If var is vec4
```

**But:** When generating `OpLoad`, we only know the variable NAME, not its type!

### Alternative 1: Look Up in Symbol Table

**Approach:** For each OpLoad, query semantic analyzer's symbol table.

**Pros:**
- Reuses existing data structure

**Cons:**
- Codegen depends on semantic analyzer (coupling)
- Symbol table destroyed after semantic phase!
- Would need to pass symbol table to codegen

### Alternative 2: Store in AST

**Approach:** Add type info to VAR_NODE during semantic analysis.

**Pros:**
- Self-contained in AST

**Cons:**
- Modifies AST (semantic analysis changes data structure)
- Need to access AST during codegen

### Why We Chose Local Variable Tracking

**Approach:** Maintain separate `var_entry` list in codegen.

```c
typedef struct var_entry {
    char *name;
    int id;
    int type_code;
    int vec_size;
} var_entry;
```

**During Phase 1 (allocate):**
```c
add_local_var("x", 100, FLOAT_T, 0);
```

**During expression generation:**
```c
const char *type = lookup_var_type("x");  // Returns "%t_float"
```

**Pros:**
- Codegen self-contained (no coupling to semantic)
- Simple lookup (name → type)
- Clean separation of phases

**Cons:**
- Duplicates type info (also in semantic's symbol table)

**Decision:** Clean architecture worth the duplication. Each phase independent.

---

## Decision 7: Why Linked Lists for AST?

### The Alternative:Dynamic Arrays

**Array approach:**
```c
struct {
    node **children;
    int count;
} scope_node;
```

**Pros:**
- Random access (O(1))
- Cache-friendly

**Cons:**
- Need to know size ahead of time OR reallocate
- More complex memory management

### Why We Chose Linked Lists

**Linked list approach:**
```c
struct {
    node *item;
    node *next;
} list_node;
```

**build incrementally:**
```c
// In parser:
list = ast_allocate(LIST_NODE, new_stmt, list);  // Prepend
```

**Pros:**
- No reallocation needed
- Easy to build incrementally
- Simple memory management (free recursively)

**Cons:**
- Sequential access only (O(n))
- Pointer overhead

**Decision:** For compiler use case (build once, traverse once), linked lists are simpler and sufficient. Don't need random access.

---

## Decision 8: Why Recursive Expression Generation?

### The Alternative: Iterative with Stack

**Stack-based approach:**
```c
stack *s = create_stack();
push(s, root);
while (!empty(s)) {
    node *n = pop(s);
    process(n);
    push(s, n->children);
}
```

**Pros:**
- Explicit control over stack
- Can optimize stack size

**Cons:**
- More code
- Harder to understand
- Need to manage stack explicitly

### Why We Chose Recursion

**Recursive approach:**
```c
int gen_expression(node *expr, FILE *out) {
    switch (expr->kind) {
        case BINARY_EXPR:
            int left = gen_expression(expr->binary_expr.left, out);
            int right = gen_expression(expr->binary_expr.right, out);
            ...
    }
}
```

**Pros:**
- Mirrors tree structure
- Automatic stack management (call stack)
- Easy to understand (matches how we think about expressions)
- Less code

**Cons:**
- Stack depth limited by system
- Harder to optimize

**Decision:** MiniGLSL expressions aren't deeply nested (no risk of stack overflow). Recursion's simplicity wins.

---

## Decision 9: Why Separate Scanner and Parser?

### The Alternative: Combined Lexer-Parser

**Combined approach:** Parse characters directly
```c
parse_declaration() {
    skip_whitespace();
    type = read_until(' ');
    if (strcmp(type, "float") == 0) ...
}
```

**Pros:**
- One less phase
- Potentially faster

**Cons:**
- Mixes two concerns (lexical and syntactic)
- Hard to maintain
- Can't use parser generators (Bison)
- Can't use scanner generators (Flex)

### Why We Chose Separation

**Separation:**
- Scanner (Flex): Characters → Tokens
- Parser (Bison): Tokens → AST

**Pros:**
- **Separation of concerns**
- Can use tools (Flex + Bison)
- Scanner handles: whitespace, comments, number formats
- Parser handles: syntax, precedence, associativity
- Easier to test each phase independently

**Cons:**
- Two phases instead of one

**Decision:** Standard compiler design. Benefits far outweigh costs. Industry standard tooling (Flex/Bison) sealed the decision.

---

## Decision 10: Why Union for AST Nodes?

### The Alternative: Separate Structs

**Separate structs approach:**
```c
typedef struct binary_expr_node {
    int op;
    struct node *left;
    struct node *right;
} binary_expr_node;

typedef struct var_node {
    char *identifier;
    bool is_array;
} var_node;

// No common structure!
```

**Pros:**
- Type-safe (can't confuse node types)

**Cons:**
- Can't have generic `process_node()` function
- Hard to store in lists (need void* or wrapper)
- Each type separate (no polymorphism in C)

### Why We Chose Union

**Union approach:**
```c
typedef struct node_ {
    node_kind kind;  // Discriminator
    union {
        struct { ... } binary_expr;
        struct { ... } variable;
        ...
    };
} node;
```

**Pros:**
- **Polymorphism in C!**
- Can have generic `node*` type
- Switch on `kind` to access right union field
- Memory efficient (union overlays fields)

**Cons:**
- Not type-safe (can access wrong union field - but we check `kind`)

**Decision:** Only way to get polymorphism in C. Standard approach for tree structures in C compilers. The `kind` field provides safety.

---

## Decision 11: Why Emit SPIR-V Text Instead of Binary?

### The Alternative: Binary SPIR-V

**Binary format:**
- Compact
- Faster to process
- Standard format

**But:**
- Hard to debug (hex dumps)
- Need bit-level manipulation
- More complex to generate

### Why We Chose Text (Assembly)

**Text format (`.spvasm`):**
- Human-readable
- Easy to debug
- Easy to generate (printf!)
- Can use `spirv-as` to convert to binary

**Workflow:**
```
Our compiler → .spv asm → spirv-as → .spv (binary) → spirv-val
```

**Pros:**
- **Debuggable!** Can read generated code
- Printf-based generation (simple)
- Validation happens on text (easier to see errors)

**Cons:**
- Intermediate step (need spirv-as)
- Larger files

**Decision:** For a learning/academic project, readability trumps efficiency. Real compilers would emit binary directly, but we prioritize understanding.

---

## Decision 12: Why Process Declarations Before Statements?

### The Requirement

**SPIR-V requirement:** All `OpVariable` at function start before any other instructions.

**Correct:**
```spirv
%100 = OpVariable %t_ptr_func_float Function
%101 = OpVariable %t_ptr_func_vec4 Function
...                                            ; Then
%102 = OpLoad %t_float %100                    ; other code
```

**Wrong:**
```spirv
%100 = OpVariable %t_ptr_func_float Function
%102 = OpLoad %t_float %100                    ; Using variable
%101 = OpVariable %t_ptr_func_vec4 Function  ; ❌ OpVariable after use!
```

### Why We Split into Two Phases

**Phase 1:** Emit all `OpVariable` (from declarations)
**Phase 2:** Emit `OpStore` for initializers (from declarations)
**Phase 3:** Emit statement code

**Ensures:** All variables declared before any are used.

**Decision:** SPIR-V spec requirement. No alternative.

---

## Tradeoffs Summary

### Correctness vs Performance

**Choices favoring correctness:**
- Two-pass semantic (handles forward refs)
- OpSelect predication (spec-compliant but both branches evaluated)
- Text SPIR-V (readable but need assembly step)

**Why:** Academic/learning project. Correctness and understandability prioritized.

### Simplicity vs Generality

**Choices favoring simplicity:**
- Single scope level (no nested scopes beyond basic blocks)
- Simple OpSelect (warnings for complex cases)
- Fixed prologue (not generating all SPIR-V from scratch)

**Why:** MiniGLSL is simple enough.** Don't need full GLSL complexity.

### Independence vs Efficiency

**Choices favoring independence:**
- Separate variable tracking in codegen (vs reusing symbol table)
- Constant pooling (vs two-pass AST scan)
- Four distinct phases

**Why:** Clean architecture, testable components, easier to understand and modify.

---

## What We DIDN'T Do (And Why)

### 1. Optimizations

**Didn't:**
- Constant folding (`1 + 2` → `3`)
- Dead code elimination
- Common subexpression elimination

**Why:** Not required by lab. Correctness first.

### 2. Better Error Messages

**Didn't:**
- Show source code line
- Suggest fixes
- Pretty error formatting

**Why:** Minimal viable error reporting. Focus on core functionality.

### 3. Full GLSL Compatibility

**Didn't:**
- All GLSL types (matrices, arrays)
- All GLSL functions
- Preprocessor (#define, #include)

**Why:** MiniGLSL is a subset! Only implement what's needed for the reference shaders.

### 4. Binary SPIR-V Output

**Didn't:** Emit `.spv` directly

**Why:** Text (.spvasm) easier to debug. Use spirv-as for conversion.

### 5. Advanced Control Flow

**Didn't:**
- Loops
- Switch statements
- Break/continue

**Why:** Not in MiniGLSL spec, reference shaders don't use them.

---

## Summary: Design Philosophy

**Core Principles:**

1. **Spec Compliance First**
   - OpSelect because spec forbids branching
   - C99 declarations because reference shaders use them

2. **Separation of Concerns**
   - Each phase independent
   - Clean interfaces between stages

3. **Simplicity Over Performance**
   - Readable code prioritized
   - Optimizations deferred

4. **Standard Techniques**
   - Flex/Bison (industry standard)
   - SSA generation (modern compilers)
   - Recursive descent (clear, proven)

5. **Debuggability**
   - Text SPIR-V (readable)
   - Clear data structures (union with kind)
   - Explicit phases (easy to trace)

**The Result:**
- 560-line code generator that works correctly
- Passes all validation (5/5 shaders)
- Maintainable and understandable
- Spec-compliant (0 branches, linear SSA)

**For a learning/academic project, this is the right balance.**

---

## Final Thoughts

Every design decision was guided by:
1. **Requirements** (lab spec, SPIR-V spec)
2. **Constraints** (C language, fixed prologue)
3. **Goals** (correctness, understandability, maintainability)

Could it be faster? Yes (optimize constant pooling, inline work).
Could it be smaller? Yes (fewer phases, combined passes).
Could it handle more? Yes (full GLSL, optimizations).

**But:** For the given requirements (5 MiniGLSL shaders, educational project), this design achieves the goal with clean, understandable architecture.

**That's good engineering!** ✨

---

## This Concludes the Complete Compiler Walkthrough

You've now learned:
- **Part 1:** Scanner (tokens from text)
- **Part 2:** Parser (AST from tokens)
- **Part 3:** Semantic Analysis (type checking, two-pass)
- **Part 4:** Code Generation (SPIR-V, OpSelect, constant pooling)
- **Part 5:** Complete Examples (end-to-end data flow)
- **Part 6:** Design Decisions (why it's built this way)

**Total understanding: COMPLETE!** 🎉
