{
    // Quadrants (different color per quadrant)
    float x = gl_FragCoord[0] / 800.0;
    float y = gl_FragCoord[1] / 600.0;
    
    vec4 color;
    color[3] = 1.0;
    
    if (x < 0.5) {
        if (y < 0.5) {
            color[0] = 1.0; color[1] = 0.0; color[2] = 0.0; // Red
        } else {
            color[0] = 0.0; color[1] = 1.0; color[2] = 0.0; // Green
        }
    } else {
        if (y < 0.5) {
            color[0] = 0.0; color[1] = 0.0; color[2] = 1.0; // Blue
        } else {
            color[0] = 1.0; color[1] = 1.0; color[2] = 0.0; // Yellow
        }
    }
    
    gl_FragColor = color;
}
