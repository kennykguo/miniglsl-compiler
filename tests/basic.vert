#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outTexCoord;

void main() {
    // simple orthographic-like projection
    vec3 pos = inPosition * 0.2; // scale down
    gl_Position = vec4(pos.x, pos.y, -pos.z * 0.5 + 0.5, 1.0);
    
    // pass to fragment shader
    outColor = inColor;
    outTexCoord = vec4(0.0, 0.0, 0.0, 1.0);
}
