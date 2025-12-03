# Lab 5 - Testing Guide

## File Structure (Updated)

```
ece467/
├── src/                  # Compiler source code
│   ├── compiler467       # Built executable
│   ├── codegen.c/.h      # SPIR-V code generation
│   ├── parser.y          # Parser (modified for C99-style declarations)
│   ├── scanner.l         # Scanner
│   ├── semantic.c/.h     # Semantic analysis
│   ├── Makefile         # Build system
│   └── WALKTHROUGH.md   # Documentation
│
└── tests/               # Test files
    ├── shaders/         # Reference MiniGLSL shaders
    │   ├── flat.frag
    │   ├── lambert.frag
    │   ├── phong.frag
    │   ├── rings.frag
    │   └── grid.frag
    └── spirv/          # Reference SPIR-V assembly
        ├── flat.spvasm
        ├── lambert.spvasm
        ├── phong.spvasm
        ├── rings.spvasm
        └── grid.spvasm
```

## Quick Test

```bash
cd src

# Test all 5 shaders
for shader in flat lambert phong rings grid; do
  ./compiler467 ../tests/shaders/$shader.frag -O $shader.spvasm
  spirv-as $shader.spvasm -o $shader.spv
  spirv-val $shader.spv && echo "✓ $shader"
done
```

## Key Changes

1. **Parser Modified**: Now accepts declarations mixed with statements (C99/GLSL style)
   - Required by Lab Section 5.1: "ensure your MiniGLSL parser accepts it"
   
2. **OpSelect Predication**: All conditionals use OpSelect (linear SSA, no branches)

3. **All Features**: Indexed assignment, type tracking, constant pooling

## Status

✅ All 5 shaders parse successfully
✅ flat.frag and lambert.frag validate
⚠️ phong/rings/grid need debug (parser works, codegen may have issues)
