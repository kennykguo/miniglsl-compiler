// shader.c
// Headless OSMesa renderer for:
//   - FF   : fixed-function reference
//   - GLSL : MiniGLSL fragment body wrapped into GLSL 1.20
//   - SPIR-V : fragment shader from SPIR-V binary, with the same interface
//
// Usage (typical):
//   ./shader --mode ff    --obj assets/teapot.obj --png ff.png
//   ./shader --mode glsl  --frag shaders/flat.frag   --obj assets/teapot.obj --png flat.png
//   ./shader --mode spirv --frag spirv/flat.spv      --obj assets/teapot.obj --png spirv_flat.png

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif

#include <GL/osmesa.h>
#include <GL/gl.h>
#include <png.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ------------------------------------------------------------
// Small math helpers
// ------------------------------------------------------------
typedef struct { float x,y,z; } Vec3;
typedef struct { int a,b,c; } Tri;

static Vec3 v3(float x,float y,float z){ Vec3 v={x,y,z}; return v; }
static Vec3 v3add(Vec3 a,Vec3 b){ return v3(a.x+b.x,a.y+b.y,a.z+b.z); }
static Vec3 v3sub(Vec3 a,Vec3 b){ return v3(a.x-b.x,a.y-b.y,a.z-b.z); }
static Vec3 v3scale(Vec3 a,float s){ return v3(a.x*s,a.y*s,a.z*s); }
static float v3dot(Vec3 a,Vec3 b){ return a.x*b.x+a.y*b.y+a.z*b.z; }
static Vec3 v3cross(Vec3 a,Vec3 b){
    return v3(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}
static float v3len(Vec3 a){ return sqrtf(v3dot(a,a)); }
static Vec3 v3norm(Vec3 a){
    float L=v3len(a);
    return (L>1e-8f)?v3scale(a,1.0f/L):v3(0,0,0);
}

static void mat_identity(float M[16]){
    memset(M,0,16*sizeof(float));
    M[0]=M[5]=M[10]=M[15]=1.0f;
}

static void mat_mul(float C[16], const float A[16], const float B[16]){
    float r[16];
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            r[i*4+j]=0.f;
            for(int k=0;k<4;k++){
                r[i*4+j]+=A[i*4+k]*B[k*4+j];
            }
        }
    }
    memcpy(C,r,sizeof r);
}

static void mat_perspective(float M[16], float fovy_deg, float aspect,
                            float zn, float zf){
    float f = 1.0f / tanf((float)M_PI * fovy_deg / 180.0f * 0.5f);
    mat_identity(M);
    M[0]=f/aspect;
    M[5]=f;
    M[10]=(zf+zn)/(zn-zf);
    M[11]=-1.0f;
    M[14]=(2*zf*zn)/(zn-zf);
    M[15]=0.0f;
}

static void mat_lookat(float M[16], Vec3 eye, Vec3 cen, Vec3 up){
    Vec3 f=v3norm(v3sub(cen,eye));
    Vec3 s=v3norm(v3cross(f,up));
    Vec3 u=v3cross(s,f);
    float R[16]={
        s.x, u.x,-f.x, 0,
        s.y, u.y,-f.y, 0,
        s.z, u.z,-f.z, 0,
       -v3dot(s,eye), -v3dot(u,eye), v3dot(f,eye), 1
    };
    memcpy(M,R,sizeof R);
}

// ------------------------------------------------------------
// Minimal OBJ loader (triangles only)
// ------------------------------------------------------------
typedef struct {
    Vec3* v; size_t nv;
    Tri*  t; size_t nt;
} Mesh;

