/***** ltl2ba : lex.c *****/

/* Written by Denis Oddoux, LIAFA, France                                 */
/* Copyright (c) 2001  Denis Oddoux                                       */
/* Modified by Paul Gastin, LSV, France                                   */
/* Copyright (c) 2007  Paul Gastin                                        */
/*                                                                        */
/* This program is free software; you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation; either version 2 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program; if not, write to the Free Software            */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA*/
/*                                                                        */
/* Based on the translation algorithm by Gastin and Oddoux,               */
/* presented at the 13th International Conference on Computer Aided       */
/* Verification, CAV 2001, Paris, France.                                 */
/* Proceedings - LNCS 2102, pp. 53-65                                     */
/*                                                                        */
/* Send bug-reports and/or questions to Paul Gastin                       */
/* http://www.lsv.ens-cachan.fr/~gastin                                   */
/*                                                                        */
/* Some of the code in this file was taken from the Spin software         */
/* Written by Gerard J. Holzmann, Bell Laboratories, U.S.A.               */

#include <stdlib.h>
#include <ctype.h>
#include "ltl2ba.h"

static Symbol	*symtab[Nhash+1];
static int	tl_lex(void);

extern YYSTYPE	tl_yylval;
char	yytext[2048];

#define Token(y)        tl_yylval = tl_nn(y,ZN,ZN); return y

int
isalnum_(int c)
{       return (isalnum(c) || c == '_');
}

int
hash(char *s)
{       int h=0;

        while (*s)
        {       h += *s++;
                h <<= 1;
                if (h&(Nhash+1))
                        h |= 1;
        }
        return h&Nhash;
}

static void
getword(int first, int (*tst)(int))
{	int i=0; char c;

	yytext[i++]= (char ) first;
	while (tst(c = tl_Getchar()))
		yytext[i++] = c;
	yytext[i] = '\0';
	tl_UnGetchar();
}

static int
follow(int tok, int ifyes, int ifno)
{	int c;
	char buf[32];
	extern int tl_yychar;

	if ((c = tl_Getchar()) == tok)
		return ifyes;
	tl_UnGetchar();
	tl_yychar = c;
	sprintf(buf, "expected '%c'", tok);
	tl_yyerror(buf);	/* no return from here */
	return ifno;
}

int
tl_yylex(void)
{	int c = tl_lex();
#if 0
	printf("c = %d\n", c);
#endif
	return c;
}

static int
tl_lex(void)
{	int c;

	do {
		c = tl_Getchar();
		yytext[0] = (char ) c;
		yytext[1] = '\0';

		if (c <= 0)
		{	Token(';');
		}

	} while (c == ' ');	/* '\t' is removed in tl_main.c */

	if (islower(c))
	{	getword(c, isalnum_);
		if (strcmp("true", yytext) == 0)
		{	Token(TRUE);
		}
		if (strcmp("false", yytext) == 0)
		{	Token(FALSE);
		}
		tl_yylval = tl_nn(PREDICATE,ZN,ZN);
		tl_yylval->sym = tl_lookup(yytext);
		return PREDICATE;
	}
	if (c == '<')
	{	c = tl_Getchar();
		if (c == '>')
		{	Token(EVENTUALLY);
		}
		if (c != '-')
		{	tl_UnGetchar();
			tl_yyerror("expected '<>' or '<->'");
		}
		c = tl_Getchar();
		if (c == '>')
		{	Token(EQUIV);
		}
		tl_UnGetchar();
		tl_yyerror("expected '<->'");
	}

	switch (c) {
	case '/' : c = follow('\\', AND, '/'); break;
	case '\\': c = follow('/', OR, '\\'); break;
	case '&' : c = follow('&', AND, '&'); break;
	case '|' : c = follow('|', OR, '|'); break;
	case '[' : c = follow(']', ALWAYS, '['); break;
	case '-' : c = follow('>', IMPLIES, '-'); break;
	case '!' : c = NOT; break;
	case 'U' : c = U_OPER; break;
	case 'V' : c = V_OPER; break;
#ifdef NXT
	case 'X' : c = NEXT; break;
#endif
	default  : break;
	}
	Token(c);
}

Symbol *
tl_lookup(char *s)
{	Symbol *sp;
	int h = hash(s);

	for (sp = symtab[h]; sp; sp = sp->next)
		if (strcmp(sp->name, s) == 0)
			return sp;

	sp = (Symbol *) tl_emalloc(sizeof(Symbol));
	sp->name = (char *) tl_emalloc(strlen(s) + 1);
	strcpy(sp->name, s);
	sp->next = symtab[h];
	symtab[h] = sp;

	return sp;
}

Symbol *
getsym(Symbol *s)
{	Symbol *n = (Symbol *) tl_emalloc(sizeof(Symbol));

	n->name = s->name;
	return n;
}
