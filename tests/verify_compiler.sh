#!/bin/bash
# Comprehensive verification of compiler after OriginUpperLeft change
# Compares reference SPIR-V (glslang) vs compiler-generated SPIR-V

set -e  # Exit on error

echo "========================================="
echo "Compiler Verification Test"
echo "========================================="
echo ""

# Test shaders (the 5 main ones from the assignment)
SHADERS="flat lambert phong rings grid"

echo "Step 1: Generating reference SPIR-V with glslangValidator..."
echo "-------------------------------------------------------------"
for shader in $SHADERS; do
    echo "  - Compiling shaders/${shader}.frag (reference)"
    
    # Need to wrap MiniGLSL in GLSL 450 wrapper for glslang
    cat > /tmp/${shader}_wrapper.frag << EOF
#version 450
layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 inTexCoord;
layout(location = 0) out vec4 fragColor;

void main()
EOF
    cat shaders/${shader}.frag | sed 's/gl_Color/inColor/g' | sed 's/gl_FragColor/fragColor/g' | sed 's/gl_TexCoord/inTexCoord/g' >> /tmp/${shader}_wrapper.frag
    
    glslangValidator -V /tmp/${shader}_wrapper.frag -o output/reference_spirv/${shader}.spv 2>&1 | grep -v "^$" || true
done
echo "✓ Reference SPIR-V generated"
echo ""

echo "Step 2: Generating SPIR-V with your compiler..."
echo "------------------------------------------------"
for shader in $SHADERS; do
    echo "  - Compiling shaders/${shader}.frag (compiler)"
    ../src/compiler467 < shaders/${shader}.frag > output/compiler_spirv/${shader}.spvasm 2>&1
    spirv-as output/compiler_spirv/${shader}.spvasm -o output/compiler_spirv/${shader}.spv 2>&1
    spirv-val output/compiler_spirv/${shader}.spv 2>&1 | head -1
done
echo "✓ Compiler SPIR-V generated"
echo ""

echo "Step 3: Rendering with REFERENCE SPIR-V..."
echo "-------------------------------------------"
for shader in $SHADERS; do
    echo "  - Rendering ${shader} (reference)"
    ./vulkan_viewer output/reference_spirv/${shader}.spv assets/teapot.obj output/reference_images/${shader}.png 2>&1 | grep "✓ saved"
done
echo "✓ Reference images rendered"
echo ""

echo "Step 4: Rendering with COMPILER SPIR-V..."
echo "------------------------------------------"
for shader in $SHADERS; do
    echo "  - Rendering ${shader} (compiler)"
    ./vulkan_viewer output/compiler_spirv/${shader}.spv assets/teapot.obj output/compiler_images/${shader}.png 2>&1 | grep "✓ saved"
done
echo "✓ Compiler images rendered"
echo ""

echo "Step 5: Comparison Summary..."
echo "------------------------------"
echo "Reference SPIR-V:"
ls -lh output/reference_spirv/*.spv

echo ""
echo "Compiler SPIR-V:"
ls -lh output/compiler_spirv/*.spv

echo ""
echo "Reference Images:"
ls -lh output/reference_images/*.png

echo ""
echo "Compiler Images:"
ls -lh output/compiler_images/*.png

echo ""
echo "========================================="
echo "✅ Verification Complete!"
echo "========================================="
echo ""
echo "Compare visually:"
echo "  reference: output/reference_images/"
echo "  compiler:  output/compiler_images/"
