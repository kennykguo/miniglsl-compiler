/**********************************************************************
 *  Interface to the scanner module for ECE467 course project
 **********************************************************************/

%{
#include "common.h"
#include "parser.tab.h"
#include "string.h"
#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput input
#define yTRACE(x) { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x) { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }
int CheckInt(void);
int CheckIdentifier(void);
int yyline = 1;
%}

%option noyywrap
/* ws = whitespace */
WS [ \t]
DIGIT [0-9]


%%
"/*"        {
              int c1      = 0;
              int c2      = yyinput();
              int curline = yyline;
              for(;;) {
                if (c2 == EOF) {
                  fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: Unmatched /*\n", curline);
                  errorOccurred = TRUE;
                  yyterminate();
                }
                if (c1 == '*' && c2 == '/')
                  break;
                c1 = c2;
                c2 = yyinput();
                if (c1 == '\n' && c2 != EOF)
                  yyline++;
              }
            }


"if" { yTRACE(IF); return IF; }
"else" { yTRACE(ELSE); return ELSE; }
"while" { yTRACE(WHILE); return WHILE; }
"break" { yTRACE(BREAK); return BREAK; }
"return" { yTRACE(RETURN); return RETURN; }
"const" { yTRACE(CONST); return CONST; }
"true" { yTRACE(TRUE_T); return TRUE_T; }
"false" { yTRACE(FALSE_T); return FALSE_T; }
"int" { yTRACE(INTEGER_T); return INTEGER_T; }
"bool" { yTRACE(BOOLEAN_T); return BOOLEAN_T; }
"float" { yTRACE(FLOAT_T); return FLOAT_T; }
"vec2" { yTRACE(VEC2_T); return VEC2_T; }
"vec3" { yTRACE(VEC3_T); return VEC3_T; }
"vec4" { yTRACE(VEC4_T); return VEC4_T; }
"ivec2" { yTRACE(IVEC2_T); return IVEC2_T; }
"ivec3" { yTRACE(IVEC3_T); return IVEC3_T; }
"ivec4" { yTRACE(IVEC4_T); return IVEC4_T; }
"bvec2" { yTRACE(BVEC2_T); return BVEC2_T; }
"bvec3" { yTRACE(BVEC3_T); return BVEC3_T; }
"bvec4" { yTRACE(BVEC4_T); return BVEC4_T; }
"mat2" { yTRACE(MAT2_T); return MAT2_T; }
"mat3" { yTRACE(MAT3_T); return MAT3_T; }
"mat4" { yTRACE(MAT4_T); return MAT4_T; }
">=" { yTRACE(GEQ); return GEQ; }
"<=" { yTRACE(LEQ); return LEQ; }
"!=" { yTRACE(NEQ); return NEQ; }
{DIGIT}+"."{DIGIT}*  {yTRACE(FLOAT_C); return FLOAT_C;}
{DIGIT}+ {CheckInt(); yTRACE(INT_C); return INT_C;}
[a-zA-Z_][a-zA-Z0-9_]*  { CheckIdentifier(); yTRACE(ID); return ID; }
"{" { yTRACE('{'); return '{'; }
"}" { yTRACE('}'); return '}'; }
"(" { yTRACE('('); return '('; }
")" { yTRACE(')'); return ')'; }
"[" { yTRACE('['); return '['; }
"]" { yTRACE(']'); return ']'; }
";" { yTRACE(';'); return ';'; }
"," { yTRACE(','); return ','; }
"+" { yTRACE('+'); return '+'; }
"-" { yTRACE('-'); return '-'; }
"*" { yTRACE('*'); return '*'; }
"/" { yTRACE('/'); return '/'; }
"^" { yTRACE('^'); return '^'; }
"!" { yTRACE('!'); return '!'; }
"&" { yTRACE('&'); return '&'; }
"|" { yTRACE('|'); return '|'; }
"=" { yTRACE('='); return '='; }
"<" { yTRACE('<'); return '<'; }
">" { yTRACE('>'); return '>'; }
{WS} { /* ignore whitespace */ }
"\n" { yyline++; }
. { yERROR("Unknown token"); }


%%

int CheckInt() {
  int num = atoi(yytext);
  if (num > MAX_INTEGER){
    yERROR("Integer constant out of range");
  }
  return 0;
}

int CheckIdentifier() {
  if (strlen(yytext) > MAX_IDENTIFIER){
    yERROR("Identifier length out of range");
  }
  return 0;
}