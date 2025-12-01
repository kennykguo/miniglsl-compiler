/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
 * 
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

#include <string.h>

#include "common.h"
#include "ast.h"
#include "symbol.h"
#include "semantic.h"

#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(char* s);
int yylex();
extern int yyline;

#line 28 "parser.y"

#define YYDEBUG 1

#line 101 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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

#line 215 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_FLOAT_T = 3,                    /* FLOAT_T  */
  YYSYMBOL_INT_T = 4,                      /* INT_T  */
  YYSYMBOL_BOOL_T = 5,                     /* BOOL_T  */
  YYSYMBOL_CONST = 6,                      /* CONST  */
  YYSYMBOL_FALSE_C = 7,                    /* FALSE_C  */
  YYSYMBOL_TRUE_C = 8,                     /* TRUE_C  */
  YYSYMBOL_IF = 9,                         /* IF  */
  YYSYMBOL_ELSE = 10,                      /* ELSE  */
  YYSYMBOL_AND = 11,                       /* AND  */
  YYSYMBOL_OR = 12,                        /* OR  */
  YYSYMBOL_NEQ = 13,                       /* NEQ  */
  YYSYMBOL_EQ = 14,                        /* EQ  */
  YYSYMBOL_LEQ = 15,                       /* LEQ  */
  YYSYMBOL_GEQ = 16,                       /* GEQ  */
  YYSYMBOL_VEC_T = 17,                     /* VEC_T  */
  YYSYMBOL_BVEC_T = 18,                    /* BVEC_T  */
  YYSYMBOL_IVEC_T = 19,                    /* IVEC_T  */
  YYSYMBOL_FLOAT_C = 20,                   /* FLOAT_C  */
  YYSYMBOL_INT_C = 21,                     /* INT_C  */
  YYSYMBOL_ID = 22,                        /* ID  */
  YYSYMBOL_FUNC = 23,                      /* FUNC  */
  YYSYMBOL_24_ = 24,                       /* '<'  */
  YYSYMBOL_25_ = 25,                       /* '>'  */
  YYSYMBOL_26_ = 26,                       /* '+'  */
  YYSYMBOL_27_ = 27,                       /* '-'  */
  YYSYMBOL_28_ = 28,                       /* '*'  */
  YYSYMBOL_29_ = 29,                       /* '/'  */
  YYSYMBOL_30_ = 30,                       /* '^'  */
  YYSYMBOL_31_ = 31,                       /* '!'  */
  YYSYMBOL_UMINUS = 32,                    /* UMINUS  */
  YYSYMBOL_33_ = 33,                       /* '('  */
  YYSYMBOL_34_ = 34,                       /* '['  */
  YYSYMBOL_WITHOUT_ELSE = 35,              /* WITHOUT_ELSE  */
  YYSYMBOL_WITH_ELSE = 36,                 /* WITH_ELSE  */
  YYSYMBOL_37_ = 37,                       /* '{'  */
  YYSYMBOL_38_ = 38,                       /* '}'  */
  YYSYMBOL_39_ = 39,                       /* ';'  */
  YYSYMBOL_40_ = 40,                       /* '='  */
  YYSYMBOL_41_ = 41,                       /* ')'  */
  YYSYMBOL_42_ = 42,                       /* ']'  */
  YYSYMBOL_43_ = 43,                       /* ','  */
  YYSYMBOL_YYACCEPT = 44,                  /* $accept  */
  YYSYMBOL_program = 45,                   /* program  */
  YYSYMBOL_scope = 46,                     /* scope  */
  YYSYMBOL_declarations = 47,              /* declarations  */
  YYSYMBOL_statements = 48,                /* statements  */
  YYSYMBOL_declaration = 49,               /* declaration  */
  YYSYMBOL_statement = 50,                 /* statement  */
  YYSYMBOL_type = 51,                      /* type  */
  YYSYMBOL_expression = 52,                /* expression  */
  YYSYMBOL_variable = 53,                  /* variable  */
  YYSYMBOL_arguments = 54,                 /* arguments  */
  YYSYMBOL_arguments_opt = 55              /* arguments_opt  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   219

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  50
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  95

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   281


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    31,     2,     2,     2,     2,     2,     2,
      33,    41,    28,    26,    43,    27,     2,    29,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    39,
      24,    40,    25,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    34,     2,    42,    30,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    37,     2,    38,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    32,
      35,    36
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    88,    88,    94,   100,   104,   109,   113,   118,   121,
     124,   131,   134,   137,   140,   143,   150,   153,   156,   159,
     162,   165,   172,   175,   178,   181,   184,   187,   190,   193,
     196,   199,   202,   205,   208,   211,   214,   217,   220,   223,
     226,   229,   232,   235,   238,   244,   247,   253,   256,   262,
     266
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "FLOAT_T", "INT_T",
  "BOOL_T", "CONST", "FALSE_C", "TRUE_C", "IF", "ELSE", "AND", "OR", "NEQ",
  "EQ", "LEQ", "GEQ", "VEC_T", "BVEC_T", "IVEC_T", "FLOAT_C", "INT_C",
  "ID", "FUNC", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'^'", "'!'",
  "UMINUS", "'('", "'['", "WITHOUT_ELSE", "WITH_ELSE", "'{'", "'}'", "';'",
  "'='", "')'", "']'", "','", "$accept", "program", "scope",
  "declarations", "statements", "declaration", "statement", "type",
  "expression", "variable", "arguments", "arguments_opt", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-49)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -35,   -49,     4,   -49,   196,   -49,   -49,   -49,   -49,   200,
     -49,   -49,   -49,    44,   -49,    -7,    18,    21,    17,   -49,
     -49,   -49,   -49,    22,   -14,    27,    38,    47,    38,   -49,
      38,    38,   -49,   -49,   -49,   -49,    37,    38,    38,    38,
      51,    61,   -49,    56,   101,   121,   141,    38,   -49,   -49,
      81,    38,    38,    38,    38,    38,    38,    38,    38,    38,
      38,    38,    38,    38,    38,    41,   -49,   -49,   -49,   -49,
     161,    57,    58,   -49,    60,    -6,   168,   180,   180,   180,
     180,   180,   180,    19,    19,    73,    73,    73,    94,    38,
     -49,   -49,    41,   161,   -49
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     5,     0,     2,     7,     1,    20,    16,    18,     0,
      21,    19,    17,     0,     4,     0,     0,     0,    45,     3,
      15,    14,     6,     0,     0,     0,     0,     0,     0,     8,
       0,     0,    40,    39,    42,    41,     0,     0,     0,     0,
       0,     0,    44,     0,     0,     0,     0,    50,    24,    25,
       0,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,    11,     9,    10,
      48,    49,     0,    43,     0,    26,    27,    29,    28,    31,
      33,    30,    32,    34,    35,    36,    37,    38,    13,     0,
      23,    22,     0,    47,    12
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -49,   -49,   118,   -49,   -49,   -49,   -48,     7,   -25,   -13,
     -49,    68
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,    21,     4,    13,    14,    22,    40,    70,    42,
      71,    72
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      23,    41,     1,    44,     5,    45,    46,    54,    55,    56,
      57,    15,    48,    49,    50,    24,    16,    88,    58,    59,
      60,    61,    62,    63,    64,    29,    30,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      25,     6,     7,     8,    94,    32,    33,    62,    63,    64,
      17,    27,    23,    17,    26,    10,    11,    12,    34,    35,
      18,    36,    28,    18,    93,    37,    18,    31,    43,    38,
      47,    39,    52,    53,    54,    55,    56,    57,     1,    23,
      20,     1,    19,    20,    51,    58,    59,    60,    61,    62,
      63,    64,    52,    53,    54,    55,    56,    57,    66,    90,
      89,    91,    65,    64,    92,    58,    59,    60,    61,    62,
      63,    64,    52,    53,    54,    55,    56,    57,     3,    74,
       0,     0,    73,     0,     0,    58,    59,    60,    61,    62,
      63,    64,    52,    53,    54,    55,    56,    57,     0,     0,
      67,     0,     0,     0,     0,    58,    59,    60,    61,    62,
      63,    64,    52,    53,    54,    55,    56,    57,     0,     0,
      68,     0,     0,     0,     0,    58,    59,    60,    61,    62,
      63,    64,    52,    53,    54,    55,    56,    57,     0,    52,
      69,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    58,    59,    60,    61,    62,    63,    64,     6,
       7,     8,     9,     6,     7,     8,    60,    61,    62,    63,
      64,     0,     0,    10,    11,    12,     0,    10,    11,    12
};

static const yytype_int8 yycheck[] =
{
      13,    26,    37,    28,     0,    30,    31,    13,    14,    15,
      16,     4,    37,    38,    39,    22,     9,    65,    24,    25,
      26,    27,    28,    29,    30,    39,    40,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      22,     3,     4,     5,    92,     7,     8,    28,    29,    30,
       9,    34,    65,     9,    33,    17,    18,    19,    20,    21,
      22,    23,    40,    22,    89,    27,    22,    40,    21,    31,
      33,    33,    11,    12,    13,    14,    15,    16,    37,    92,
      39,    37,    38,    39,    33,    24,    25,    26,    27,    28,
      29,    30,    11,    12,    13,    14,    15,    16,    42,    41,
      43,    41,    41,    30,    10,    24,    25,    26,    27,    28,
      29,    30,    11,    12,    13,    14,    15,    16,     0,    51,
      -1,    -1,    41,    -1,    -1,    24,    25,    26,    27,    28,
      29,    30,    11,    12,    13,    14,    15,    16,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    24,    25,    26,    27,    28,
      29,    30,    11,    12,    13,    14,    15,    16,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    24,    25,    26,    27,    28,
      29,    30,    11,    12,    13,    14,    15,    16,    -1,    11,
      39,    13,    14,    15,    16,    24,    25,    26,    27,    28,
      29,    30,    24,    25,    26,    27,    28,    29,    30,     3,
       4,     5,     6,     3,     4,     5,    26,    27,    28,    29,
      30,    -1,    -1,    17,    18,    19,    -1,    17,    18,    19
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    37,    45,    46,    47,     0,     3,     4,     5,     6,
      17,    18,    19,    48,    49,    51,    51,     9,    22,    38,
      39,    46,    50,    53,    22,    22,    33,    34,    40,    39,
      40,    40,     7,     8,    20,    21,    23,    27,    31,    33,
      51,    52,    53,    21,    52,    52,    52,    33,    52,    52,
      52,    33,    11,    12,    13,    14,    15,    16,    24,    25,
      26,    27,    28,    29,    30,    41,    42,    39,    39,    39,
      52,    54,    55,    41,    55,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    50,    43,
      41,    41,    10,    52,    50
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    44,    45,    46,    47,    47,    48,    48,    49,    49,
      49,    50,    50,    50,    50,    50,    51,    51,    51,    51,
      51,    51,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    53,    53,    54,    54,    55,
      55
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     4,     2,     0,     2,     0,     3,     5,
       6,     4,     7,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     1,     1,     3,     1,     1,     4,     3,     1,     1,
       0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: scope  */
