{
    // Diagonal gradient
    float x = gl_FragCoord[0] / 800.0;
    float y = gl_FragCoord[1] / 600.0;
    float diag = (x + y) / 2.0;
    gl_FragColor = vec4(diag, diag, diag, 1.0);
}
