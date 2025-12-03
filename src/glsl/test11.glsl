{
    vec4 color;
    
    color = gl_Color;
    color = gl_TexCoord;
    color = gl_Light_Half;
    
    gl_FragColor = color;
}