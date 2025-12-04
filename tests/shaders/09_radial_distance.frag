{
    // Radial distance from center
    float x = gl_FragCoord[0] / 800.0 - 0.5;
    float y = gl_FragCoord[1] / 600.0 - 0.5;
    float dist = x * x + y * y;
    
    gl_FragColor = vec4(dist, dist, dist, 1.0);
}
