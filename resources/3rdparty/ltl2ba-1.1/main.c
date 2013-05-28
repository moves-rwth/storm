/***** ltl2ba : main.c *****/

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

#include "ltl2ba.h"

FILE	*tl_out;

int	tl_stats     = 0; /* time and size stats */	
int tl_simp_log  = 1; /* logical simplification */
int tl_simp_diff = 1; /* automata simplification */
int tl_simp_fly  = 1; /* on the fly simplification */
int tl_simp_scc  = 1; /* use scc simplification */
int tl_fjtofj    = 1; /* 2eme fj */
int	tl_errs      = 0;
int	tl_verbose   = 0;
int	tl_terse     = 0;
unsigned long	All_Mem = 0;

static char	uform[4096];
static int	hasuform=0, cnt=0;
static char     **ltl_file = (char **)0;
static char     **add_ltl  = (char **)0;
static char     out1[64];

static void	tl_endstats(void);
static void	non_fatal(char *, char *);

void
alldone(int estatus)
{
        if (strlen(out1) > 0)
                (void) unlink((const char *)out1);
        exit(estatus);
}

FILE *
cpyfile(char *src, char *tgt)
{       FILE *inp, *out;
        char buf[1024];

        inp = fopen(src, "r");
        out = fopen(tgt, "w");
        if (!inp || !out)
        {       printf("ltl2ba: cannot cp %s to %s\n", src, tgt);
                alldone(1);
        }
        while (fgets(buf, 1024, inp))
                fprintf(out, "%s", buf);
        fclose(inp);
        return out;
}

char *
emalloc(int n)
{       char *tmp;

        if (!(tmp = (char *) malloc(n)))
                fatal("not enough memory", (char *)0);
        memset(tmp, 0, n);
        return tmp;
}

int
tl_Getchar(void)
{
	if (cnt < hasuform)
		return uform[cnt++];
	cnt++;
	return -1;
}

void
put_uform(void)
{
	fprintf(tl_out, "%s", uform);
}

void
tl_UnGetchar(void)
{
	if (cnt > 0) cnt--;
}

void
usage(void)
{
        printf("usage: ltl2ba [-flag] -f formula\n");
        printf("                   or -F file\n");
        printf(" -f \"formula\"\ttranslate LTL ");
        printf("into never claim\n");
        printf(" -F file\tlike -f, but with the LTL ");
        printf("formula stored in a 1-line file\n");
        printf(" -d\t\tdisplay automata (D)escription at each step\n");
        printf(" -s\t\tcomputing time and automata sizes (S)tatistics\n");
        printf(" -l\t\tdisable (L)ogic formula simplification\n");
        printf(" -p\t\tdisable a-(P)osteriori simplification\n");
        printf(" -o\t\tdisable (O)n-the-fly simplification\n");
        printf(" -c\t\tdisable strongly (C)onnected components simplification\n");
        printf(" -a\t\tdisable trick in (A)ccepting conditions\n");
	
        alldone(1);
}

int
tl_main(int argc, char *argv[])
{       int i;
	while (argc > 1 && argv[1][0] == '-')
	{	switch (argv[1][1]) {
		case 'f':	argc--; argv++;
				for (i = 0; i < argv[1][i]; i++)
				{	if (argv[1][i] == '\t'
					||  argv[1][i] == '\"'
					||  argv[1][i] == '\n')
						argv[1][i] = ' ';
				}
				strcpy(uform, argv[1]);
				hasuform = strlen(uform);
				break;
		default :	usage();
		}
		argc--; argv++;
	}
	if (hasuform == 0) usage();
	tl_parse();
	if (tl_stats) tl_endstats();
	return tl_errs;
}