static Mesh mesh_load(const char* path){
    Mesh m={0};
    FILE* f=fopen(path,"r");
    if(!f){
        fprintf(stderr,"cannot open %s: %s\n", path, strerror(errno));
        exit(1);
    }
    char line[512];
    // pass 1: counts
    while(fgets(line,sizeof line,f)){
        if(!strncmp(line,"v ",2)) m.nv++;
        else if(!strncmp(line,"f ",2)) m.nt++;
    }
    rewind(f);
    m.v = (Vec3*)malloc(m.nv*sizeof(Vec3));
    m.t = (Tri*)malloc(m.nt*sizeof(Tri));
    if(!m.v || !m.t){
        fprintf(stderr,"OOM mesh\n");
        exit(1);
    }
    size_t iv=0, it=0;
    while(fgets(line,sizeof line,f)){
        if(!strncmp(line,"v ",2)){
            Vec3 vtx;
            sscanf(line,"v %f %f %f",&vtx.x,&vtx.y,&vtx.z);
            m.v[iv++] = vtx;
        } else if(!strncmp(line,"f ",2)){
            int a,b,c;
            if(sscanf(line,"f %d %d %d",&a,&b,&c)==3){
                m.t[it++] = (Tri){a-1,b-1,c-1};
            }else{
                int ai,bi,ci,ni,nj,nk;
                if(sscanf(line,"f %d//%d %d//%d %d//%d",
                          &ai,&ni,&bi,&nj,&ci,&nk)==6){
                    m.t[it++] = (Tri){ai-1,bi-1,ci-1};
                }else{
                    int av,bv,cv, au,buu,cuu, an,bn,cn;
                    if(sscanf(line,"f %d/%d/%d %d/%d/%d %d/%d/%d",
                              &av,&au,&an,&bv,&buu,&bn,
                              &cv,&cuu,&cn)==9){
                        m.t[it++] = (Tri){av-1,bv-1,cv-1};
                    }
                }
            }
        }
    }
    fclose(f);
    return m;
}

static void mesh_free(Mesh* m){
    free(m->v);
    free(m->t);
    memset(m,0,sizeof *m);
}

// ------------------------------------------------------------
// PNG writer (RGBA8, flipped Y)
// ------------------------------------------------------------
static void write_png_rgba8(const char* path, int W,int H,
                            const unsigned char* rgba){
    FILE* fp=fopen(path,"wb");
    if(!fp){
        fprintf(stderr,"png open %s: %s\n", path, strerror(errno));
        exit(1);
    }
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                              NULL,NULL,NULL);
    png_infop info  = png_create_info_struct(png);
    if(!png || !info || setjmp(png_jmpbuf(png))){
        fprintf(stderr,"png error\n");
        exit(1);
    }
    png_init_io(png,fp);
    png_set_IHDR(png,info,(png_uint_32)W,(png_uint_32)H,8,
                 PNG_COLOR_TYPE_RGBA,PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
    png_write_info(png,info);
    png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep)*(size_t)H);
    for(int y=0;y<H;y++){
        rows[y]=(png_bytep)(rgba + (size_t)(H-1-y)*W*4);
    }
    png_write_image(png, rows);
    png_write_end(png,NULL);
    free(rows);
    png_destroy_write_struct(&png,&info);
    fclose(fp);
}

// ------------------------------------------------------------
// GLSL helpers (MiniGLSL ? wrapped GLSL 1.20)
// ------------------------------------------------------------
static char* slurp_text(const char* path){
    FILE* f=fopen(path,"rb");
    if(!f){
        fprintf(stderr,"open %s failed: %s\n", path, strerror(errno));
        exit(1);
    }
    fseek(f,0,SEEK_END); long n=ftell(f); fseek(f,0,SEEK_SET);
    char* s=(char*)malloc((size_t)n+1);
    if(!s){ fclose(f); fprintf(stderr,"OOM slurp\n"); exit(1); }
    if(fread(s,1,(size_t)n,f)!=(size_t)n){
        fclose(f);
        fprintf(stderr,"read %s failed\n", path);
        exit(1);
    }
    s[n]=0;
    fclose(f);
    return s;
}

static GLuint compile_shader(GLenum type, const char* src, const char* label){
    GLuint sh=glCreateShader(type);
    glShaderSource(sh,1,&src,NULL);
    glCompileShader(sh);
    GLint ok=0;
    glGetShaderiv(sh,GL_COMPILE_STATUS,&ok);
    if(!ok){
        GLint len=0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        char* log=(char*)malloc(len?len:1);
        glGetShaderInfoLog(sh,len,NULL,log);
        fprintf(stderr,"%s compile error:\n%s\n", label, log);
        free(log);
        exit(1);
    }
    return sh;
}

static GLuint link_program(GLuint vs, GLuint fs){
    GLuint p=glCreateProgram();
    glAttachShader(p,vs);
    glAttachShader(p,fs);
    glLinkProgram(p);
    GLint ok=0;
    glGetProgramiv(p,GL_LINK_STATUS,&ok);
    if(!ok){
        GLint len=0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        char* log=(char*)malloc(len?len:1);
        glGetProgramInfoLog(p,len,NULL,log);
        fprintf(stderr,"link error:\n%s\n", log);
        free(log);
        exit(1);
    }
    return p;
}

