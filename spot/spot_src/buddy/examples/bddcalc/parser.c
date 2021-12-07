/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 8 "parser.y" /* yacc.c:337  */

#include <string.h>
#include <stdarg.h>
#include <fstream>
#include <getopt.h>
#define IMPLEMENTSLIST /* Special for list template handling */
#include "slist.h"
#include "hashtbl.h"
#include "parser_.h"

   using namespace std;

   /* Definitions for storing and caching of identifiers */
#define inputTag  0
#define exprTag   1

   struct nodeData
   {
      nodeData(const nodeData &d) { tag=d.tag; name=sdup(d.name); val=d.val; }
      nodeData(int t, char *n, bdd v) { tag=t; name=n; val=v; }
      ~nodeData(void) { delete[] name; }
      int tag;
      char *name;
      bdd val;
   };

   typedef SList<nodeData> nodeLst;
   nodeLst inputs;
   hashTable names;

      /* Other */
   int linenum;

   bddgbchandler gbcHandler = bdd_default_gbchandler;

      /* Prototypes */
void actInit(token *nodes, token *cache);
void actInputs(void);
void actAddInput(token *id);
void actAssign(token *id, token *expr);
void actOpr2(token *res, token *left, token *right, int opr);
void actNot(token *res, token *right);
void actId(token *res, token *id);
void actConst(token *res, int);
void actSize(token *id);
void actDot(token *fname, token *id);
void actAutoreorder(token *times, token *method);
void actCache(void);
void actTautology(token *id);
void actExist(token *res, token *var, token *expr);
void actForall(token *res, token *var, token *expr);
void actQuantVar2(token *res, token *id, token *list);
void actQuantVar1(token *res, token *id);
void actPrint(token *id);


#line 127 "parser.c" /* yacc.c:337  */
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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_id = 258,
    T_str = 259,
    T_intval = 260,
    T_true = 261,
    T_false = 262,
    T_initial = 263,
    T_inputs = 264,
    T_actions = 265,
    T_size = 266,
    T_dumpdot = 267,
    T_autoreorder = 268,
    T_reorder = 269,
    T_win2 = 270,
    T_win2ite = 271,
    T_sift = 272,
    T_siftite = 273,
    T_none = 274,
    T_cache = 275,
    T_tautology = 276,
    T_print = 277,
    T_lpar = 278,
    T_rpar = 279,
    T_equal = 280,
    T_semi = 281,
    T_dot = 282,
    T_exist = 283,
    T_forall = 284,
    T_biimp = 285,
    T_imp = 286,
    T_or = 287,
    T_nor = 288,
    T_xor = 289,
    T_nand = 290,
    T_and = 291,
    T_not = 292
  };
#endif
/* Tokens.  */
#define T_id 258
#define T_str 259
#define T_intval 260
#define T_true 261
#define T_false 262
#define T_initial 263
#define T_inputs 264
#define T_actions 265
#define T_size 266
#define T_dumpdot 267
#define T_autoreorder 268
#define T_reorder 269
#define T_win2 270
#define T_win2ite 271
#define T_sift 272
#define T_siftite 273
#define T_none 274
#define T_cache 275
#define T_tautology 276
#define T_print 277
#define T_lpar 278
#define T_rpar 279
#define T_equal 280
#define T_semi 281
#define T_dot 282
#define T_exist 283
#define T_forall 284
#define T_biimp 285
#define T_imp 286
#define T_or 287
#define T_nor 288
#define T_xor 289
#define T_nand 290
#define T_and 291
#define T_not 292

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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


#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
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
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
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
#define YYLAST   88

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  46
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  84

#define YYUNDEFTOK  2
#define YYMAXUTOK   292

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    96,    96,   102,   106,   110,   111,   118,   122,   123,
     127,   128,   129,   130,   131,   132,   133,   137,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   157,   158,   162,   163,   168,   172,   176,   177,   181,
     182,   183,   184,   185,   189,   193,   197
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_id", "T_str", "T_intval", "T_true",
  "T_false", "T_initial", "T_inputs", "T_actions", "T_size", "T_dumpdot",
  "T_autoreorder", "T_reorder", "T_win2", "T_win2ite", "T_sift",
  "T_siftite", "T_none", "T_cache", "T_tautology", "T_print", "T_lpar",
  "T_rpar", "T_equal", "T_semi", "T_dot", "T_exist", "T_forall", "T_biimp",
  "T_imp", "T_or", "T_nor", "T_xor", "T_nand", "T_and", "T_not", "$accept",
  "calc", "initial", "inputs", "inputSeq", "actions", "actionSeq",
  "action", "assign", "expr", "quantifier", "varlist", "size", "dot",
  "reorder", "method", "cache", "tautology", "print", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292
};
# endif

