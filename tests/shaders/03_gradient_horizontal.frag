{
    // Horizontal gradient (left=black, right=white)
    float x = gl_FragCoord[0] / 800.0;
    gl_FragColor = vec4(x, x, x, 1.0);
}
