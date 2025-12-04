{
    vec4 base   = gl_Color;
    vec4 bright = base;
    vec4 dark   = vec4(base[0] * 0.2,
                       base[1] * 0.2,
                       base[2] * 0.2,
                       base[3]);

    float x = gl_FragCoord[0];
    float y = gl_FragCoord[1];

    // Coarse grid: a few vertical + horizontal bands.
    float band = 0.0;

    // Vertical lines near x = 200, 400, 600
    if (x > 196.0 && x < 204.0) {
        band = 1.0;
    }
    if (x > 396.0 && x < 404.0) {
        band = 1.0;
    }
    if (x > 596.0 && x < 604.0) {
        band = 1.0;
    }

    // Horizontal lines near y = 150, 300, 450
    if (y > 146.0 && y < 154.0) {
        band = 1.0;
    }
    if (y > 296.0 && y < 304.0) {
        band = 1.0;
    }
    if (y > 446.0 && y < 454.0) {
        band = 1.0;
    }

    vec4 outc;
    if (band > 0.5) {
        outc = bright;
    } else {
        outc = dark;
    }

    gl_FragColor = outc;
}