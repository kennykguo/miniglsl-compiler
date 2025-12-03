{
    vec4 baseOn  = vec4(0.9, 0.3, 0.3, 1.0);  // bright warm band
    vec4 baseOff = vec4(0.1, 0.1, 0.1, 1.0);  // dark background

    float dx = gl_FragCoord[0] - 400.0;
    float dy = gl_FragCoord[1] - 300.0;
    float r2 = dx * dx + dy * dy;

    float inner = 80.0 * 80.0;
    float outer = 120.0 * 120.0;

    float inside_band = 0.0;
    if (r2 > inner && r2 < outer) {
        inside_band = 1.0;
    }

    vec4 color;
    color[0] = baseOff[0] + (baseOn[0] - baseOff[0]) * inside_band;
    color[1] = baseOff[1] + (baseOn[1] - baseOff[1]) * inside_band;
    color[2] = baseOff[2] + (baseOn[2] - baseOff[2]) * inside_band;
    color[3] = baseOff[3] + (baseOn[3] - baseOff[3]) * inside_band;

    gl_FragColor = color;
}
