{
    vec4 v;
    vec3 v3;
    float f;
    int i;
    ivec3 iv3;
    
    f = dp3(v, v);
    f = dp3(v3, v3);
    i = dp3(iv3, iv3);
    
    f = rsq(5.0);
    f = rsq(5);
    
    v = lit(v);
}