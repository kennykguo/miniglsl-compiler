{
    // Vertical gradient (bottom=black, top=white)
    float y = gl_FragCoord[1] / 600.0;
    gl_FragColor = vec4(y, y, y, 1.0);
}