#line 89 "parser.y"
      { yTRACE("program -> scope\n");
        ast = (yyvsp[0].as_ast); }
#line 1331 "y.tab.c"
    break;

  case 3: /* scope: '{' declarations statements '}'  */
#line 95 "parser.y"
      { yTRACE("scope -> { declarations statements }\n");
        (yyval.as_ast) = ast_allocate(SCOPE_NODE, (yyvsp[-2].as_ast), (yyvsp[-1].as_ast)); }
#line 1338 "y.tab.c"
    break;

  case 4: /* declarations: declarations declaration  */
#line 101 "parser.y"
      { yTRACE("declarations -> declarations  declaration\n");
        (yyval.as_ast) = ast_allocate(DECLARATIONS_NODE, (yyvsp[0].as_ast), (yyvsp[-1].as_ast)); }
#line 1345 "y.tab.c"
    break;

  case 5: /* declarations: %empty  */
#line 104 "parser.y"
      { yTRACE("declarations -> \n");
        (yyval.as_ast) = NULL; }
#line 1352 "y.tab.c"
    break;

  case 6: /* statements: statements statement  */
#line 110 "parser.y"
      { yTRACE("statements -> statements statement\n");
        (yyval.as_ast) = ast_allocate(STATEMENTS_NODE, (yyvsp[0].as_ast), (yyvsp[-1].as_ast)); }
