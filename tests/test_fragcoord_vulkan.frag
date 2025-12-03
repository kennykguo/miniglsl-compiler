#version 450

layout(location = 0) in vec4 inColor;
layout(location = 0) out vec4 outColor;

void main() {
    float x = gl_FragCoord.x / 800.0;
    float y = gl_FragCoord.y / 600.0;
    outColor = vec4(x, y, 0.0, 1.0);
}
