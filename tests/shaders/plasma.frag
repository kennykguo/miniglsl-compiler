{
    // Fixed plasma effect - keep values in 0-1 range
    float x = gl_FragCoord[0] / 800.0;
    float y = gl_FragCoord[1] / 600.0;
    
    // Create wave patterns (0 to 1)
    float wave1 = x;
    float wave2 = y;
    float wave3 = (x + y) * 0.5;
    
    // Combine with modulation to keep in range
    float r = wave1 * 0.7 + wave3 * 0.3;
    float g = wave2 * 0.7 + wave1 * 0.3;
    float b = wave3 * 0.7 + wave2 * 0.3;
    
    vec4 color = vec4(r, g, b, 1.0);
    gl_FragColor = color;
}
