{
    // Inverted radial (bright center, dark edges)
    float x = gl_FragCoord[0] / 800.0 - 0.5;
    float y = gl_FragCoord[1] / 600.0 - 0.5;
    float dist = x * x + y * y;
    float val = 1.0 - dist * 2.0;
    
    gl_FragColor = vec4(val, val, val, 1.0);
}