#define YYPACT_NINF -38

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-38)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      -6,     7,    15,    14,    46,   -38,    49,    44,    50,   -38,
      -2,    36,   -38,   -38,   -38,   -38,    30,    74,    75,    73,
      51,   -38,    77,    78,    36,    56,   -38,   -38,   -38,   -38,
     -38,   -38,   -38,    -3,   -38,    80,    51,   -38,   -38,   -38,
     -38,   -38,   -38,   -38,   -38,    58,   -38,   -38,   -38,   -38,
      -3,    82,    82,    -3,    10,   -38,   -38,   -38,   -38,    29,
     -38,     8,    11,   -38,    -3,    -3,    -3,    -3,    -3,    -3,
      -3,   -38,   -38,    -3,    -3,   -26,    39,   -17,   -17,   -14,
     -38,   -38,    10,    10
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     1,     0,     0,     0,     6,
       0,     0,     2,     3,     5,     4,     0,     0,     0,     0,
       0,    44,     0,     0,     7,     0,    10,    11,    12,    13,
      14,    15,    16,     0,    35,     0,     0,    39,    40,    41,
      42,    43,    37,    45,    46,     0,     9,    27,    28,    29,
       0,     0,     0,     0,    17,    30,    36,    38,     8,     0,
      34,     0,     0,    25,     0,     0,     0,     0,     0,     0,
       0,    26,    33,     0,     0,    24,    23,    21,    22,    20,
      19,    18,    31,    32
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -38,   -38,   -38,   -38,   -38,   -38,   -38,    62,   -38,   -37,
     -38,    35,   -38,   -38,   -38,    52,   -38,   -38,   -38
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     3,     7,    10,    12,    24,    25,    26,    54,
      55,    61,    27,    28,    29,    42,    30,    31,    32
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      47,    14,     1,    48,    49,    65,    66,    67,    68,    69,
      70,    72,     4,    59,    72,     5,    63,    68,    69,    70,
      50,    69,    70,     6,    15,    51,    52,    75,    76,    77,
      78,    79,    80,    81,    53,    73,    82,    83,    74,    16,
      64,    65,    66,    67,    68,    69,    70,    17,    18,    19,
      20,     8,     9,    71,    11,    33,    21,    22,    23,    64,
      65,    66,    67,    68,    69,    70,    37,    38,    39,    40,
      41,    66,    67,    68,    69,    70,    13,    34,    36,    35,
      43,    44,    46,    56,    58,    60,    45,    62,    57
};