int
main(int argc, char *argv[])
{	int i;
	tl_out = stdout;

	while (argc > 1 && argv[1][0] == '-')
        {       switch (argv[1][1]) {
                case 'F': ltl_file = (char **) (argv+2);
                          argc--; argv++; break;
                case 'f': add_ltl = (char **) argv;
                          argc--; argv++; break;
                case 'a': tl_fjtofj = 0; break;
                case 'c': tl_simp_scc = 0; break;
                case 'o': tl_simp_fly = 0; break;
                case 'p': tl_simp_diff = 0; break;
                case 'l': tl_simp_log = 0; break;
                case 'd': tl_verbose = 1; break;
                case 's': tl_stats = 1; break;
                default : usage(); break;
                }
                argc--, argv++;
        }

	if(!ltl_file && !add_ltl) usage();

        if (ltl_file)
        {       char formula[4096];
                add_ltl = ltl_file-2; add_ltl[1][1] = 'f';
                if (!(tl_out = fopen(*ltl_file, "r")))
                {       printf("ltl2ba: cannot open %s\n", *ltl_file);
                        alldone(1);
                }
                fgets(formula, 4096, tl_out);
                fclose(tl_out);
                tl_out = stdout;
                *ltl_file = (char *) formula;
        }
        if (argc > 1)
        {       char cmd[128], out2[64];
                strcpy(out1, "_tmp1_");
                strcpy(out2, "_tmp2_");
                tl_out = cpyfile(argv[1], out2);
                tl_main(2, add_ltl);  
                fclose(tl_out);
        } else 
	{
                if (argc > 0)
                        exit(tl_main(2, add_ltl));
		usage();
	}
}

/* Subtract the `struct timeval' values X and Y, storing the result X-Y in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */
 
int
timeval_subtract (result, x, y)
struct timeval *result, *x, *y;
{
	if (x->tv_usec < y->tv_usec) {
		x->tv_usec += 1000000;
		x->tv_sec--;
	}
	
	/* Compute the time remaining to wait. tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;
	
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

static void
tl_endstats(void)
{	extern int Stack_mx;
	printf("\ntotal memory used: %9ld\n", All_Mem);
	/*printf("largest stack sze: %9d\n", Stack_mx);*/
	/*cache_stats();*/
	a_stats();
}

#define Binop(a)		\
		fprintf(tl_out, "(");	\
		dump(n->lft);		\
		fprintf(tl_out, a);	\
		dump(n->rgt);		\
		fprintf(tl_out, ")")

void
dump(Node *n)
{
	if (!n) return;

	switch(n->ntyp) {
	case OR:	Binop(" || "); break;
	case AND:	Binop(" && "); break;
	case U_OPER:	Binop(" U ");  break;
	case V_OPER:	Binop(" V ");  break;
#ifdef NXT
	case NEXT:
		fprintf(tl_out, "X");
		fprintf(tl_out, " (");
		dump(n->lft);
		fprintf(tl_out, ")");
		break;
#endif
	case NOT:
		fprintf(tl_out, "!");
		fprintf(tl_out, " (");
		dump(n->lft);
		fprintf(tl_out, ")");
		break;
	case FALSE:
		fprintf(tl_out, "false");
		break;
	case TRUE:
		fprintf(tl_out, "true");
		break;
	case PREDICATE:
		fprintf(tl_out, "(%s)", n->sym->name);
		break;
	case -1:
		fprintf(tl_out, " D ");
		break;
	default:
		printf("Unknown token: ");
		tl_explain(n->ntyp);
		break;
	}
}

void
tl_explain(int n)
{
	switch (n) {
	case ALWAYS:	printf("[]"); break;
	case EVENTUALLY: printf("<>"); break;
	case IMPLIES:	printf("->"); break;
	case EQUIV:	printf("<->"); break;
	case PREDICATE:	printf("predicate"); break;
	case OR:	printf("||"); break;
	case AND:	printf("&&"); break;
	case NOT:	printf("!"); break;
	case U_OPER:	printf("U"); break;
	case V_OPER:	printf("V"); break;
#ifdef NXT
	case NEXT:	printf("X"); break;
#endif
	case TRUE:	printf("true"); break;
	case FALSE:	printf("false"); break;
	case ';':	printf("end of formula"); break;
	default:	printf("%c", n); break;
	}
}

static void
non_fatal(char *s1, char *s2)
{	extern int tl_yychar;
	int i;

	printf("ltl2ba: ");
	if (s2)
		printf(s1, s2);
	else
		printf(s1);
	if (tl_yychar != -1 && tl_yychar != 0)
	{	printf(", saw '");
		tl_explain(tl_yychar);
		printf("'");
	}
	printf("\nltl2ba: %s\n---------", uform);
	for (i = 0; i < cnt; i++)
		printf("-");
	printf("^\n");
	fflush(stdout);
	tl_errs++;
}

void
tl_yyerror(char *s1)
{
	Fatal(s1, (char *) 0);
}

void
Fatal(char *s1, char *s2)
{
  non_fatal(s1, s2);
  alldone(1);
}

void
fatal(char *s1, char *s2)
{
        non_fatal(s1, s2);
        alldone(1);
}


