{
    vec4 temp;
    float fx = gl_FragCoord[0] / 800.0;
    float fy = gl_FragCoord[1] / 600.0;

    if (true) {
        temp[0] = gl_Color[0] * fx;        // red scales with x
        temp[1] = gl_Color[1] * fy;        // green scales with y
        temp[2] = gl_Color[2];             // blue unchanged
        temp[3] = gl_Color[3] * fx * fy;   // alpha varies over screen
    } else {
        temp = gl_Color;
    }

    gl_FragColor = temp;
}