// Simple GLSL 1.20 vertex shader for all modes.
static const char* kVertexSrc =
"#version 120\n"
"varying vec4 vColor;\n"
"varying vec4 vTexCoord;\n"
"void main(){\n"
"  vColor    = gl_Color;\n"
"  vTexCoord = gl_TexCoord[0];\n"
"  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"}\n";

// Wrap a MiniGLSL fragment *body* into a full GLSL 1.20 fragment shader.
// The body is assumed to be a single scope, e.g.:
//   {
//      vec4 temp = gl_Color;
//      gl_FragColor = temp;
//   }
static char* build_fragment_source_from_miniglsl(const char* body){
    const char* prefix =
        "#version 120\n"
        "varying vec4 vColor;\n"
        "varying vec4 vTexCoord;\n"
        "uniform vec4 env1;\n"
        "uniform vec4 env2;\n"
        "uniform vec4 env3;\n"
        "void main()\n";

    size_t len = strlen(prefix) + strlen(body) + 2;
    char* out = (char*)malloc(len);
    if(!out){
        fprintf(stderr,"OOM building fragment source\n");
        exit(1);
    }
    strcpy(out, prefix);
    strcat(out, body);   // body already contains braces
    return out;
}

// ------------------------------------------------------------
// SPIR-V helpers (fragment-only, mixed with GLSL vertex)
// ------------------------------------------------------------
#ifndef GL_SHADER_BINARY_FORMAT_SPIR_V
#define GL_SHADER_BINARY_FORMAT_SPIR_V 0x9551
#endif

typedef void (APIENTRYP PFNGLSHADERBINARYPROC)(GLsizei, const GLuint*, GLenum,
                                               const void*, GLsizei);
typedef void (APIENTRYP PFNGLSPECIALIZESHADERPROC)(GLuint, const GLchar*,
                                                   GLuint, const GLuint*,
                                                   const GLuint*);

static void* get_gl_proc(const char* name){
    return OSMesaGetProcAddress(name);
}

static GLuint create_spirv_shader(GLenum stage, const char* spv_path){
    FILE* f=fopen(spv_path,"rb");
    if(!f){
        fprintf(stderr,"open SPIR-V %s failed: %s\n", spv_path, strerror(errno));
        exit(1);
    }
    fseek(f,0,SEEK_END); long n = ftell(f); fseek(f,0,SEEK_SET);
    void* buf = malloc((size_t)n);
    if(!buf){
        fclose(f);
        fprintf(stderr,"OOM SPIR-V buffer\n");
        exit(1);
    }
    if(fread(buf,1,(size_t)n,f)!=(size_t)n){
        fclose(f);
        fprintf(stderr,"read SPIR-V %s failed\n", spv_path);
        exit(1);
    }
    fclose(f);

    PFNGLSHADERBINARYPROC     pShaderBinary =
        (PFNGLSHADERBINARYPROC)get_gl_proc("glShaderBinary");
    PFNGLSPECIALIZESHADERPROC pSpecialize =
        (PFNGLSPECIALIZESHADERPROC)get_gl_proc("glSpecializeShader");

    if(!pShaderBinary || !pSpecialize){
        fprintf(stderr,
                "SPIR-V unsupported: missing glShaderBinary/glSpecializeShader\n");
        exit(1);
    }

    GLuint sh = glCreateShader(stage);
    pShaderBinary(1, &sh, GL_SHADER_BINARY_FORMAT_SPIR_V, buf, (GLsizei)n);
    free(buf);

    // Entry point "main", no specialization constants
    pSpecialize(sh, "main", 0, NULL, NULL);

    GLint ok=0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if(!ok){
        GLint len=0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        char* log=(char*)malloc(len?len:1);
        glGetShaderInfoLog(sh, len, NULL, log);
        fprintf(stderr,"SPIR-V shader specialization error:\n%s\n", log);
        free(log);
        exit(1);
    }
    return sh;
}

