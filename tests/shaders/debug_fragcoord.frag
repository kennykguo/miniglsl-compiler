{
    // Show raw FragCoord values as colors
    float x = gl_FragCoord[0] / 800.0;
    float y = gl_FragCoord[1] / 600.0;
    
    vec4 color;
    color[0] = x;  // Red: should be 0 at left, 1 at right
    color[1] = y;  // Green: should be 0 at bottom, 1 at top
    color[2] = 0.0;
    color[3] = 1.0;
    
    gl_FragColor = color;
}
