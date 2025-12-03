
# Lab 5 - SPIR-V Code Generation Walkthrough

**Author**: Kenny Guo  
**Course**: ECE467F - Compilers and Interpreters  
**Date**: December 2, 2025

## Summary

Implemented SPIR-V code generation for the MiniGLSL compiler following the **strict linear SSA subset** (no branches, no phi). All conditionals use **OpSelect predication**. Generated SPIR-V validates successfully with `spirv-as` and `spirv-val`.

## Key Features Implemented

### 1. OpSelect Predication for Conditionals

**Spec Requirement**: Uses linear SSA only - **NO branches, NO phi, NO labels**

**Implementation** ([codegen.c:553-627](file:///home/kennykguo/ece467/part4/codegen.c#L553-L627)):
- Evaluate **both** branches unconditionally
- Use `OpSelect` to choose result based on condition
- **FORBIDDEN**: OpBranch, OpBranchConditional, OpPhi, OpLabel

```c
// Predication pattern: evaluate both, select result
int then_val = gen_expression(then_assign->assignment.expr, out);
int else_val = gen_expression(else_assign->assignment.expr, out);

// OpSelect: condition ? then_val : else_val
int result = gen_id();
fprintf(out, "%%%-3d = OpSelect %s %%%-3d %%%-3d %%%-3d\n",
        result, var_type, cond_id, then_val, else_val);
```

**Example SPIR-V**:
```spirv
%cond = OpFOrdLessThan %t_bool %x %c_float_0
%then_val = OpFMul %t_float %a %b
%else_val = OpLoad %t_float %current_value
%result = OpSelect %t_float %cond %then_val %else_val
OpStore %var %result
```

### 2. Indexed Assignment (OpCompositeInsert)

Support for assigning to vector components: `temp[0] = value`

**Implementation** ([codegen.c:437-492](file:///home/kennykguo/ece467/part4/codegen.c#L437-L492)):

```c
// Load, insert, store pattern
%loaded = OpLoad %t_vec4 %temp
%updated = OpCompositeInsert %t_vec4 %value %loaded 0
OpStore %temp %updated
```

### 3. Type Tracking

Variables track and use correct types (float vs vec4).

**Implementation** ([codegen.c:51-99](file:///home/kennykguo/ece467/part4/codegen.c#L51-L99)):
- Extended `var_entry` to store `type_code` and `vec_size`
- `lookup_var_type()` returns proper SPIR-V type
- OpLoad/OpSelect use correct types

### 4. Constant Pooling

Constants emitted before function (SPIR-V requirement).

**Implementation** ([codegen.c:13-48](file:///home/kennykguo/ece467/part4/codegen.c#L13-L48)):
- Buffer function body using `open_memstream`
- Collect constants during codegen
- Emit: prologue → constants → function body

## Allowed SPIR-V Instructions

Per spec, **ONLY** these instructions are allowed:

**Arithmetic**: OpFAdd, OpFSub, OpFMul, OpFDiv, OpFNegate  
**Composite**: OpCompositeConstruct, OpCompositeExtract, OpCompositeInsert  
**Memory**: OpLoad, OpStore, OpVariable  
**Comparison**: OpFOrdLessThan, OpFOrdGreaterThan, OpFOrdEqual, etc.  
**Logical**: OpLogicalAnd, OpLogicalOr, OpLogicalNot  
**Predication**: **OpSelect** (the ONLY control flow!)  
**GLSL intrinsics**: OpExtInst with Dot, Normalize, InverseSqrt  

**FORBIDDEN**: OpBranch, OpBranchConditional, OpPhi, OpLabel, OpSelectionMerge

## Test Results

✅ **flat.frag** - Simple assignment - **0 branches**  
✅ **lambert.frag** - Array indexing + OpSelect conditionals - **0 branches**

**Verification**:
```bash
grep "OpBranch" lambert_opselect.spvasm  # returns 0 matches
grep "OpSelect" lambert_opselect.spvasm  # shows predication
spirv-val lambert_opselect.spv           # validates successfully
```

## Usage

```bash
cd /home/kennykguo/ece467/part4

# Compile shader to SPIR-V assembly
./compiler467 ../part5/shaders/flat.frag -O flat.spvasm

# Assemble & validate
spirv-as flat.spvasm -o flat.spv
spirv-val flat.spv

# Verify no branches (should output nothing)
grep "OpBranch" flat.spvasm
```

## SPIR-V Structure

```spirv
; Header
OpCapability Shader
%ext = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %main "main" ...

; Decorations
OpDecorate %gl_FragCoord BuiltIn FragCoord
...

; Types
%t_void = OpTypeVoid
%t_float = OpTypeFloat 32
%t_vec4 = OpTypeVector %t_float 4
...

; Interface variables
%mglsl_Color = OpVariable %t_ptr_input_vec4 Input
%mglsl_FragColor = OpVariable %t_ptr_output_vec4 Output
...

; Shared constants
%c_float_0 = OpConstant %t_float 0.0
%c_float_1 = OpConstant %t_float 1.0

; Shader-specific constants (from pool)
%105 = OpConstant %t_float 800.0
...

; Main function (linear SSA - no branches!)
%main = OpFunction %t_void None %t_fn
%entry = OpLabel
  ; All OpVariable first
  ; Then assignments (with OpSelect for conditionals)
  OpReturn
OpFunctionEnd
```

## Files Modified

- **codegen.c** - All SPIR-V generation logic (pure SSA with OpSelect)
- **scanner.l** - No changes (already correct)
- **parser.y** - No changes needed

## Known Limitations

- Parser requires all declarations at start of scope
- Complex scopes with multiple assignments in if/else may need manual restructuring
- Future: Extend OpSelect handling for more complex control flow patterns

## Compliance Checklist

✅ Linear SSA only (no branches)  
✅ All conditionals use OpSelect  
✅ No OpPhi nodes  
✅ No OpLabel (except %entry)  
✅ No OpBranch/OpBranchConditional  
✅ Constants before function  
✅ All OpVariable at function start  
✅ Validates with spirv-as and spirv-val