#line 1359 "y.tab.c"
    break;

  case 7: /* statements: %empty  */
#line 113 "parser.y"
      { yTRACE("statements -> \n");
        (yyval.as_ast) = NULL; }
#line 1366 "y.tab.c"
    break;

  case 8: /* declaration: type ID ';'  */
#line 119 "parser.y"
      { yTRACE("declaration -> type ID ;\n");
        (yyval.as_ast) = ast_allocate(DECLARATION_NODE, 0, (yyvsp[-2].as_ast), (yyvsp[-1].as_str), NULL); }
#line 1373 "y.tab.c"
    break;

  case 9: /* declaration: type ID '=' expression ';'  */
#line 122 "parser.y"
      { yTRACE("declaration -> type ID = expression ;\n");
        (yyval.as_ast) = ast_allocate(DECLARATION_NODE, 0, (yyvsp[-4].as_ast), (yyvsp[-3].as_str), (yyvsp[-1].as_ast)); }
#line 1380 "y.tab.c"
    break;

  case 10: /* declaration: CONST type ID '=' expression ';'  */
#line 125 "parser.y"
      { yTRACE("declaration -> CONST type ID =  expression ;\n");
        (yyval.as_ast) = ast_allocate(DECLARATION_NODE, 1, (yyvsp[-4].as_ast), (yyvsp[-3].as_str), (yyvsp[-1].as_ast)); }
