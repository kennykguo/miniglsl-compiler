#!/bin/bash
# Quick comparison test - no interactive prompts
set -e
cd /home/kennykguo/ece467/tests

SHADERS="checkerboard plasma radial rainbow circles"

# Clean
find reference output -type f -delete 2>/dev/null || true

# Reference SPIR-V
for s in $SHADERS; do
    cat > /tmp/${s}_w.frag << 'W'
#version 450
layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 inTexCoord;
layout(location = 0) out vec4 fragColor;
void main()
W
    sed 's/gl_Color/inColor/g; s/gl_FragColor/fragColor/g; s/gl_TexCoord/inTexCoord/g' shaders/${s}.frag >> /tmp/${s}_w.frag
    glslangValidator -V /tmp/${s}_w.frag -o reference/spirv/${s}.spv --quiet 2>&1 >/dev/null || true
done

# Compiler SPIR-V
for s in $SHADERS; do
    ../src/compiler467 < shaders/${s}.frag 2>/dev/null > output/spirv/${s}.spvasm
    spirv-as output/spirv/${s}.spvasm -o output/spirv/${s}.spv 2>&1 >/dev/null
done

# Render reference
for s in $SHADERS; do
    ./vulkan_viewer reference/spirv/${s}.spv assets/teapot.obj reference/images/${s}.png 2>&1 >/dev/null
done

# Render compiler
for s in $SHADERS; do
    ./vulkan_viewer output/spirv/${s}.spv assets/teapot.obj output/images/${s}.png 2>&1 >/dev/null
done

echo "✅ Complete! Compare:"
echo "  reference/images/"
echo "  output/images/"
ls -lh reference/images/ output/images/
