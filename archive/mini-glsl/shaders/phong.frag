{
    // Red base, clearly different from lambert/flat.
    vec4 base = vec4(1.0, 0.2, 0.2, 1.0);

    // Screen-space position relative to approximate center (400,300).
    float dx = gl_FragCoord[0] - 400.0;
    float dy = gl_FragCoord[1] - 300.0;
    float r2 = dx * dx + dy * dy;

    float outer = 160.0 * 160.0;
    float spec = 0.0;

    // Simple radial "specular" factor.
    if (r2 < outer) {
        spec = 1.0 - (r2 / outer);   // 1 at center, 0 at radius ~160
        if (spec < 0.0) {
            spec = 0.0;
        }
    }

    vec4 color = base;
    color[0] = color[0] + spec * 1.0;
    color[1] = color[1] + spec * 0.3;
    color[2] = color[2] + spec * 0.3;

    gl_FragColor = color;
}
