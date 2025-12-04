#!/bin/bash
# Detailed SPIR-V comparison for ONE shader at a time
cd /home/kennykguo/ece467/tests

SHADER="$1"
if [ -z "$SHADER" ]; then
    echo "Usage: $0 <shader_name>"
    exit 1
fi

echo "=== Compiling $SHADER with YOUR compiler ==="
../src/compiler467 < shaders/${SHADER}.frag > /tmp/your_${SHADER}.spvasm 2>/dev/null
if [ $? -ne 0 ]; then
    echo "ERROR: Your compiler failed"
    exit 1
fi

echo "=== Compiling $SHADER with REFERENCE compiler ==="
cat > /tmp/${SHADER}_wrap.frag << 'WRAPPER'
#version 450
layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 inTexCoord;
layout(location = 0) out vec4 fragColor;
void main()
WRAPPER
sed 's/gl_Color/inColor/g; s/gl_FragColor/fragColor/g; s/gl_TexCoord/inTexCoord/g' \
    shaders/${SHADER}.frag >> /tmp/${SHADER}_wrap.frag

glslangValidator -V /tmp/${SHADER}_wrap.frag -o /tmp/ref_${SHADER}.spv --quiet 2>/dev/null
if [ $? -ne 0 ]; then
    echo "ERROR: Reference compiler failed"
    exit 1
fi

spirv-dis /tmp/ref_${SHADER}.spv > /tmp/ref_${SHADER}.spvasm 2>/dev/null

echo ""
echo "=== YOUR COMPILER OUTPUT (relevant lines) ==="
echo ""
grep -E "(OpCompositeConstruct|OpCompositeExtract|OpLoad.*%[0-9]+$|OpStore|OpFDiv|OpFAdd|OpFSub|OpFMul)" /tmp/your_${SHADER}.spvasm | tail -20

echo ""
echo "=== REFERENCE COMPILER OUTPUT (relevant lines) ==="
echo ""
grep -E "(OpCompositeConstruct|OpCompositeExtract|OpLoad.*%[0-9]+$|OpStore|OpFDiv|OpFAdd|OpFSub|OpFMul)" /tmp/ref_${SHADER}.spvasm | tail -20

echo ""
echo "=== FULL DIFF ==="
diff -u /tmp/ref_${SHADER}.spvasm /tmp/your_${SHADER}.spvasm | head -200
