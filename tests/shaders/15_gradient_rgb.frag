{
    //  Complex color mix
    float x = gl_FragCoord[0] / 800.0;
    float y = gl_FragCoord[1] / 600.0;
    
    float r = x;
    float g = y;
    float b = 1.0 - x;
    
    gl_FragColor = vec4(r, g, b, 1.0);
}
