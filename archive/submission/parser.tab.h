/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    FLOAT_T = 258,                 /* FLOAT_T  */
    INT_T = 259,                   /* INT_T  */
    BOOL_T = 260,                  /* BOOL_T  */
    CONST = 261,                   /* CONST  */
    FALSE_C = 262,                 /* FALSE_C  */
    TRUE_C = 263,                  /* TRUE_C  */
    IF = 264,                      /* IF  */
    ELSE = 265,                    /* ELSE  */
    AND = 266,                     /* AND  */
    OR = 267,                      /* OR  */
    NEQ = 268,                     /* NEQ  */
    EQ = 269,                      /* EQ  */
    LEQ = 270,                     /* LEQ  */
    GEQ = 271,                     /* GEQ  */
    VEC_T = 272,                   /* VEC_T  */
    BVEC_T = 273,                  /* BVEC_T  */
    IVEC_T = 274,                  /* IVEC_T  */
    FLOAT_C = 275,                 /* FLOAT_C  */
    INT_C = 276,                   /* INT_C  */
    ID = 277,                      /* ID  */
    FUNC = 278,                    /* FUNC  */
    UMINUS = 279,                  /* UMINUS  */
    WITHOUT_ELSE = 280,            /* WITHOUT_ELSE  */
    WITH_ELSE = 281                /* WITH_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define FLOAT_T 258
#define INT_T 259
#define BOOL_T 260
#define CONST 261
#define FALSE_C 262
#define TRUE_C 263
#define IF 264
#define ELSE 265
#define AND 266
#define OR 267
#define NEQ 268
#define EQ 269
#define LEQ 270
#define GEQ 271
#define VEC_T 272
#define BVEC_T 273
#define IVEC_T 274
#define FLOAT_C 275
#define INT_C 276
#define ID 277
#define FUNC 278
#define UMINUS 279
#define WITHOUT_ELSE 280
#define WITH_ELSE 281

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 32 "parser.y"

  int as_int;
  int as_vec;
  float as_float;
  char *as_str;
  int as_func;
  node *as_ast;

#line 128 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
