#version 450

// ----- Interface matching MiniGLSL builtins -----
//
// In MiniGLSL source, students still write:
//
//   gl_FragColor
//   gl_Color
//   gl_TexCoord
//   gl_Light_Half
//   gl_Light_Ambient
//   gl_Material_Shininess
//   env1, env2, env3
//
// Our AWK rewriter will rename the gl_* identifiers to these mglsl_*
//
// We also give explicit locations to satisfy SPIR-V rules.

layout(location = 0) in  vec4 mglsl_Color;
layout(location = 1) in  vec4 mglsl_TexCoord;

layout(location = 0) out vec4 mglsl_FragColor;

// Uniforms corresponding to MiniGLSL env* and lighting values.
// Names do NOT start with gl_ (for Vulkan).
layout(location = 2) uniform vec4 env1;
layout(location = 3) uniform vec4 env2;
layout(location = 4) uniform vec4 env3;

// "Fixed-function style" uniforms used by phong.frag
layout(location = 5) uniform vec4 mglsl_Light_Half;
layout(location = 6) uniform vec4 mglsl_Light_Ambient;
layout(location = 7) uniform vec4 mglsl_Material_Shininess;

// gl_FragCoord is a true built-in in Vulkan GLSL; we just use it as-is:
//   in vec4 gl_FragCoord;

// ----- MiniGLSL intrinsics implemented in GLSL -----
//
// dp3 : 3-component dot product
// rsq : reciprocal square root
// lit : classic (1, N.L, spec, ?) lighting helper

float dp3(vec3 a, vec3 b)
{
    return dot(a, b);
}

float dp3(vec4 a, vec4 b)
{
    return dot(a.xyz, b.xyz);
}

float rsq(float x)
{
    // For x > 0, inversesqrt(x) is exactly "1 / sqrt(x)"
    return inversesqrt(x);
}

// MiniGLSL / ARB_fragment_program style lit:
//
//   coeff[0] : ignored here
//   coeff[1] : N.L   (diffuse)
//   coeff[2] : N.H   (for specular)
//   coeff[3] : shininess
//
// Returns: vec4(1, max(N.L,0), spec, 1)
//
// with spec = (N.L > 0) ? max(N.H,0)^shininess : 0

vec4 lit(vec4 coeff)
{
    float NdL = max(coeff[1], 0.0);
    float NdH = max(coeff[2], 0.0);
    float shininess = coeff[3];

    float spec = 0.0;
    if (NdL > 0.0 && NdH > 0.0) {
        spec = pow(NdH, shininess);
    }

    return vec4(1.0, NdL, spec, 1.0);
}