#line 1387 "y.tab.c"
    break;

  case 11: /* statement: variable '=' expression ';'  */
#line 132 "parser.y"
      { yTRACE("statement -> variable = expression ;\n");
        (yyval.as_ast) = ast_allocate(ASSIGNMENT_NODE, (yyvsp[-3].as_ast), (yyvsp[-1].as_ast)); }
#line 1394 "y.tab.c"
    break;

  case 12: /* statement: IF '(' expression ')' statement ELSE statement  */
#line 135 "parser.y"
      { yTRACE("statement -> IF (  expression  ) statement ELSE statement \n");
        (yyval.as_ast) = ast_allocate(IF_STMT_NODE, (yyvsp[-4].as_ast), (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1401 "y.tab.c"
    break;

  case 13: /* statement: IF '(' expression ')' statement  */
#line 138 "parser.y"
      { yTRACE("statement -> IF ( expression ) statement \n");
        (yyval.as_ast) = ast_allocate(IF_STMT_NODE, (yyvsp[-2].as_ast), (yyvsp[0].as_ast), NULL); }
#line 1408 "y.tab.c"
    break;

  case 14: /* statement: scope  */
#line 141 "parser.y"
      { yTRACE("statement -> scope \n");
        (yyval.as_ast) = (yyvsp[0].as_ast); }
#line 1415 "y.tab.c"
    break;

  case 15: /* statement: ';'  */
#line 144 "parser.y"
      { yTRACE("statement -> ; \n");
        (yyval.as_ast) = NULL; }
#line 1422 "y.tab.c"
    break;

  case 16: /* type: INT_T  */
#line 151 "parser.y"
      { yTRACE("type -> INT_T \n");
        (yyval.as_ast) = ast_allocate(TYPE_NODE, INT_T, 0); }
#line 1429 "y.tab.c"
    break;

  case 17: /* type: IVEC_T  */
#line 154 "parser.y"
      { yTRACE("type -> IVEC_T \n");
        (yyval.as_ast) = ast_allocate(TYPE_NODE, IVEC_T, (yyvsp[0].as_vec)); }
#line 1436 "y.tab.c"
    break;

  case 18: /* type: BOOL_T  */
#line 157 "parser.y"
      {  yTRACE("type -> BOOL_T \n");
        (yyval.as_ast) = ast_allocate(TYPE_NODE, BOOL_T, 0); }
#line 1443 "y.tab.c"
    break;

  case 19: /* type: BVEC_T  */
#line 160 "parser.y"
      { yTRACE("type -> BVEC_T \n");
        (yyval.as_ast) = ast_allocate(TYPE_NODE, BVEC_T, (yyvsp[0].as_vec)); }
#line 1450 "y.tab.c"
    break;

  case 20: /* type: FLOAT_T  */
#line 163 "parser.y"
      { yTRACE("type -> FLOAT_T \n");
        (yyval.as_ast) = ast_allocate(TYPE_NODE, FLOAT_T, 0); }
#line 1457 "y.tab.c"
    break;

  case 21: /* type: VEC_T  */
#line 166 "parser.y"
      { yTRACE("type -> VEC_T \n");
        (yyval.as_ast) = ast_allocate(TYPE_NODE, VEC_T, (yyvsp[0].as_vec)); }
#line 1464 "y.tab.c"
    break;

  case 22: /* expression: type '(' arguments_opt ')'  */
#line 173 "parser.y"
      { yTRACE("expression -> type ( arguments_opt ) \n");
        (yyval.as_ast) = ast_allocate(CONSTRUCTOR_NODE, (yyvsp[-3].as_ast), (yyvsp[-1].as_ast)); }
#line 1471 "y.tab.c"
    break;

  case 23: /* expression: FUNC '(' arguments_opt ')'  */
#line 176 "parser.y"
      { yTRACE("expression  -> FUNC ( arguments_opt ) \n");
        (yyval.as_ast) = ast_allocate(FUNCTION_NODE, (yyvsp[-3].as_func), (yyvsp[-1].as_ast)); }
#line 1478 "y.tab.c"
    break;

  case 24: /* expression: '-' expression  */
#line 179 "parser.y"
      { yTRACE("expression -> - expression \n");
        (yyval.as_ast) = ast_allocate(UNARY_EXPR_NODE, '-', (yyvsp[0].as_ast)); }
#line 1485 "y.tab.c"
    break;

  case 25: /* expression: '!' expression  */
#line 182 "parser.y"
      { yTRACE("expression -> ! expression \n");
        (yyval.as_ast) = ast_allocate(UNARY_EXPR_NODE, '!', (yyvsp[0].as_ast)); }
#line 1492 "y.tab.c"
    break;

  case 26: /* expression: expression AND expression  */
#line 185 "parser.y"
      { yTRACE("expression  -> expression AND expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, AND, (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1499 "y.tab.c"
    break;

  case 27: /* expression: expression OR expression  */
#line 188 "parser.y"
      { yTRACE("expression -> expression OR expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, OR, (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1506 "y.tab.c"
    break;

  case 28: /* expression: expression EQ expression  */
#line 191 "parser.y"
      { yTRACE("expression -> expression EQ expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, EQ, (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1513 "y.tab.c"
    break;

  case 29: /* expression: expression NEQ expression  */
#line 194 "parser.y"
      {  yTRACE("expression -> expression NEQ expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, NEQ, (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1520 "y.tab.c"
    break;

  case 30: /* expression: expression '<' expression  */
#line 197 "parser.y"
      { yTRACE("expression -> expression < expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, '<', (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1527 "y.tab.c"
    break;

  case 31: /* expression: expression LEQ expression  */
#line 200 "parser.y"
      { yTRACE("expression  -> expression LEQ expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, LEQ, (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1534 "y.tab.c"
    break;

  case 32: /* expression: expression '>' expression  */
#line 203 "parser.y"
      { yTRACE("expression -> expression > expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, '>', (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1541 "y.tab.c"
    break;

  case 33: /* expression: expression GEQ expression  */
#line 206 "parser.y"
      { yTRACE("expression -> expression GEQ expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, GEQ, (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1548 "y.tab.c"
    break;

  case 34: /* expression: expression '+' expression  */
#line 209 "parser.y"
      { yTRACE("expression -> expression + expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, '+', (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1555 "y.tab.c"
    break;

  case 35: /* expression: expression '-' expression  */
#line 212 "parser.y"
      { yTRACE("expression -> expression - expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, '-', (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1562 "y.tab.c"
    break;

  case 36: /* expression: expression '*' expression  */
#line 215 "parser.y"
      { yTRACE("expression  -> expression * expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, '*', (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1569 "y.tab.c"
    break;

  case 37: /* expression: expression '/' expression  */
#line 218 "parser.y"
      { yTRACE("expression -> expression / expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, '/', (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1576 "y.tab.c"
    break;

  case 38: /* expression: expression '^' expression  */
#line 221 "parser.y"
      { yTRACE("expression -> expression ^ expression \n");
        (yyval.as_ast) = ast_allocate(BINARY_EXPR_NODE, '^', (yyvsp[-2].as_ast), (yyvsp[0].as_ast)); }
#line 1583 "y.tab.c"
    break;

  case 39: /* expression: TRUE_C  */
#line 224 "parser.y"
      { yTRACE("expression -> TRUE_C \n");
        (yyval.as_ast) = ast_allocate(BOOL_LITERAL_NODE, 1); }
#line 1590 "y.tab.c"
    break;

  case 40: /* expression: FALSE_C  */
#line 227 "parser.y"
      { yTRACE("expression -> FALSE_C \n");
        (yyval.as_ast) = ast_allocate(BOOL_LITERAL_NODE, 0); }
#line 1597 "y.tab.c"
    break;

  case 41: /* expression: INT_C  */
#line 230 "parser.y"
      { yTRACE("expression  -> INT_C \n");
        (yyval.as_ast) = ast_allocate(INT_LITERAL_NODE, (yyvsp[0].as_int)); }
#line 1604 "y.tab.c"
    break;

  case 42: /* expression: FLOAT_C  */
#line 233 "parser.y"
      { yTRACE("expression -> FLOAT_C \n");
        (yyval.as_ast) = ast_allocate(FLOAT_LITERAL_NODE, (yyvsp[0].as_float)); }
#line 1611 "y.tab.c"
    break;

  case 43: /* expression: '(' expression ')'  */
#line 236 "parser.y"
      { yTRACE("expression  -> ( expression ) \n");
        (yyval.as_ast) = (yyvsp[-1].as_ast); }
#line 1618 "y.tab.c"
    break;

  case 44: /* expression: variable  */
#line 239 "parser.y"
      { yTRACE("expression -> variable \n");
        (yyval.as_ast) = (yyvsp[0].as_ast); }
#line 1625 "y.tab.c"
    break;

  case 45: /* variable: ID  */
#line 245 "parser.y"
      { yTRACE("variable -> ID \n");
        (yyval.as_ast) = ast_allocate(VAR_NODE, (yyvsp[0].as_str), 0); }
#line 1632 "y.tab.c"
    break;

  case 46: /* variable: ID '[' INT_C ']'  */
#line 248 "parser.y"
      { yTRACE("variable -> ID [ INT_C ] \n");
        (yyval.as_ast) = ast_allocate(VAR_NODE, (yyvsp[-3].as_str), 1, (yyvsp[-1].as_int)); }
#line 1639 "y.tab.c"
    break;

  case 47: /* arguments: arguments ',' expression  */
#line 254 "parser.y"
      { yTRACE("arguments -> arguments , expression \n");
        (yyval.as_ast) = ast_allocate(ARGUMENTS_NODE, (yyvsp[0].as_ast), (yyvsp[-2].as_ast)); }
#line 1646 "y.tab.c"
    break;

  case 48: /* arguments: expression  */
#line 257 "parser.y"
      { yTRACE("arguments -> expression \n");
        (yyval.as_ast) = ast_allocate(ARGUMENTS_NODE, (yyvsp[0].as_ast), NULL); }
#line 1653 "y.tab.c"
    break;

  case 49: /* arguments_opt: arguments  */
#line 263 "parser.y"
      { yTRACE("arguments_opt -> arguments \n");
        (yyval.as_ast) = (yyvsp[0].as_ast); }
#line 1660 "y.tab.c"
    break;

  case 50: /* arguments_opt: %empty  */
#line 266 "parser.y"
      { yTRACE("arguments_opt -> \n");
        (yyval.as_ast) = NULL; }
#line 1667 "y.tab.c"
    break;


#line 1671 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 270 "parser.y"


void yyerror(char* s) {
  if(errorOccurred) {
    return;
  } else {
    errorOccurred = 1;
  }
  fprintf(errorFile, "\nPARSER ERROR, LINE %d", yyline);
  if(strcmp(s, "parse error")) {
    if(strncmp(s, "parse error, ", 13)) {
      fprintf(errorFile, ": %s\n", s);
    } else {
      fprintf(errorFile, ": %s\n", s+13);
    }
  } else {
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
  }
}
