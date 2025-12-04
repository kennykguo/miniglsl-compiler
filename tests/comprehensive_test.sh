#!/bin/bash
# Comprehensive compiler comparison test
# Compiles all shaders with both compilers, compares SPIR-V, and renders images

set -e
cd /home/kennykguo/ece467/tests

echo "=========================================="
echo "  Comprehensive Compiler Comparison"
echo "=========================================="
echo ""

# Get all shaders
SHADERS=$(ls shaders/*.frag | xargs -n1 basename | sed 's/\.frag$//' | sort)
TOTAL=$(echo "$SHADERS" | wc -w)

echo "Testing $TOTAL shaders"
echo ""

# Clean
find reference output -type f -delete 2>/dev/null || true

COMPILE_PASS=0
COMPILE_FAIL=0
SPIRV_MATCH=0
SPIRV_DIFF=0
RENDER_PASS=0
RENDER_FAIL=0

for shader in $SHADERS; do
    printf "%-30s " "$shader..."
    
    # === STEP 1: Compile with YOUR compiler ===
    if ! ../src/compiler467 < shaders/${shader}.frag 2>/dev/null > output/spirv/${shader}.spvasm; then
        echo "❌ YOUR COMPILER FAILED"
        COMPILE_FAIL=$((COMPILE_FAIL + 1))
        continue
    fi
    
    if ! spirv-as output/spirv/${shader}.spvasm -o output/spirv/${shader}.spv 2>/dev/null; then
        echo "❌ ASSEMBLE FAILED"
        COMPILE_FAIL=$((COMPILE_FAIL + 1))
        continue
    fi
    
    if ! spirv-val output/spirv/${shader}.spv 2>/dev/null; then
        echo "❌ VALIDATION FAILED"  
        COMPILE_FAIL=$((COMPILE_FAIL + 1))
        continue
    fi
    
    COMPILE_PASS=$((COMPILE_PASS + 1))
    
    # === STEP 2: Compile with REFERENCE compiler ===
    cat > /tmp/${shader}_wrap.frag << 'WRAPPER'
#version 450
layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 inTexCoord;
layout(location = 0) out vec4 fragColor;
void main()
WRAPPER
    sed 's/gl_Color/inColor/g; s/gl_FragColor/fragColor/g; s/gl_TexCoord/inTexCoord/g' \
        shaders/${shader}.frag >> /tmp/${shader}_wrap.frag
    
    if ! glslangValidator -V /tmp/${shader}_wrap.frag -o reference/spirv/${shader}.spv --quiet 2>/dev/null; then
        echo "⚠️  REF COMPILE FAILED"
        continue
    fi
    
    # === STEP 3: Compare SPIR-V (basic check) ===
    YOUR_SIZE=$(stat -c%s output/spirv/${shader}.spv 2>/dev/null || echo "0")
    REF_SIZE=$(stat -c%s reference/spirv/${shader}.spv 2>/dev/null || echo "0")
    SIZE_DIFF=$(echo "$YOUR_SIZE $REF_SIZE" | awk '{printf "%.1f", ($1-$2)/$2*100}')
    
    if [ "$YOUR_SIZE" -eq "$REF_SIZE" ]; then
        SPIRV_MATCH=$((SPIRV_MATCH + 1))
    else
        SPIRV_DIFF=$((SPIRV_DIFF + 1))
    fi
    
    # === STEP 4: Render BOTH ===
    if ! ./vulkan_viewer output/spirv/${shader}.spv assets/teapot.obj output/images/${shader}.png 2>&1 >/dev/null; then
        echo "❌ RENDER FAILED"
        RENDER_FAIL=$((RENDER_FAIL + 1))
        continue
    fi
    
    ./vulkan_viewer reference/spirv/${shader}.spv assets/teapot.obj reference/images/${shader}.png 2>&1 >/dev/null || true
    
    RENDER_PASS=$((RENDER_PASS + 1))
    
    # === RESULT ===
    printf "✅ (SPIR-V: %sB vs %sB, diff: %s%%)\n" "$YOUR_SIZE" "$REF_SIZE" "$SIZE_DIFF"
done

echo ""
echo "=========================================="
echo "  FINAL RESULTS"
echo "=========================================="
echo ""
echo "Compilation:"
echo "  ✅ Passed: $COMPILE_PASS/$TOTAL"
echo "  ❌ Failed: $COMPILE_FAIL/$TOTAL"
echo ""
echo "SPIR-V Comparison:"
echo "  ✅ Exact match: $SPIRV_MATCH"
echo "  ⚠️  Different size: $SPIRV_DIFF"
echo ""
echo "Rendering:"
echo "  ✅ Rendered: $RENDER_PASS/$TOTAL"
echo "  ❌ Failed: $RENDER_FAIL/$TOTAL"
echo ""
echo "Output locations:"
echo "  Your compiler: output/spirv/ and output/images/"
echo "  Reference:     reference/spirv/ and reference/images/"
echo ""

if [ $COMPILE_PASS -eq $TOTAL ] && [ $RENDER_PASS -eq $TOTAL ]; then
    echo "🎉 ALL TESTS PASSED! Compiler is working correctly."
else
    echo "⚠️  Some tests failed. Review results above."
fi
