/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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
