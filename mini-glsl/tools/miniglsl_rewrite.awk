#!/usr/bin/awk -f
#
# Rewrite MiniGLSL fragment *body* so that:
#   gl_FragColor         -> mglsl_FragColor
#   gl_Color             -> mglsl_Color
#   gl_TexCoord          -> mglsl_TexCoord
#   gl_Light_Half        -> mglsl_Light_Half
#   gl_Light_Ambient     -> mglsl_Light_Ambient
#   gl_Material_Shininess-> mglsl_Material_Shininess
#
# Everything else (dp3, rsq, lit, env1/env2/env3, gl_FragCoord, etc.)
# is left as-is and handled by the header + GLSL builtins.
#
# This version avoids gawk-specific \y word boundaries, so it works
# with mawk, busybox awk, etc.

{
    gsub(/gl_FragColor/,          "mglsl_FragColor");
    gsub(/gl_Color/,              "mglsl_Color");
    gsub(/gl_TexCoord/,           "mglsl_TexCoord");
    gsub(/gl_Light_Half/,         "mglsl_Light_Half");
    gsub(/gl_Light_Ambient/,      "mglsl_Light_Ambient");
    gsub(/gl_Material_Shininess/, "mglsl_Material_Shininess");

    print;
}