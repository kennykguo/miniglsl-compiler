{
    // Red-green gradient
    float x = gl_FragCoord[0] / 800.0;
    float y = gl_FragCoord[1] / 600.0;
    gl_FragColor = vec4(x, y, 0.0, 1.0);
}
