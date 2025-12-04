{
    // Threshold test (half and half)
    float x = gl_FragCoord[0] / 800.0;
    
    vec4 color;
    if (x > 0.5) {
        color[0] = 1.0; color[1] = 1.0; color[2] = 1.0;
    } else {
        color[0] = 0.0; color[1] = 0.0; color[2] = 0.0;
    }
    color[3] = 1.0;
    
    gl_FragColor = color;
}
