#!/bin/bash
# Comprehensive test of ALL shaders
set -e
cd /home/kennykguo/ece467/tests

echo "=========================================="
echo "  Testing ALL Shaders"
echo "=========================================="
echo ""

# Get all shader files
SHADERS=$(ls shaders/*.frag | xargs -n1 basename | sed 's/\.frag$//')
TOTAL=$(echo "$SHADERS" | wc -w)

echo "Found $TOTAL shaders to test"
echo ""

# Clean directories
find reference output -type f -delete 2>/dev/null || true

PASSED=0
FAILED=0
FAILED_LIST=""

for shader in $SHADERS; do
    echo -n "Testing $shader... "
    
    # Compile with your compiler
    if ! ../src/compiler467 < shaders/${shader}.frag 2>/dev/null > output/spirv/${shader}.spvasm; then
        echo "❌ COMPILE FAILED"
        FAILED=$((FAILED + 1))
        FAILED_LIST="$FAILED_LIST\n  - $shader (compile error)"
        continue
    fi
    
    # Assemble
    if ! spirv-as output/spirv/${shader}.spvasm -o output/spirv/${shader}.spv 2>/dev/null; then
        echo "❌ ASSEMBLE FAILED"
        FAILED=$((FAILED + 1))
        FAILED_LIST="$FAILED_LIST\n  - $shader (assemble error)"
        continue
    fi
    
    # Validate
    if ! spirv-val output/spirv/${shader}.spv 2>/dev/null; then
        echo "❌ VALIDATION FAILED"
        FAILED=$((FAILED + 1))
        FAILED_LIST="$FAILED_LIST\n  - $shader (validation error)"
        continue
    fi
    
    # Render
    if ! ./vulkan_viewer output/spirv/${shader}.spv assets/teapot.obj output/images/${shader}.png 2>&1 >/dev/null; then
        echo "❌ RENDER FAILED"
        FAILED=$((FAILED + 1))
        FAILED_LIST="$FAILED_LIST\n  - $shader (render error)"
        continue
    fi
    
    echo "✅ PASSED"
    PASSED=$((PASSED + 1))
done

echo ""
echo "=========================================="
echo "  Results: $PASSED/$TOTAL passed"
echo "=========================================="

if [ $FAILED -gt 0 ]; then
    echo ""
    echo "Failed shaders:"
    echo -e "$FAILED_LIST"
fi

echo ""
echo "Successful renders in: output/images/"
ls -lh output/images/*.png 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