static const yytype_uint8 yycheck[] =
{
       3,     3,     8,     6,     7,    31,    32,    33,    34,    35,
      36,     3,     5,    50,     3,     0,    53,    34,    35,    36,
      23,    35,    36,     9,    26,    28,    29,    64,    65,    66,
      67,    68,    69,    70,    37,    27,    73,    74,    27,     3,
      30,    31,    32,    33,    34,    35,    36,    11,    12,    13,
      14,     5,     3,    24,    10,    25,    20,    21,    22,    30,
      31,    32,    33,    34,    35,    36,    15,    16,    17,    18,
      19,    32,    33,    34,    35,    36,    26,     3,     5,     4,
       3,     3,    26,     3,    26,     3,    24,    52,    36
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,    39,    40,     5,     0,     9,    41,     5,     3,
      42,    10,    43,    26,     3,    26,     3,    11,    12,    13,
      14,    20,    21,    22,    44,    45,    46,    50,    51,    52,
      54,    55,    56,    25,     3,     4,     5,    15,    16,    17,
      18,    19,    53,     3,     3,    45,    26,     3,     6,     7,
      23,    28,    29,    37,    47,    48,     3,    53,    26,    47,
       3,    49,    49,    47,    30,    31,    32,    33,    34,    35,
      36,    24,     3,    27,    27,    47,    47,    47,    47,    47,
      47,    47,    47,    47
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    38,    39,    40,    41,    42,    42,    43,    44,    44,
      45,    45,    45,    45,    45,    45,    45,    46,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    48,    48,    49,    49,    50,    51,    52,    52,    53,
      53,    53,    53,    53,    54,    55,    56
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     4,     3,     2,     1,     2,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     1,     1,     1,
       1,     4,     4,     2,     1,     2,     3,     2,     3,     1,
       1,     1,     1,     1,     1,     2,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
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
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
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
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
        case 3:
#line 102 "parser.y" /* yacc.c:1652  */
    { actInit(&yyvsp[-2],&yyvsp[-1]); }
#line 1386 "parser.c" /* yacc.c:1652  */
    break;

  case 4:
#line 106 "parser.y" /* yacc.c:1652  */
    { actInputs(); }
#line 1392 "parser.c" /* yacc.c:1652  */
    break;

  case 5:
#line 110 "parser.y" /* yacc.c:1652  */
    { actAddInput(&yyvsp[0]); }
#line 1398 "parser.c" /* yacc.c:1652  */
    break;

  case 6:
#line 111 "parser.y" /* yacc.c:1652  */
    { actAddInput(&yyvsp[0]); }
#line 1404 "parser.c" /* yacc.c:1652  */
    break;

  case 17:
#line 137 "parser.y" /* yacc.c:1652  */
    { actAssign(&yyvsp[-2],&yyvsp[0]); }
#line 1410 "parser.c" /* yacc.c:1652  */
    break;

  case 18:
#line 141 "parser.y" /* yacc.c:1652  */
    { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_and); }
#line 1416 "parser.c" /* yacc.c:1652  */
    break;

  case 19:
#line 142 "parser.y" /* yacc.c:1652  */
    { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_nand); }
#line 1422 "parser.c" /* yacc.c:1652  */
    break;

  case 20:
#line 143 "parser.y" /* yacc.c:1652  */
    { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_xor); }
#line 1428 "parser.c" /* yacc.c:1652  */
    break;

  case 21:
#line 144 "parser.y" /* yacc.c:1652  */
    { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_or); }
#line 1434 "parser.c" /* yacc.c:1652  */
    break;

  case 22:
#line 145 "parser.y" /* yacc.c:1652  */
    { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_nor); }
#line 1440 "parser.c" /* yacc.c:1652  */
    break;

  case 23:
#line 146 "parser.y" /* yacc.c:1652  */
    { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_imp); }
#line 1446 "parser.c" /* yacc.c:1652  */
    break;

  case 24:
#line 147 "parser.y" /* yacc.c:1652  */
    { actOpr2(&yyval,&yyvsp[-2],&yyvsp[0],bddop_biimp); }
#line 1452 "parser.c" /* yacc.c:1652  */
    break;

  case 25:
#line 148 "parser.y" /* yacc.c:1652  */
    { actNot(&yyval,&yyvsp[0]); }
#line 1458 "parser.c" /* yacc.c:1652  */
    break;

  case 26:
#line 149 "parser.y" /* yacc.c:1652  */
    { yyval.bval = yyvsp[-1].bval; }
#line 1464 "parser.c" /* yacc.c:1652  */
    break;

  case 27:
#line 150 "parser.y" /* yacc.c:1652  */
    { actId(&yyval,&yyvsp[0]); }
#line 1470 "parser.c" /* yacc.c:1652  */
    break;

  case 28:
#line 151 "parser.y" /* yacc.c:1652  */
    { yyval.bval = new bdd(bddtrue); }
#line 1476 "parser.c" /* yacc.c:1652  */
    break;

  case 29:
#line 152 "parser.y" /* yacc.c:1652  */
    { yyval.bval = new bdd(bddfalse); }
#line 1482 "parser.c" /* yacc.c:1652  */
    break;

  case 30:
#line 153 "parser.y" /* yacc.c:1652  */
    { yyval.bval = yyvsp[0].bval; }
#line 1488 "parser.c" /* yacc.c:1652  */
    break;

  case 31:
