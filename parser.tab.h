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
    INT_C = 258,                   /* INT_C  */
    FLOAT_C = 259,                 /* FLOAT_C  */
    ID = 260,                      /* ID  */
    IF = 261,                      /* IF  */
    ELSE = 262,                    /* ELSE  */
    WHILE = 263,                   /* WHILE  */
    BREAK = 264,                   /* BREAK  */
    RETURN = 265,                  /* RETURN  */
    CONST = 266,                   /* CONST  */
    TRUE_T = 267,                  /* TRUE_T  */
    FALSE_T = 268,                 /* FALSE_T  */
    INTEGER_T = 269,               /* INTEGER_T  */
    BOOLEAN_T = 270,               /* BOOLEAN_T  */
    FLOAT_T = 271,                 /* FLOAT_T  */
    VEC2_T = 272,                  /* VEC2_T  */
    VEC3_T = 273,                  /* VEC3_T  */
    VEC4_T = 274,                  /* VEC4_T  */
    IVEC2_T = 275,                 /* IVEC2_T  */
    IVEC3_T = 276,                 /* IVEC3_T  */
    IVEC4_T = 277,                 /* IVEC4_T  */
    BVEC2_T = 278,                 /* BVEC2_T  */
    BVEC3_T = 279,                 /* BVEC3_T  */
    BVEC4_T = 280,                 /* BVEC4_T  */
    MAT2_T = 281,                  /* MAT2_T  */
    MAT3_T = 282,                  /* MAT3_T  */
    MAT4_T = 283,                  /* MAT4_T  */
    GEQ = 284,                     /* GEQ  */
    LEQ = 285,                     /* LEQ  */
    NEQ = 286                      /* NEQ  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define INT_C 258
#define FLOAT_C 259
#define ID 260
#define IF 261
#define ELSE 262
#define WHILE 263
#define BREAK 264
#define RETURN 265
#define CONST 266
#define TRUE_T 267
#define FALSE_T 268
#define INTEGER_T 269
#define BOOLEAN_T 270
#define FLOAT_T 271
#define VEC2_T 272
#define VEC3_T 273
#define VEC4_T 274
#define IVEC2_T 275
#define IVEC3_T 276
#define IVEC4_T 277
#define BVEC2_T 278
#define BVEC3_T 279
#define BVEC4_T 280
#define MAT2_T 281
#define MAT3_T 282
#define MAT4_T 283
#define GEQ 284
#define LEQ 285
#define NEQ 286

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 54 "parser.y"

  int num;

#line 133 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
