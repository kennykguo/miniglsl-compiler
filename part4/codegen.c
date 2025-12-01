#include "codegen.h"
#include "ast.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int next_id = 100;

static int gen_id(void) {
    return next_id++;
}

static int get_max_id(void) {
    return next_id;
}


static void emit_prologue(FILE *out) {
    fprintf(out, "; ===== MiniGLSL Fragment Shader Prologue =====\n");
    fprintf(out, "OpCapability Shader\n");
    fprintf(out, "%%ext = OpExtInstImport \"GLSL.std.450\"\n");
    fprintf(out, "OpMemoryModel Logical GLSL450\n");
    fprintf(out, "OpEntryPoint Fragment %%main \"main\"\n");
    fprintf(out, "  %%mglsl_Color %%gl_FragCoord %%mglsl_FragColor %%mglsl_TexCoord\n");
    fprintf(out, "  %%env1 %%env2 %%env3\n");
    fprintf(out, "  %%mglsl_Light_Half %%mglsl_Light_Ambient %%mglsl_Material_Shininess\n");
    fprintf(out, "OpExecutionMode %%main OriginLowerLeft\n\n");

    fprintf(out, "; ---- Types ----\n");
    fprintf(out, "%%t_void = OpTypeVoid\n");
    fprintf(out, "%%t_fn = OpTypeFunction %%t_void\n");
    fprintf(out, "%%t_float = OpTypeFloat 32\n");
    fprintf(out, "%%t_bool = OpTypeBool\n");
    fprintf(out, "%%t_int = OpTypeInt 32 1\n");
    fprintf(out, "%%t_vec2 = OpTypeVector %%t_float 2\n");
    fprintf(out, "%%t_vec3 = OpTypeVector %%t_float 3\n");
    fprintf(out, "%%t_vec4 = OpTypeVector %%t_float 4\n\n");

    fprintf(out, "; ---- Pointer Types ----\n");
    fprintf(out, "%%t_ptr_input_vec4 = OpTypePointer Input %%t_vec4\n");
    fprintf(out, "%%t_ptr_uniform_vec4 = OpTypePointer UniformConstant %%t_vec4\n");
    fprintf(out, "%%t_ptr_output_vec4 = OpTypePointer Output %%t_vec4\n");
    fprintf(out, "%%t_ptr_func_float = OpTypePointer Function %%t_float\n");
    fprintf(out, "%%t_ptr_func_vec2 = OpTypePointer Function %%t_vec2\n");
    fprintf(out, "%%t_ptr_func_vec3 = OpTypePointer Function %%t_vec3\n");
    fprintf(out, "%%t_ptr_func_vec4 = OpTypePointer Function %%t_vec4\n\n");

    fprintf(out, "; ---- Interface Variables ----\n");
    fprintf(out, "%%mglsl_Color = OpVariable %%t_ptr_input_vec4 Input\n");
    fprintf(out, "%%gl_FragCoord = OpVariable %%t_ptr_input_vec4 Input\n");
    fprintf(out, "OpDecorate %%gl_FragCoord BuiltIn FragCoord\n");
    fprintf(out, "%%mglsl_TexCoord = OpVariable %%t_ptr_input_vec4 Input\n");
    fprintf(out, "%%mglsl_FragColor = OpVariable %%t_ptr_output_vec4 Output\n\n");

    fprintf(out, "OpDecorate %%mglsl_Color Location 0\n");
    fprintf(out, "OpDecorate %%mglsl_FragColor Location 0\n");
    fprintf(out, "OpDecorate %%mglsl_TexCoord Location 1\n");
    fprintf(out, "OpDecorate %%env1 Location 2\n");
    fprintf(out, "OpDecorate %%env2 Location 3\n");
    fprintf(out, "OpDecorate %%env3 Location 4\n");
    fprintf(out, "OpDecorate %%mglsl_Light_Half Location 5\n");
    fprintf(out, "OpDecorate %%mglsl_Light_Ambient Location 6\n");
    fprintf(out, "OpDecorate %%mglsl_Material_Shininess Location 7\n\n");

    fprintf(out, "; ---- Uniforms ----\n");
    fprintf(out, "%%env1 = OpVariable %%t_ptr_uniform_vec4 UniformConstant\n");
    fprintf(out, "%%env2 = OpVariable %%t_ptr_uniform_vec4 UniformConstant\n");
    fprintf(out, "%%env3 = OpVariable %%t_ptr_uniform_vec4 UniformConstant\n");
    fprintf(out, "%%mglsl_Light_Half = OpVariable %%t_ptr_uniform_vec4 UniformConstant\n");
    fprintf(out, "%%mglsl_Light_Ambient = OpVariable %%t_ptr_uniform_vec4 UniformConstant\n");
    fprintf(out, "%%mglsl_Material_Shininess = OpVariable %%t_ptr_uniform_vec4 UniformConstant\n\n");

    fprintf(out, "; ---- Shared Constants ----\n");
    fprintf(out, "%%c_float_0 = OpConstant %%t_float 0.0\n");
    fprintf(out, "%%c_float_1 = OpConstant %%t_float 1.0\n\n");
}


static const char* get_spirv_type(int type_code, int vec_size) {
    if (vec_size == 0) {
        switch (type_code) {
            case FLOAT_T: return "%t_float";
            case INT_T:   return "%t_int";
            case BOOL_T:  return "%t_bool";
            default:      return "%t_float";
        }
    } else {
        switch (vec_size) {
            case 1: return "%t_vec2";
            case 2: return "%t_vec3";
            case 3: return "%t_vec4";
            default: return "%t_vec4";
        }
    }
}

static const char* get_spirv_ptr_type(int type_code, int vec_size) {
    if (vec_size == 0) {
        return "%t_ptr_func_float";
    } else {
        switch (vec_size) {
            case 1: return "%t_ptr_func_vec2";
            case 2: return "%t_ptr_func_vec3";
            case 3: return "%t_ptr_func_vec4";
            default: return "%t_ptr_func_vec4";
        }
    }
}


static int gen_expression(node *expr, FILE *out);
static void gen_statement(node *stmt, FILE *out);


void generate_code(node *ast, FILE *output) {
    fprintf(output, "; SPIR-V\n");
    fprintf(output, "; Version: 1.0\n");
    fprintf(output, "; Generator: MiniGLSL Compiler\n");
    fprintf(output, "; Bound: PLACEHOLDER\n");  // We'll fix this later
    fprintf(output, "; Schema: 0\n");
    
    emit_prologue(output);
    
    // TODO: Generate main function body
    fprintf(output, "; ===== Generated Code =====\n");
    fprintf(output, "%%main = OpFunction %%t_void None %%t_fn\n");
    fprintf(output, "%%entry = OpLabel\n");
    
    // TODO: Process AST here
    
    fprintf(output, "OpReturn\n");
    fprintf(output, "OpFunctionEnd\n");
}