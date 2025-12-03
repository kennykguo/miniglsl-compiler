Yes, shaders are "programs" that run on the GPU for each pixel/vertex. Fragment shaders determine the color of each pixel on a 3D object's surface. Think of it like a function that runs millions of times in parallel.
Vulkan Pipeline Overview


3D Mesh (teapot.obj) → Vertex Shader → Rasterizer → Fragment Shader (YOUR SPIR-V!) → Pixels
                         (positions)                  (YOUR CODE runs per-pixel)


flat.spv: Just passes color through
lambert.spv: Calculates diffuse lighting (dot product)
phong.spv: Adds specular highlights (shiny reflections)
rings.spv: Makes concentric circles pattern
grid.spv: Makes a checkerboard pattern