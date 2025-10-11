/***********************************************************************
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/
/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/
%{
#include <string.h>
#include "common.h"
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }
void yyerror(char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */
%}




/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}


// TODO: Modify me to add more data types
// Can access me from flex useing yyval

%union {
  int num;
}

// identifiers
%token INT_C FLOAT_C
%token ID

// keywords
%token IF ELSE WHILE BREAK RETURN CONST

// boolean literals
%token TRUE_T FALSE_T

// types
%token INTEGER_T BOOLEAN_T FLOAT_T
%token VEC2_T VEC3_T VEC4_T
%token IVEC2_T IVEC3_T IVEC4_T
%token BVEC2_T BVEC3_T BVEC4_T
%token MAT2_T MAT3_T MAT4_T

// multi-character operators
%token GEQ LEQ NEQ



%start program

%%
/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 *  Phase 3:
 *    1. Add code to rules for construction of AST.
 ***********************************************************************/
program
  :   tokens       
  ;
tokens
  :  tokens token  
  |      
  ;
token
  :  INT_C | FLOAT_C | ID
  |  IF | ELSE | WHILE | BREAK | RETURN | CONST
  |  TRUE_T | FALSE_T
  |  INTEGER_T | BOOLEAN_T | FLOAT_T
  |  VEC2_T | VEC3_T | VEC4_T
  |  IVEC2_T | IVEC3_T | IVEC4_T
  |  BVEC2_T | BVEC3_T | BVEC4_T
  |  MAT2_T | MAT3_T | MAT4_T
  |  GEQ | LEQ | NEQ
  |  '{' | '}' | '(' | ')' | '[' | ']' | ';' | ','
  |  '+' | '-' | '*' | '/' | '^' | '!' | '&' | '|' | '=' | '<' | '>'
  ;
%%



/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(char* s) {
  if (errorOccurred)
    return;    /* Error has already been reported by scanner */
  else
    errorOccurred = 1;
        
  fprintf(errorFile, "\nPARSER ERROR, LINE %d",yyline);
  if (strcmp(s, "parse error")) {
    if (strncmp(s, "parse error, ", 13))
      fprintf(errorFile, ": %s\n", s);
    else
      fprintf(errorFile, ": %s\n", s+13);
  } else
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
}