#line 157 "parser.y" /* yacc.c:1652  */
    { actExist(&yyval,&yyvsp[-2],&yyvsp[0]); }
#line 1494 "parser.c" /* yacc.c:1652  */
    break;

  case 32:
#line 158 "parser.y" /* yacc.c:1652  */
    { actForall(&yyval,&yyvsp[-2],&yyvsp[0]); }
#line 1500 "parser.c" /* yacc.c:1652  */
    break;

  case 33:
#line 162 "parser.y" /* yacc.c:1652  */
    { actQuantVar2(&yyval,&yyvsp[0],&yyvsp[-1]); }
#line 1506 "parser.c" /* yacc.c:1652  */
    break;

  case 34:
#line 163 "parser.y" /* yacc.c:1652  */
    { actQuantVar1(&yyval,&yyvsp[0]); }
#line 1512 "parser.c" /* yacc.c:1652  */
    break;

  case 35:
#line 168 "parser.y" /* yacc.c:1652  */
    { actSize(&yyvsp[0]); }
#line 1518 "parser.c" /* yacc.c:1652  */
    break;

  case 36:
#line 172 "parser.y" /* yacc.c:1652  */
    { actDot(&yyvsp[-1],&yyvsp[0]); }
#line 1524 "parser.c" /* yacc.c:1652  */
    break;

  case 37:
#line 176 "parser.y" /* yacc.c:1652  */
    { bdd_reorder(yyvsp[0].ival); }
#line 1530 "parser.c" /* yacc.c:1652  */
    break;

  case 38:
#line 177 "parser.y" /* yacc.c:1652  */
    { actAutoreorder(&yyvsp[-1],&yyvsp[0]); }
#line 1536 "parser.c" /* yacc.c:1652  */
    break;

  case 39:
#line 181 "parser.y" /* yacc.c:1652  */
    { yyval.ival = BDD_REORDER_WIN2; }
#line 1542 "parser.c" /* yacc.c:1652  */
    break;

  case 40:
#line 182 "parser.y" /* yacc.c:1652  */
    { yyval.ival = BDD_REORDER_WIN2ITE; }
#line 1548 "parser.c" /* yacc.c:1652  */
    break;

  case 41:
#line 183 "parser.y" /* yacc.c:1652  */
    { yyval.ival = BDD_REORDER_SIFT; }
#line 1554 "parser.c" /* yacc.c:1652  */
    break;

  case 42:
#line 184 "parser.y" /* yacc.c:1652  */
    { yyval.ival = BDD_REORDER_SIFTITE; }
#line 1560 "parser.c" /* yacc.c:1652  */
    break;

  case 43:
#line 185 "parser.y" /* yacc.c:1652  */
    { yyval.ival = BDD_REORDER_NONE; }
#line 1566 "parser.c" /* yacc.c:1652  */
    break;

  case 44:
#line 189 "parser.y" /* yacc.c:1652  */
    { actCache(); }
#line 1572 "parser.c" /* yacc.c:1652  */
    break;

  case 45:
#line 193 "parser.y" /* yacc.c:1652  */
    { actTautology(&yyvsp[0]); }
#line 1578 "parser.c" /* yacc.c:1652  */
    break;

  case 46:
#line 197 "parser.y" /* yacc.c:1652  */
    { actPrint(&yyvsp[0]); }
#line 1584 "parser.c" /* yacc.c:1652  */
    break;


#line 1588 "parser.c" /* yacc.c:1652  */
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
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
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 199 "parser.y" /* yacc.c:1918  */

/*************************************************************************
   Main and more
*************************************************************************/

void usage(void)
{
   cerr << "USAGE: bddcalc [-hg] file\n";
   cerr << " -h : print this message\n";
   cerr << " -g : disable garbage collection info\n";
}


int main(int ac, char **av)
{
   int c;

   while ((c=getopt(ac, av, "hg")) != EOF)
   {
      switch (c)
      {
      case 'h':
	 usage();
	 break;
      case 'g':
	 gbcHandler = bdd_default_gbchandler;
	 break;
      }
   }

   if (optind >= ac)
      usage();

   yyin = fopen(av[optind],"r");
   if (!yyin)
   {
      cerr << "Could not open file: " << av[optind] << endl;
      exit(2);
   }

   linenum = 1;
   bdd_setcacheratio(2);
   yyparse();

   bdd_printstat();
   bdd_done();

   return 0;
}


