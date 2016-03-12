/* draft.h */

/* (reserved for copyright notice) */

#ifndef DRAFT_H
#define DRAFT_H

#include "glpk.h"

#if 1 /* 28/XI-2009 */
int _glp_analyze_row(glp_prob *P, int len, const int ind[],
      const double val[], int type, double rhs, double eps, int *_piv,
      double *_x, double *_dx, double *_y, double *_dy, double *_dz);
/* simulate one iteration of dual simplex method */
#endif

#if 1 /* 08/XII-2009 */
void _glp_mpl_init_rand(glp_tran *tran, int seed);
#endif

#define glp_skpgen _glp_skpgen
void glp_skpgen(int n, int r, int type, int v, int s, int a[],
   int *b, int c[]);
/* Pisinger's 0-1 single knapsack problem generator */

#if 1 /* 28/V-2010 */
int _glp_intopt1(glp_prob *P, const glp_iocp *parm);
#endif

#endif

/* eof */
