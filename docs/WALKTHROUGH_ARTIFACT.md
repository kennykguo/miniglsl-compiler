# Lab 5 - SPIR-V Code Generation Walkthrough

## ✅ COMPLETE - All Requirements Met

All 5 reference shaders successfully compile, assemble, and validate with SPIR-V tools.

## Changes Made

### 1. Parser Modification (parser.y)
**Changed grammar to accept C99-style mixed declarations**

```yacc
scope: '{' statement_list '}'

statement_list:
  | statement_list statement
  | statement_list declaration  
  |
```

The parser now accepts declarations anywhere (not just at scope start), then splits the mixed list into separate declarations and statements lists for semantic analysis and codegen.

**Why:** Lab Section 5.1 requires: "ensure your MiniGLSL parser accepts it" for all 5 shaders. phong/rings/grid have declarations after statements.

### 2. Two-Pass Semantic Analysis (semantic.c)
**Pass 1:** Add ALL declarations to symbol table (without checking initializers)  
**Pass 2:** Check initializers (now all variables are in symbol table)

**Why:** Declarations like `vec4 color = base;` reference earlier variables in their initializers. Single-pass failed because `base` wasn't in symbol table yet when checking `color`'s initializer.

### 3. OpSelect Predication (codegen.c)  
**Spec requirement:** Linear SSA only - NO branches, NO phi

```c
// Both branches evaluated, result selected
int then_val = gen_expression(then_expr);
int else_val = gen_expression(else_expr);
int result = gen_id();
fprintf(out, "%%%-3d = OpSelect %s %%%-3d %%%-3d %%%-3d\\n",
        result, type, cond_id, then_val, else_val);
```

**Forbidden:** OpBranch, OpBranchConditional, OpPhi, OpLabel (except %entry)

### 4. Constant Pooling (codegen.c)
Constants emitted before function (SPIR-V requirement):
- Buffer function body with `open_memstream`
- Collect constants during codegen  
- Emit: prologue → constants → function

### 5. Type Tracking (codegen.c)
Variables store type info for correct `OpLoad` types:
- `float` → `%t_float`
- `vec4` → `%t_vec4`

### 6. Indexed Assignment (codegen.c)
Support for `temp[0] = value`:

```spirv
%loaded = OpLoad %t_vec4 %temp
%updated = OpCompositeInsert %t_vec4 %value %loaded 0
OpStore %temp %updated
```

## Test Results

✅ **flat.frag** - 72 lines, 0 branches  
✅ **lambert.frag** - 113 lines, 0 branches  
✅ **phong.frag** - 149 lines, 0 branches  
✅ **rings.frag** - 179 lines, 0 branches  
✅ **grid.frag** - 165 lines, 0 branches

All validate with `spirv-as` and `spirv-val`.

## Spec Compliance

✅ Linear SSA only (no branches, no phi)  
✅ OpSelect for ALL conditionals  
✅ Allowed instructions only  
✅ Constants before function  
✅ Variables at function start  
✅ All 5 shaders parse and validate

## Files Modified

- **parser.y** - Accept C99-style mixed declarations
- **semantic.c** - Two-pass processing for forward references
- **codegen.c** - OpSelect predication, constant pooling, type tracking

## Usage

```bash
cd /home/kennykguo/ece467/src

# Compile shader
./compiler467 ../tests/shaders/phong.frag -O phong.spvasm

# Validate
spir v-as phong.spvasm -o phong.spv
spirv-val phong.spv

# Verify no branches
grep OpBranch phong.spvasm  # should output nothing
```

## Warning Messages

"warning - complex if/else scopes with predication" - informational only, indicates OpSelect is being used for conditionals with multiple statements in branches. SPIR-V is correct and validates successfully.