void yyerror(const char *fmt, ...)
{
   va_list argp;
   va_start(argp,fmt);
   fprintf(stderr, "Parse error in (or before) line %d: ", linenum);
   vfprintf(stderr, fmt, argp);
   va_end(argp);
   exit(3);
}


/*************************************************************************
   Semantic actions
*************************************************************************/

void actInit(token *nodes, token *cache)
{
   bdd_init(nodes->ival, cache->ival);
   bdd_gbc_hook(gbcHandler);
   bdd_reorder_verbose(0);
}


void actInputs(void)
{
   bdd_setvarnum(inputs.size());

   int vnum=0;
   for (nodeLst::ite i=inputs.first() ; i.more() ; i++, vnum++)
   {
      if (names.exists((*i).name))
	 yyerror("Redefinition of input %s", (*i).name);

      (*i).val = bdd_ithvar(vnum);
      hashData hd((*i).name, 0, &(*i));
      names.add(hd);
   }

   bdd_varblockall();
}


void actAddInput(token *id)
{
   inputs.append( nodeData(inputTag,sdup(id->id),bddtrue) );
}


void actAssign(token *id, token *expr)
{
   if (names.exists(id->id))
      yyerror("Redefinition of %s", id->id);

   nodeData *d = new nodeData(exprTag, sdup(id->id), *expr->bval);
   hashData hd(d->name, 0, d);
   names.add(hd);
   delete expr->bval;
}


void actOpr2(token *res, token *left, token *right, int opr)
{
   res->bval = new bdd( bdd_apply(*left->bval, *right->bval, opr) );
   delete left->bval;
   delete right->bval;
}


void actNot(token *res, token *right)
{
   res->bval = new bdd( bdd_not(*right->bval) );
   delete right->bval;
   //printf("%5d -> %f\n", fixme, bdd_satcount(*res->bval));
}


void actId(token *res, token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      res->bval = new bdd( ((nodeData*)hd.def)->val );
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actExist(token *res, token *var, token *expr)
{
   res->bval = new bdd( bdd_exist(*expr->bval, *var->bval) );
   delete var->bval;
   delete expr->bval;
}


void actForall(token *res, token *var, token *expr)
{
   res->bval = new bdd( bdd_forall(*expr->bval, *var->bval) );
   delete var->bval;
   delete expr->bval;
}


void actQuantVar2(token *res, token *id, token *list)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      if (hd.type == inputTag)
      {
	 res->bval = list->bval;
	 *res->bval &= ((nodeData*)hd.def)->val;
      }
      else
	 yyerror("%s is not a variable", id->id);
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actQuantVar1(token *res, token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      if (hd.type == inputTag)
	 res->bval = new bdd( ((nodeData*)hd.def)->val );
      else
	 yyerror("%s is not a variable", id->id);
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actSize(token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      cout << "Number of nodes used for " << id->id << " = "
	   << bdd_nodecount(((nodeData*)hd.def)->val) << endl;
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actDot(token *fname, token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      if (bdd_fnprintdot(fname->str, ((nodeData*)hd.def)->val) < 0)
	 cout << "Could not open file: " << fname->str << endl;
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actAutoreorder(token *times, token *method)
{
   if (times->ival == 0)
      bdd_autoreorder(method->ival);
   else
      bdd_autoreorder_times(method->ival, times->ival);
}


void actCache(void)
{
   bdd_printstat();
}


void actTautology(token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
   {
      if (((nodeData*)hd.def)->val == bddtrue)
	 cout << "Formula " << id->id << " is a tautology!\n";
      else
	 cout << "Formula " << id->id << " is NOT a tautology!\n";
   }
   else
      yyerror("Unknown variable %s", id->id);
}


void actPrint(token *id)
{
   hashData hd;

   if (names.lookup(id->id,hd) == 0)
      cout << id->id << " = " << bddset << ((nodeData*)hd.def)->val << endl;
   else
      yyerror("Unknown variable %s", id->id);
}

/* EOF */
