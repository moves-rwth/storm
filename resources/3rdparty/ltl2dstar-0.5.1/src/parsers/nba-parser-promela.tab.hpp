/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PROMELA_AP = 258,
     PROMELA_OR = 259,
     PROMELA_AND = 260,
     PROMELA_FALSE = 261,
     PROMELA_TRUE = 262,
     PROMELA_NOT = 263,
     PROMELA_NEVER = 264,
     PROMELA_IF = 265,
     PROMELA_FI = 266,
     PROMELA_GOTO = 267,
     PROMELA_SKIP = 268,
     PROMELA_LABEL = 269,
     PROMELA_COLON = 270,
     PROMELA_SEMICOLON = 271,
     PROMELA_DOUBLE_COLON = 272,
     PROMELA_LBRACE = 273,
     PROMELA_RBRACE = 274,
     PROMELA_LPAREN = 275,
     PROMELA_RPAREN = 276,
     PROMELA_RIGHT_ARROW = 277
   };
#endif
#define PROMELA_AP 258
#define PROMELA_OR 259
#define PROMELA_AND 260
#define PROMELA_FALSE 261
#define PROMELA_TRUE 262
#define PROMELA_NOT 263
#define PROMELA_NEVER 264
#define PROMELA_IF 265
#define PROMELA_FI 266
#define PROMELA_GOTO 267
#define PROMELA_SKIP 268
#define PROMELA_LABEL 269
#define PROMELA_COLON 270
#define PROMELA_SEMICOLON 271
#define PROMELA_DOUBLE_COLON 272
#define PROMELA_LBRACE 273
#define PROMELA_RBRACE 274
#define PROMELA_LPAREN 275
#define PROMELA_RPAREN 276
#define PROMELA_RIGHT_ARROW 277




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE promela_lval;



