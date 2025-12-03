{
    // Test: show if FragCoord is working at all
    // If working, top-left pixel should be black (0,0)
    // bottom-right pixel should be yellow (800,600 scaled to 1,1)
    
    float x = gl_FragCoord[0];
    float y = gl_FragCoord[1];
    
    // Scale down by a large factor to see if values are huge
    float r = x / 8000.0;  // if x is ~8000, this will be ~1.0
    float g = y / 6000.0;  // if y is ~6000, this will be ~1.0
    
    // Clamp to 0-1
    if (r > 1.0) { r = 1.0; }
    if (g > 1.0) { g = 1.0; }
    
    vec4 color = vec4(r, g, 0.5, 1.0);
    gl_FragColor = color;
}
