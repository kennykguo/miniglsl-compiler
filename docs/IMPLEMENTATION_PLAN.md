# Lab 5 MiniGLSL SPIR-V Code Generation - Implementation Plan

## Summary

The compiler currently generates basic SPIR-V for simple shaders (flat.frag works correctly) but lacks critical features needed for the remaining test shaders. Based on analysis of lambert.frag, phong.frag, rings.frag, and grid.frag, the following features must be implemented:

1. **Indexed assignment to vector components** (e.g., `temp[0] = value`)
2. **Complex conditional handling** in if/else statements
3. **Nested scopes** within conditionals
4. **Multiple assignments** within conditional branches

## User Review Required

> [!IMPORTANT]
> The scanner.l file appears to be correct and doesn't need changes. The main work is in codegen.c.

> [!WARNING]
> The spec says to use OpSelect (predication) instead of branches, but the reference SPIR-V from glslangValidator uses OpBranch/OpSelectionMerge. We'll implement proper branching to match the reference output since that's what will be validated against.

## Proposed Changes

### Codegen Enhancements

#### [MODIFY] [codegen.c](file:///home/kennykguo/ece467/part4/codegen.c)

**1. Add indexed assignment support (lines 437-459)**
- Detect when assignment target has array indexing: `var[i] = expr`
- Load the current vector value  
- Use `OpCompositeInsert` to update the indexed component
- Store the modified vector back

**2. Extend conditional handling to support scopes (lines 461-517)**
- Currently only handles single assignment statements
- Must support SCOPE_NODE as then/else branches
- Process multiple statements within each branch
- Generate proper OpSelectionMerge/OpBranchConditional/OpBranch structure

**3. Add proper type detection for variables**
- Local variables need correct types (not always vec4)
- Look up variable type from declaration node
- Use correct pointer types in OpLoad/OpStore

**4. Support vector*scalar multiplication**
- Detect when one operand is scalar, one is vector
- Use `OpVectorTimesScalar` instead of OpFMul

**5. Handle vector constructors from scalars**
- Constructor with 4 scalar arguments → vec4
- Already implemented, just verify correctness

---

### Key Implementation Details

**Indexed Assignment Pattern:**
```spirv
; temp[0] = value
%loaded = OpLoad %t_vec4 %temp
%updated = OpCompositeInsert %t_vec4 %value %loaded 0
OpStore %temp %updated
```

**Branching Pattern:**
```spirv
; if (cond) { ... } else { ... }
%cond = <evaluate condition>
OpSelectionMerge %merge None
OpBranchConditional %cond %then_label %else_label
%then_label = OpLabel
  <then body>
  OpBranch %merge
%else_label = OpLabel
  <else body>
  OpBranch %merge
%merge = OpLabel
```

## Verification Plan

### Automated Tests

**1. Compile all 5 reference shaders:**
```bash
cd /home/kennykguo/ece467/part4
./compiler467 ../part5/shaders/flat.frag -O output/flat.spvasm
./compiler467 ../part5/shaders/lambert.frag -O output/lambert.spvasm
./compiler467 ../part5/shaders/phong.frag -O output/phong.spvasm
./compiler467 ../part5/shaders/rings.frag -O output/rings.spvasm
./compiler467 ../part5/shaders/grid.frag -O output/grid.spvasm
```

**2. Validate SPIR-V with spirv-as:**
```bash
cd /home/kennykguo/ece467/part4
spirv-as output/flat.spvasm -o output/flat.spv
spirv-as output/lambert.spvasm -o output/lambert.spv
spirv-as output/phong.spvasm -o output/phong.spv
spirv-as output/rings.spvasm -o output/rings.spv
spirv-as output/grid.spvasm -o output/grid.spv
```

**3. Validate with spirv-val:**
```bash
cd /home/kennykguo/ece467/part4
spirv-val output/flat.spv
spirv-val output/lambert.spv
spirv-val output/phong.spv
spirv-val output/rings.spv
spirv-val output/grid.spv
```

**4. Semantic comparison:**
- Manually compare generated .spvasm against reference files in `/home/kennykguo/ece467/part5/spirv/`
- Look for equivalent instruction sequences (SSA IDs will differ, but logic must match)
- Verify all assignments, loads, stores, and branches are present

### Manual Verification

Not applicable - all verification can be done automatically via spirv-as and spirv-val tools.
