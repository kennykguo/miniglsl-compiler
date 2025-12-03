{
    // Rainbow gradient across screen
    float x = gl_FragCoord[0] / 800.0;
    float y = gl_FragCoord[1] / 600.0;
    
    vec4 color;
    color[0] = x;                    // Red increases left to right
    color[1] = y;                    // Green increases bottom to top
    color[2] = 1.0 - x;              // Blue decreases left to right
    color[3] = 1.0;
    
    gl_FragColor = color;
}
