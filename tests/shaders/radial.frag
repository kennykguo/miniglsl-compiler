{
    // Radial rainbow emanating from center
    float cx = 400.0;
    float cy = 300.0;
    
    float dx = gl_FragCoord[0] - cx;
    float dy = gl_FragCoord[1] - cy;
    
    // Distance from center
    float dist = dx * dx + dy * dy;
    dist = dist / 10000.0; // Normalize
    
    // Create color based on distance
    vec4 color;
    color[0] = dist;
    color[1] = 1.0 - dist;
    color[2] = dist * 0.5;
    color[3] = 1.0;
    
    gl_FragColor = color;
}
