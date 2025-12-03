# ECE467F Lab 5 - SPIR-V Code Generator

## Build Instructions
```bash
cd src
make clean
make
```

## Run Compiler
```bash
./compiler467 < input.frag > output.spvasm
spirv-as output.spvasm -o output.spv
spirv-val output.spv
```

## Validation Results
All 5 reference shaders successfully compile and validate:
- flat.frag → flat.spv (✓ validates)
- lambert.frag → lambert.spv (✓ validates)  
- phong.frag → phong.spv (✓ validates)
- rings.frag → rings.spv (✓ validates)
- grid.frag → grid.spv (✓ validates)

## Implementation Notes
- SSA form with unique IDs
- Linear control flow (no branches)
- If/else via OpSelect predication
- Two-pass variable allocation
- Constant hoisting to function prologue
- All expression types supported (unary, binary, constructors, functions, indexing)

## Known Limitations
- Complex if/else scopes with multiple assignments generate warnings (handled via OpSelect)
- Rendering harness requires hardware OpenGL (OSMesa SPIR-V support is incomplete)

## Team Contributions
[Add your team member contributions here]
