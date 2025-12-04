#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outTexCoord;

void main() {
    // Match shader.c camera, but fix for Vulkan coordinate system
    vec3 eye = vec3(0.0, 0.0, 6.3);
    vec3 center = vec3(0.0, 0.0, 0.0);
    vec3 up = vec3(0.0, 1.0, 0.0);
    
    // View matrix (lookat)
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    
    mat4 view = mat4(
        vec4(s.x, u.x, -f.x, 0.0),
        vec4(s.y, u.y, -f.y, 0.0),
        vec4(s.z, u.z, -f.z, 0.0),
        vec4(-dot(s, eye), -dot(u, eye), dot(f, eye), 1.0)
    );
    
    // Perspective matrix (Vulkan-style: Y inverted, depth [0,1])
    float fovy = radians(60.0);
    float aspect = 800.0 / 600.0;
    float zNear = 0.05;
    float zFar = 1000.0;
    float f_val = 1.0 / tan(fovy * 0.5);
    
    mat4 proj = mat4(
        vec4(f_val / aspect, 0.0, 0.0, 0.0),
        vec4(0.0, f_val, 0.0, 0.0),  // POSITIVE for OriginLowerLeft
        vec4(0.0, 0.0, zFar / (zNear - zFar), -1.0),  // Vulkan depth [0,1]
        vec4(0.0, 0.0, (zFar * zNear) / (zNear - zFar), 0.0)
    );
    
    vec4 viewPos = view * vec4(inPosition, 1.0);
    gl_Position = proj * viewPos;
    
    outColor = inColor;
    outTexCoord = vec4(0.0, 0.0, 0.0, 1.0);
}