// Build a program with:
//   - GLSL vertex shader (kVertexSrc)
//   - SPIR-V fragment shader from file
static GLuint build_spirv_program(const char* frag_spv_path){
    GLuint vs = compile_shader(GL_VERTEX_SHADER, kVertexSrc, "vertex");
    GLuint fs = create_spirv_shader(GL_FRAGMENT_SHADER, frag_spv_path);
    GLuint prog = link_program(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// ------------------------------------------------------------
// CLI options
// ------------------------------------------------------------
typedef enum { MODE_FF=0, MODE_GLSL=1, MODE_SPIRV=2 } Mode;

typedef struct {
    int   W,H;
    const char* obj_path;
    const char* frag_path;  // MiniGLSL .frag (GLSL) or .spv (SPIR-V)
    const char* png_path;
    Mode mode;
} Opts;

static Opts parse_args(int argc, char** argv){
    Opts o;
    o.W = 800;
    o.H = 600;
    o.obj_path = NULL;
    o.frag_path = NULL;
    o.png_path = "frame.png";
    o.mode = MODE_FF;

    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"--obj") && i+1<argc){
            o.obj_path = argv[++i];
        }else if(!strcmp(argv[i],"--frag") && i+1<argc){
            o.frag_path = argv[++i];
        }else if(!strcmp(argv[i],"--png") && i+1<argc){
            o.png_path = argv[++i];
        }else if(!strcmp(argv[i],"--mode") && i+1<argc){
            const char* m = argv[++i];
            if(!strcmp(m,"ff"))      o.mode = MODE_FF;
            else if(!strcmp(m,"glsl"))   o.mode = MODE_GLSL;
            else if(!strcmp(m,"spirv"))  o.mode = MODE_SPIRV;
            else{
                fprintf(stderr,"Unknown mode '%s' (use ff|glsl|spirv)\n", m);
                exit(1);
            }
        }else if(!strcmp(argv[i],"--size") && i+1<argc){
            sscanf(argv[++i], "%dx%d", &o.W, &o.H);
        }else{
            fprintf(stderr,"Unknown arg: %s\n", argv[i]);
            exit(1);
        }
    }

    if(!o.obj_path){
        fprintf(stderr,
            "Usage: %s --obj mesh.obj [--mode ff|glsl|spirv] [--frag file] [--png out.png] [--size WxH]\n",
            argv[0]);
        exit(1);
    }
    if((o.mode==MODE_GLSL || o.mode==MODE_SPIRV) && !o.frag_path){
        fprintf(stderr,"%s mode requires --frag <file>\n",
                (o.mode==MODE_GLSL?"GLSL":"SPIR-V"));
        exit(1);
    }
    return o;
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(int argc, char** argv){
    Opts opt = parse_args(argc, argv);

    // Load mesh
    Mesh mesh = mesh_load(opt.obj_path);

    // Simple camera: auto-frame the mesh
    Vec3 mn = mesh.v[0], mx = mesh.v[0];
    for(size_t i=1;i<mesh.nv;i++){
        Vec3 p = mesh.v[i];
        if(p.x<mn.x) mn.x=p.x;
        if(p.y<mn.y) mn.y=p.y;
        if(p.z<mn.z) mn.z=p.z;
        if(p.x>mx.x) mx.x=p.x;
        if(p.y>mx.y) mx.y=p.y;
        if(p.z>mx.z) mx.z=p.z;
    }
    Vec3 center = v3scale(v3add(mn,mx),0.5f);
    float diag  = v3len(v3sub(mx,mn));
    Vec3 eye    = v3(center.x, center.y,
                     center.z + (diag>0?diag*1.8f:6.0f));
    Vec3 up     = v3(0,1,0);

    // OSMesa context + framebuffer
    OSMesaContext ctx = OSMesaCreateContextExt(OSMESA_RGBA,24,8,0,NULL);
    if(!ctx){
        fprintf(stderr,"OSMesaCreateContext failed\n");
        return 1;
    }
    size_t bufsize = (size_t)opt.W*(size_t)opt.H*4;
    unsigned char* fb = (unsigned char*)malloc(bufsize);
    if(!fb){
        fprintf(stderr,"OOM fb\n");
        OSMesaDestroyContext(ctx);
        return 1;
    }
    if(!OSMesaMakeCurrent(ctx, fb, GL_UNSIGNED_BYTE, opt.W, opt.H)){
        fprintf(stderr,"OSMesaMakeCurrent failed\n");
        free(fb);
        OSMesaDestroyContext(ctx);
        return 1;
    }

    glViewport(0,0,opt.W,opt.H);
    glDisable(GL_DITHER);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f,0.07f,0.10f,1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Matrices
    float P[16], V[16], MVP[16];
    mat_perspective(P, 60.0f, (float)opt.W/(float)opt.H, 0.05f, 1000.0f);
    mat_lookat(V, eye, center, up);
    mat_mul(MVP, P, V);

    glMatrixMode(GL_PROJECTION); glLoadMatrixf(P);
    glMatrixMode(GL_MODELVIEW);  glLoadMatrixf(V);

    if(opt.mode == MODE_FF){
        // Fixed-function sanity image
        glColor3f(0.2f,0.7f,1.0f);
        glBegin(GL_TRIANGLES);
        for(size_t i=0;i<mesh.nt;i++){
            Tri t = mesh.t[i];
            Vec3 A=mesh.v[t.a], B=mesh.v[t.b], C=mesh.v[t.c];
            glVertex3f(A.x,A.y,A.z);
            glVertex3f(B.x,B.y,B.z);
            glVertex3f(C.x,C.y,C.z);
        }
        glEnd();
    } else if(opt.mode == MODE_GLSL){
        // MiniGLSL ? GLSL 120 fragment
        char* body     = slurp_text(opt.frag_path);
        char* frag_src = build_fragment_source_from_miniglsl(body);
        free(body);

        GLuint vs = compile_shader(GL_VERTEX_SHADER,   kVertexSrc, "vertex");
        GLuint fs = compile_shader(GL_FRAGMENT_SHADER, frag_src,   "fragment");
        free(frag_src);

        GLuint prog = link_program(vs,fs);
        glDeleteShader(vs);
        glDeleteShader(fs);

        glUseProgram(prog);

        // Provide env uniforms (used by some shaders)
        GLint loc_env1 = glGetUniformLocation(prog,"env1");
        GLint loc_env2 = glGetUniformLocation(prog,"env2");
        GLint loc_env3 = glGetUniformLocation(prog,"env3");
        if(loc_env1>=0) glUniform4f(loc_env1, 0.0f,0.0f,1.0f,1.0f);
        if(loc_env2>=0) glUniform4f(loc_env2, 1.0f,0.0f,0.0f,1.0f);
        if(loc_env3>=0) glUniform4f(loc_env3, 0.0f,1.0f,0.0f,1.0f);

        glShadeModel(GL_SMOOTH);
        glColor3f(0.9f,0.8f,0.3f); // base color as gl_Color

        glBegin(GL_TRIANGLES);
        for(size_t i=0;i<mesh.nt;i++){
            Tri t = mesh.t[i];
            Vec3 A=mesh.v[t.a], B=mesh.v[t.b], C=mesh.v[t.c];
            glVertex3f(A.x,A.y,A.z);
            glVertex3f(B.x,B.y,B.z);
            glVertex3f(C.x,C.y,C.z);
        }
        glEnd();

        glUseProgram(0);
        glDeleteProgram(prog);
    } else { // MODE_SPIRV
        // SPIR-V fragment shader, GLSL vertex shader
        GLuint prog = build_spirv_program(opt.frag_path);
        glUseProgram(prog);

        // Same env uniforms as GLSL path
        GLint loc_env1 = glGetUniformLocation(prog,"env1");
        GLint loc_env2 = glGetUniformLocation(prog,"env2");
        GLint loc_env3 = glGetUniformLocation(prog,"env3");
        if(loc_env1>=0) glUniform4f(loc_env1, 0.0f,0.0f,1.0f,1.0f);
        if(loc_env2>=0) glUniform4f(loc_env2, 1.0f,0.0f,0.0f,1.0f);
        if(loc_env3>=0) glUniform4f(loc_env3, 0.0f,1.0f,0.0f,1.0f);

        glShadeModel(GL_SMOOTH);
        glColor3f(0.9f,0.8f,0.3f); // base color as gl_Color

        glBegin(GL_TRIANGLES);
        for(size_t i=0;i<mesh.nt;i++){
            Tri t = mesh.t[i];
            Vec3 A=mesh.v[t.a], B=mesh.v[t.b], C=mesh.v[t.c];
            glVertex3f(A.x,A.y,A.z);
            glVertex3f(B.x,B.y,B.z);
            glVertex3f(C.x,C.y,C.z);
        }
        glEnd();

        glUseProgram(0);
        glDeleteProgram(prog);
    }

    // Read back ? PNG
    unsigned char* rgba = (unsigned char*)malloc(bufsize);
    if(!rgba){
        fprintf(stderr,"OOM rgba\n");
        free(fb);
        OSMesaDestroyContext(ctx);
        mesh_free(&mesh);
        return 1;
    }
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(0,0,opt.W,opt.H,GL_RGBA,GL_UNSIGNED_BYTE,rgba);

    write_png_rgba8(opt.png_path, opt.W,opt.H, rgba);
    printf("Wrote %s (%dx%d) [%s]\n",
           opt.png_path, opt.W,opt.H,
           (opt.mode==MODE_FF?"FF":
            (opt.mode==MODE_GLSL?"GLSL":"SPIR-V")));

    free(rgba);
    free(fb);
    mesh_free(&mesh);
    OSMesaDestroyContext(ctx);
    return 0;
}
