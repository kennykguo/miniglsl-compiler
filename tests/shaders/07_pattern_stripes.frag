{
    // Stripes pattern (vertical)
    float x = gl_FragCoord[0] / 800.0;
    float stripe = 0.0;
    
    if (x < 0.2) stripe = 1.0;
    if (x > 0.4) {
        if (x < 0.6) stripe = 1.0;
    }
    if (x > 0.8) stripe = 1.0;
    
    gl_FragColor = vec4(stripe, stripe, stripe, 1.0);
}
