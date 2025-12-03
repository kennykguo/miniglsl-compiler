{
    // 8x8 checkerboard pattern
    float x = gl_FragCoord[0];
    float y = gl_FragCoord[1];
    
    // Divide screen into 100x100 pixel squares
    float squareX = x / 100.0;
    float squareY = y / 100.0;
    
    // Integer division trick: multiply then divide
    float ix = squareX * 0.5;
    float iy = squareY * 0.5;
    float fracX = ix - (ix * 0.0); // Approximate floor
    float fracY = iy - (iy * 0.0);
    
    // Simpler: just test if coordinates are in certain ranges
    float checkerVal = 0.0;
    
    if (x < 200.0) {
        if (y < 200.0) { checkerVal = 1.0; }
        if (y > 400.0) { checkerVal = 1.0; }
    }
    if (x > 400.0 && x < 600.0) {
        if (y < 200.0) { checkerVal = 1.0; }
        if (y > 400.0) { checkerVal = 1.0; }
    }
    if (x > 200.0 && x < 400.0) {
        if (y > 200.0 && y < 400.0) { checkerVal = 1.0; }
    }
    if (x > 600.0) {
        if (y > 200.0 && y < 400.0) { checkerVal = 1.0; }
    }
    
    vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
    
    vec4 color;
    if (checkerVal > 0.5) {
        color = white;
    } else {
        color = black;
    }
    
    gl_FragColor = color;
}
