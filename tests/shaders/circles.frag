{
    // Multiple concentric circles
    float cx = 400.0;
    float cy = 300.0;
    
    float dx = gl_FragCoord[0] - cx;
    float dy = gl_FragCoord[1] - cy;
    float r2 = dx * dx + dy * dy;
    
    // Define circle radii
    float circle1 = 50.0 * 50.0;
    float circle2 = 100.0 * 100.0;
    float circle3 = 150.0 * 150.0;
    float circle4 = 200.0 * 200.0;
    
    vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 green = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);
    vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
    vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
    
    vec4 color = black;
    
    if (r2 < circle1) {
        color = red;
    }
    if (r2 > circle1 && r2 < circle2) {
        color = green;
    }
    if (r2 > circle2 && r2 < circle3) {
        color = blue;
    }
    if (r2 > circle3 && r2 < circle4) {
        color = yellow;
    }
    
    gl_FragColor = color;
}
