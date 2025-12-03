## What's in part5 Folder

**part5** is the **test harness** - it renders your compiled shaders to verify they work:

### Key Components:

1. **`shader.c`** - OpenGL/OSMesa renderer that:
   - Loads a 3D mesh (teapot.obj)
   - Renders it with your SPIR-V shader
   - Outputs PNG images

2. **`shaders/*.frag`** - MiniGLSL source files (5 reference shaders):
   - `flat.frag` - passthrough color
   - `lambert.frag` - diffuse lighting  
   - `phong.frag` - specular highlights
   - `rings.frag` - radial pattern with if/else
   - `grid.frag` - checkerboard with if/else

3. **`spirv/*.spvasm`** - **Reference SPIR-V** (staff-provided, gold standard)
   - Compare your output against these

4. **`Makefile`** - Build system that:
   - Compiles shader.c harness
   - Converts MiniGLSL → GLSL 1.20 → SPIR-V (reference path)
   - Renders images

5. **`assets/teapot.obj`** - 3D mesh for rendering

---
