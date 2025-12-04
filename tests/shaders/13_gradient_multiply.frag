{
    // Corner to corner diagonal
    float x = gl_FragCoord[0] / 800.0;
    float y = gl_FragCoord[1] / 600.0;
    float val = x * y;
    
    gl_FragColor = vec4(val, val, val, 1.0);
}
