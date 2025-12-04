{
    // Color bands (horizontal)
    float y = gl_FragCoord[1] / 600.0;
    
    vec4 color;
    if (y < 0.25) {
        color[0] = 1.0; color[1] = 0.0; color[2] = 0.0; // Red
    } else {
        if (y < 0.5) {
            color[0] = 0.0; color[1] = 1.0; color[2] = 0.0; // Green
        } else {
            if (y < 0.75) {
                color[0] = 0.0; color[1] = 0.0; color[2] = 1.0; // Blue
            } else {
                color[0] = 1.0; color[1] = 1.0; color[2] = 0.0; // Yellow
            }
        }
    }
    color[3] = 1.0;
    
    gl_FragColor = color;
}
