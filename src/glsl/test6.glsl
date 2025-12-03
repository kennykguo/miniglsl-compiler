{
    vec2 v2;
    vec3 v3;
    vec4 v4;
    float scalar;
    
    v2 = vec2(1.0, 2.0);
    v3 = vec3(1.0, 2.0, 3.0);
    v4 = vec4(1.0, 2.0, 3.0, 4.0);
    
    v3 = v3 + v3;
    v3 = v3 - v3;
    v3 = v3 * v3;
    v3 = 2.0 * v3;
    v3 = v3 * 2.0;
    
    scalar = v4[0];
    scalar = v4[3];
}