{
    // Cross pattern (plus sign)
    float x = gl_FragCoord[0] / 800.0 - 0.5;
    float y = gl_FragCoord[1] / 600.0 - 0.5;
    
    vec4 color;
    if (x > -0.05) {
        if (x < 0.05) {
            color[0] = 1.0; color[1] = 1.0; color[2] = 1.0;
        } else {
            if (y > -0.05) {
                if (y < 0.05) {
                    color[0] = 1.0; color[1] = 1.0; color[2] = 1.0;
                } else {
                    color[0] = 0.0; color[1] = 0.0; color[2] = 0.0;
                }
            } else {
                color[0] = 0.0; color[1] = 0.0; color[2] = 0.0;
            }
        }
    } else {
        if (y > -0.05) {
            if (y < 0.05) {
                color[0] = 1.0; color[1] = 1.0; color[2] = 1.0;
            } else {
                color[0] = 0.0; color[1] = 0.0; color[2] = 0.0;
            }
        } else {
            color[0] = 0.0; color[1] = 0.0; color[2] = 0.0;
        }
    }
    color[3] = 1.0;
    
    gl_FragColor = color;
}
