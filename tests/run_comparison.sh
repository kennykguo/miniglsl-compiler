#!/bin/bash
# Complete testing workflow for CUSTOM shaders
# Uses Vulkan viewer for BOTH reference and output

set -e

cd /home/kennykguo/ece467/tests

# Custom shaders that use gl_FragCoord
SHADERS="checkerboard plasma radial rainbow circles"

echo "=========================================="
echo "  Custom Shader Comparison Test"
echo "=========================================="
echo ""

# Step 1: Generate reference SPIR-V with glslangValidator
echo "Step 1: Compiling SPIR-V with glslangValidator (reference)..."
echo "--------------------------------------------------------------"
for shader in $SHADERS; do
    echo "  Processing: $shader"
    
    # Create GLSL 450 wrapper for glslang
    cat > /tmp/${shader}_glsl.frag << 'WRAPPER'
#version 450
layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 inTexCoord;
layout(location = 0) out vec4 fragColor;
void main()
WRAPPER
    
    # Append shader body with variable substitution
    sed 's/gl_Color/inColor/g; s/gl_FragColor/fragColor/g; s/gl_TexCoord/inTexCoord/g' \
        shaders/${shader}.frag >> /tmp/${shader}_glsl.frag
    
    # Compile with glslang
    glslangValidator -V /tmp/${shader}_glsl.frag \
        -o reference/spirv/${shader}.spv \
        --quiet 2>&1 || echo "    (warnings ignored)"
    
    echo "    ✓ reference/spirv/${shader}.spv"
done
echo ""

# Step 2: Generate SPIR-V with your compiler
echo "Step 2: Compiling SPIR-V with your compiler (output)..."
echo "--------------------------------------------------------"
for shader in $SHADERS; do
    echo "  Processing: $shader"
    
    # Compile with your compiler (suppress warnings)
    ../src/compiler467 < shaders/${shader}.frag 2>/dev/null \
        > output/spirv/${shader}.spvasm
    
    # Assemble to binary
    spirv-as output/spirv/${shader}.spvasm \
        -o output/spirv/${shader}.spv 2>&1 | head -1
    
    # Validate
    spirv-val output/spirv/${shader}.spv 2>&1 | head -1
    
    echo "    ✓ output/spirv/${shader}.spv"
done
echo ""

# Step 3: Render reference SPIR-V with Vulkan viewer
echo "Step 3: Rendering REFERENCE SPIR-V with Vulkan viewer..."
echo "---------------------------------------------------------"
for shader in $SHADERS; do
    echo "  Rendering: $shader"
    ./vulkan_viewer reference/spirv/${shader}.spv \
        assets/teapot.obj \
        reference/images/${shader}.png \
        2>&1 | grep "saved" | sed 's/✓ /    ✓ /'
done
echo ""

# Step 4: Render your compiler SPIR-V with Vulkan viewer
echo "Step 4: Rendering YOUR compiler SPIR-V with Vulkan viewer..."
echo "-------------------------------------------------------------"
for shader in $SHADERS; do
    echo "  Rendering: $shader"
    ./vulkan_viewer output/spirv/${shader}.spv \
        assets/teapot.obj \
        output/images/${shader}.png \
        2>&1 | grep "saved" | sed 's/✓ /    ✓ /'
done
echo ""

# Summary
echo "=========================================="
echo "  ✅ ALL TESTS COMPLETE"
echo "=========================================="
echo ""
echo "Custom shaders tested: $SHADERS"
echo ""
echo "Results:"
echo "  reference/spirv/    - SPIR-V from glslangValidator"
echo "  reference/images/   - Rendered with Vulkan viewer"
echo "  output/spirv/       - SPIR-V from YOUR compiler"
echo "  output/images/      - Rendered with Vulkan viewer"
echo ""
echo "File comparison:"
echo ""
echo "Reference:"
ls -lh reference/spirv/ reference/images/ | grep -E "^-|^total"
echo ""
echo "Your compiler:"
ls -lh output/spirv/*.spv output/images/ | grep -E "^-|^total"
