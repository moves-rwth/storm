/* spydual.c */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2015 Andrew Makhorin, Department for Applied
*  Informatics, Moscow Aviation Institute, Moscow, Russia. All rights
*  reserved. E-mail: <mao@gnu.org>.
*
*  GLPK is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GLPK is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GLPK. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include "env.h"
#include "simplex.h"
#include "spxat.h"
#include "spxnt.h"
#include "spxprob.h"
#include "spychuzc.h"
#include "spychuzr.h"

#define USE_AT 1
/* 1 - use A in row-wise format
 * 0 - use N in row-wise format */
/* (Looks like using A' provide more accuracy for dual simplex.) */

#define EXCL 1
/* 1 - exclude fixed non-basic variables
 * 0 - don't exclude variables */

#define SHIFT 1
/* 1 - shift bounds of variables toward zero
 * 0 - don't shift bounds of variables */

#define CHECK_ACCURACY 0
/* (for debugging) */

struct csa
{     /* common storage area */
      SPXLP *lp;
      /* LP problem data and its (current) basis; this LP has m rows
       * and n columns */
      int dir;
      /* original optimization direction:
       * +1 - minimization
       * -1 - maximization */
      double *b; /* double b[1+m]; */
      /* copy of original right-hand sides */
      double *l; /* double l[1+n]; */
      /* copy of original lower bounds */
      double *u; /* double u[1+n]; */
      /* copy of original upper bounds */
      SPXAT *at;
      /* mxn-matrix A of constraint coefficients, in sparse row-wise
       * format (NULL if not used) */
      SPXNT *nt;
      /* mx(n-m)-matrix N composed of non-basic columns of constraint
       * matrix A, in sparse row-wise format (NULL if not used) */
      int phase;
      /* search phase:
       * 0 - not determined yet
       * 1 - searching for dual feasible solution
       * 2 - searching for optimal solution */
      double *beta; /* double beta[1+m]; */
      /* beta[i] is primal value of basic variable xB[i] */
      int beta_st;
      /* status of the vector beta:
       * 0 - undefined
       * 1 - just computed
       * 2 - updated */
      double *d; /* double d[1+n-m]; */
      /* d[j] is reduced cost of non-basic variable xN[j] */
      int d_st;
      /* status of the vector d:
       * 0 - undefined
       * 1 - just computed
       * 2 - updated */
      SPYSE *se;
      /* dual projected steepest edge and Devex pricing data block
       * (NULL if not used) */
      int num;
      /* number of eligible basic variables */
      int *list; /* int list[1+m]; */
      /* list[1], ..., list[num] are indices i of eligible basic
       * variables xB[i] */
      int p;
      /* xB[p] is a basic variable chosen to leave the basis */
      double *trow; /* double trow[1+n-m]; */
      /* p-th (pivot) row of the simplex table */
      int q;
      /* xN[q] is a non-basic variable chosen to enter the basis */
      double *tcol; /* double tcol[1+m]; */
      /* q-th (pivot) column of the simplex table */
      double *work; /* double work[1+m]; */
      /* working array */
      double *work1; /* double work1[1+n-m]; */
      /* another working array */
      int p_stat, d_stat;
      /* primal and dual solution statuses */
      /*--------------------------------------------------------------*/
      /* control parameters (see struct glp_smcp) */
      int msg_lev;
      /* message level */
      int dualp;
      /* if this flag is set, report failure in case of instability */
      int harris;
      /* dual ratio test technique:
       * 0 - textbook ratio test
       * 1 - Harris' two pass ratio test */
      double tol_bnd, tol_bnd1;
      /* primal feasibility tolerances */
      double tol_dj, tol_dj1;
      /* dual feasibility tolerances */
      double tol_piv;
      /* pivot tolerance */
      double obj_lim;
      /* objective limit */
      int it_lim;
      /* iteration limit */
      int tm_lim;
      /* time limit, milliseconds */
      int out_frq;
      /* display output frequency, iterations */
      int out_dly;
      /* display output delay, milliseconds */
      /*--------------------------------------------------------------*/
      /* working parameters */
      double tm_beg;
      /* time value at the beginning of the search */
      int it_beg;
      /* simplex iteration count at the beginning of the search */
      int it_cnt;
      /* simplex iteration count; it increases by one every time the
       * basis changes */
      int it_dpy;
      /* simplex iteration count at most recent display output */
      int inv_cnt;
      /* basis factorization count since most recent display output */
};

/***********************************************************************
*  check_flags - check correctness of active bound flags
*
*  This routine checks that flags specifying active bounds of all
*  non-basic variables are correct.
*
*  NOTE: It is important to note that if bounds of variables have been
*  changed, active bound flags should be corrected accordingly. */

static void check_flags(struct csa *csa)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      char *flag = lp->flag;
      int j, k;
      for (j = 1; j <= n-m; j++)
      {  k = head[m+j]; /* x[k] = xN[j] */
         if (l[k] == -DBL_MAX && u[k] == +DBL_MAX)
            xassert(!flag[j]);
         else if (l[k] != -DBL_MAX && u[k] == +DBL_MAX)
            xassert(!flag[j]);
         else if (l[k] == -DBL_MAX && u[k] != +DBL_MAX)
            xassert(flag[j]);
         else if (l[k] == u[k])
            xassert(!flag[j]);
      }
      return;
}

/***********************************************************************
*  set_art_bounds - set artificial right-hand sides and bounds
*
*  This routine sets artificial right-hand sides and artificial bounds
*  for all variables to minimize the sum of dual infeasibilities on
*  phase I. Given current reduced costs d = (d[j]) this routine also
*  sets active artificial bounds of non-basic variables to provide dual
*  feasibility (this is always possible because all variables have both
*  lower and upper artificial bounds). */

static void set_art_bounds(struct csa *csa)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *b = lp->b;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      char *flag = lp->flag;
      double *d = csa->d;
      int i, j, k;
      /* set artificial right-hand sides */
      for (i = 1; i <= m; i++)
         b[i] = 0.0;
      /* set artificial bounds depending on types of variables */
      for (k = 1; k <= n; k++)
      {  if (csa->l[k] == -DBL_MAX && csa->u[k] == +DBL_MAX)
         {  /* force free variables to enter the basis */
            l[k] = -1e3, u[k] = +1e3;
         }
         else if (csa->l[k] != -DBL_MAX && csa->u[k] == +DBL_MAX)
            l[k] = 0.0, u[k] = +1.0;
         else if (csa->l[k] == -DBL_MAX && csa->u[k] != +DBL_MAX)
            l[k] = -1.0, u[k] = 0.0;
         else
            l[k] = u[k] = 0.0;
      }
      /* set active artificial bounds for non-basic variables */
      xassert(csa->d_st == 1);
      for (j = 1; j <= n-m; j++)
      {  k = head[m+j]; /* x[k] = xN[j] */
         flag[j] = (l[k] != u[k] && d[j] < 0.0);
      }
      /* invalidate values of basic variables, since active bounds of
       * non-basic variables have been changed */
      csa->beta_st = 0;
      return;
}

/***********************************************************************
*  set_orig_bounds - restore original right-hand sides and bounds
*
*  This routine restores original right-hand sides and original bounds
*  for all variables. This routine also sets active original bounds for
*  non-basic variables; for double-bounded non-basic variables current
*  reduced costs d = (d[j]) are used to decide which bound (lower or
*  upper) should be made active. */

void set_orig_bounds(struct csa *csa)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *b = lp->b;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      char *flag = lp->flag;
      double *d = csa->d;
      int j, k;
      /* restore original right-hand sides */
      memcpy(b, csa->b, (1+m) * sizeof(double));
      /* restore original bounds of all variables */
      memcpy(l, csa->l, (1+n) * sizeof(double));
      memcpy(u, csa->u, (1+n) * sizeof(double));
      /* set active original bounds for non-basic variables */
      xassert(csa->d_st == 1);
      for (j = 1; j <= n-m; j++)
      {  k = head[m+j]; /* x[k] = xN[j] */
         if (l[k] == -DBL_MAX && u[k] == +DBL_MAX)
            flag[j] = 0;
         else if (l[k] != -DBL_MAX && u[k] == +DBL_MAX)
            flag[j] = 0;
         else if (l[k] == -DBL_MAX && u[k] != +DBL_MAX)
            flag[j] = 1;
         else if (l[k] != u[k])
            flag[j] = (d[j] < 0.0);
         else
            flag[j] = 0;
      }
      /* invalidate values of basic variables, since active bounds of
       * non-basic variables have been changed */
      csa->beta_st = 0;
      return;
}

/***********************************************************************
*  check_feas - check dual feasibility of basic solution
*
*  This routine checks that reduced costs of all non-basic variables
*  d = (d[j]) have correct signs.
*
*  Reduced cost d[j] is considered as having correct sign within the
*  specified tolerance depending on status of non-basic variable xN[j]
*  if one of the following conditions is met:
*
*     xN[j] is free                       -eps <= d[j] <= +eps
*
*     xN[j] has its lower bound active    d[j] >= -eps
*
*     xN[j] has its upper bound active    d[j] <= +eps
*
*     xN[j] is fixed                      d[j] has any value
*
*  where eps = tol + tol1 * |cN[j]|, cN[j] is the objective coefficient
*  at xN[j]. (See also the routine spx_chuzc_sel.)
*
*  The flag recov allows the routine to recover dual feasibility by
*  changing active bounds of non-basic variables. (For example, if
*  xN[j] has its lower bound active and d[j] < -eps, the feasibility
*  can be recovered by making xN[j] active on its upper bound.)
*
*  If the basic solution is dual feasible, the routine returns zero.
*  If the basic solution is dual infeasible, but its dual feasibility
*  can be recovered (or has been recovered, if the flag recov is set),
*  the routine returns a negative value. Otherwise, the routine returns
*  the number j of some non-basic variable xN[j], whose reduced cost
*  d[j] is dual infeasible and cannot be recovered. */

static int check_feas(struct csa *csa, double tol, double tol1,
      int recov)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *c = lp->c;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      char *flag = lp->flag;
      double *d = csa->d;
      int j, k, ret = 0;
      double eps;
      /* reduced costs should be just computed */
      xassert(csa->d_st == 1);
      /* walk thru list of non-basic variables */
      for (j = 1; j <= n-m; j++)
      {  k = head[m+j]; /* x[k] = xN[j] */
         if (l[k] == u[k])
         {  /* xN[j] is fixed variable; skip it */
            continue;
         }
         /* determine absolute tolerance eps[j] */
         eps = tol + tol1 * (c[k] >= 0.0 ? +c[k] : -c[k]);
         /* check dual feasibility of xN[j] */
         if (d[j] > +eps)
         {  /* xN[j] should have its lower bound active */
            if (l[k] == -DBL_MAX || flag[j])
            {  /* but it either has no lower bound or its lower bound
                * is inactive */
               if (l[k] == -DBL_MAX)
               {  /* cannot recover, since xN[j] has no lower bound */
                  ret = j;
                  break;
               }
               /* recovering is possible */
               if (recov)
                  flag[j] = 0;
               ret = -1;
            }
         }
         else if (d[j] < -eps)
         {  /* xN[j] should have its upper bound active */
            if (!flag[j])
            {  /* but it either has no upper bound or its upper bound
                * is inactive */
               if (u[k] == +DBL_MAX)
               {  /* cannot recover, since xN[j] has no upper bound */
                  ret = j;
                  break;
               }
               /* recovering is possible */
               if (recov)
                  flag[j] = 1;
               ret = -1;
            }
         }
      }
      if (recov && ret)
      {  /* invalidate values of basic variables, since active bounds
          * of non-basic variables have been changed */
         csa->beta_st = 0;
      }
      return ret;
}

#if CHECK_ACCURACY
/***********************************************************************
*  err_in_vec - compute maximal relative error between two vectors
*
*  This routine computes and returns maximal relative error between
*  n-vectors x and y:
*
*     err_max = max |x[i] - y[i]| / (1 + |x[i]|).
*
*  NOTE: This routine is intended only for debugginig purposes. */

static double err_in_vec(int n, const double x[], const double y[])
{     int i;
      double err, err_max;
      err_max = 0.0;
      for (i = 1; i <= n; i++)
      {  err = fabs(x[i] - y[i]) / (1.0 + fabs(x[i]));
         if (err_max < err)
            err_max = err;
      }
      return err_max;
}
#endif

#if CHECK_ACCURACY
/***********************************************************************
*  err_in_beta - compute maximal relative error in vector beta
*
*  This routine computes and returns maximal relative error in vector
*  of values of basic variables beta = (beta[i]).
*
*  NOTE: This routine is intended only for debugginig purposes. */

static double err_in_beta(struct csa *csa)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      double err, *beta;
      beta = talloc(1+m, double);
      spx_eval_beta(lp, beta);
      err = err_in_vec(m, beta, csa->beta);
      tfree(beta);
      return err;
}
#endif

#if CHECK_ACCURACY
/***********************************************************************
*  err_in_d - compute maximal relative error in vector d
*
*  This routine computes and returns maximal relative error in vector
*  of reduced costs of non-basic variables d = (d[j]).
*
*  NOTE: This routine is intended only for debugginig purposes. */

static double err_in_d(struct csa *csa)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      int j;
      double err, *pi, *d;
      pi = talloc(1+m, double);
      d = talloc(1+n-m, double);
      spx_eval_pi(lp, pi);
      for (j = 1; j <= n-m; j++)
         d[j] = spx_eval_dj(lp, pi, j);
      err = err_in_vec(n-m, d, csa->d);
      tfree(pi);
      tfree(d);
      return err;
}
#endif

#if CHECK_ACCURACY
/***********************************************************************
*  err_in_gamma - compute maximal relative error in vector gamma
*
*  This routine computes and returns maximal relative error in vector
*  of projected steepest edge weights gamma = (gamma[j]).
*
*  NOTE: This routine is intended only for debugginig purposes. */

static double err_in_gamma(struct csa *csa)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      SPYSE *se = csa->se;
      int i;
      double err, *gamma;
      xassert(se != NULL);
gamma = talloc(1+m, double);
      for (i = 1; i <= m; i++)
         gamma[i] = spy_eval_gamma_i(lp, se, i);
      err = err_in_vec(m, gamma, se->gamma);
      tfree(gamma);
      return err;
}
#endif

#if CHECK_ACCURACY
/***********************************************************************
*  check_accuracy - check accuracy of basic solution components
*
*  This routine checks accuracy of current basic solution components.
*
*  NOTE: This routine is intended only for debugginig purposes. */

static void check_accuracy(struct csa *csa)
{     double e_beta, e_d, e_gamma;
      e_beta = err_in_beta(csa);
      e_d = err_in_d(csa);
      if (csa->se == NULL)
         e_gamma = 0.;
      else
         e_gamma = err_in_gamma(csa);
      xprintf("e_beta = %10.3e; e_d = %10.3e; e_gamma = %10.3e\n",
         e_beta, e_d, e_gamma);
      xassert(e_beta <= 1e-5 && e_d <= 1e-5 && e_gamma <= 1e-3);
      return;
}
#endif

/***********************************************************************
*  choose_pivot - choose xB[p] and xN[q]
*
*  Given the list of eligible basic variables this routine first
*  chooses basic variable xB[p]. This choice is always possible,
*  because the list is assumed to be non-empty. Then the routine
*  computes p-th row T[p,*] of the simplex table T[i,j] and chooses
*  non-basic variable xN[q]. If the pivot T[p,q] is small in magnitude,
*  the routine attempts to choose another xB[p] and xN[q] in order to
*  avoid badly conditioned adjacent bases. */

static void choose_pivot(struct csa *csa)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *l = lp->l;
      int *head = lp->head;
      SPXAT *at = csa->at;
      SPXNT *nt = csa->nt;
      double *beta = csa->beta;
      double *d = csa->d;
      SPYSE *se = csa->se;
      int *list = csa->list;
      double *rho = csa->work;
      double *trow = csa->work1;
      int nnn, try, k, p, q, t;
      xassert(csa->beta_st);
      xassert(csa->d_st);
      /* initial number of eligible basic variables */
      nnn = csa->num;
      /* nothing has been chosen so far */
      csa->p = 0;
      try = 0;
try:  /* choose basic variable xB[p] */
      xassert(nnn > 0);
      try++;
      if (se == NULL)
      {  /* dual Dantzig's rule */
         p = spy_chuzr_std(lp, beta, nnn, list);
      }
      else
      {  /* dual projected steepest edge */
         p = spy_chuzr_pse(lp, se, beta, nnn, list);
      }
      xassert(1 <= p && p <= m);
      /* compute p-th row of inv(B) */
      spx_eval_rho(lp, p, rho);
      /* compute p-th row of the simplex table */
      if (at != NULL)
         spx_eval_trow1(lp, at, rho, trow);
      else
         spx_nt_prod(lp, nt, trow, 1, -1.0, rho);
      /* choose non-basic variable xN[q] */
      k = head[p]; /* x[k] = xB[p] */
      if (!csa->harris)
         q = spy_chuzc_std(lp, d, beta[p] < l[k] ? +1. : -1., trow,
            csa->tol_piv, .30 * csa->tol_dj, .30 * csa->tol_dj1);
      else
         q = spy_chuzc_harris(lp, d, beta[p] < l[k] ? +1. : -1., trow,
            csa->tol_piv, .35 * csa->tol_dj, .35 * csa->tol_dj1);
      /* either keep previous choice or accept new choice depending on
       * which one is better */
      if (csa->p == 0 || q == 0 ||
         fabs(trow[q]) > fabs(csa->trow[csa->q]))
      {  csa->p = p;
         memcpy(&csa->trow[1], &trow[1], (n-m) * sizeof(double));
         csa->q = q;
      }
      /* check if current choice is acceptable */
      if (csa->q == 0 || fabs(csa->trow[csa->q]) >= 0.001)
         goto done;
      if (nnn == 1)
         goto done;
      if (try == 5)
         goto done;
      /* try to choose other xB[p] and xN[q] */
      /* find xB[p] in the list */
      for (t = 1; t <= nnn; t++)
         if (list[t] == p) break;
      xassert(t <= nnn);
      /* move xB[p] to the end of the list */
      list[t] = list[nnn], list[nnn] = p;
      /* and exclude it from consideration */
      nnn--;
      /* repeat the choice */
      goto try;
done: /* the choice has been made */
      return;
}

/***********************************************************************
*  display - display search progress
*
*  This routine displays some information about the search progress
*  that includes:
*
*  search phase;
*
*  number of simplex iterations performed by the solver;
*
*  original objective value (only on phase II);
*
*  sum of (scaled) dual infeasibilities for original bounds;
*
*  number of dual infeasibilities (phase I) or primal infeasibilities
*  (phase II);
*
*  number of basic factorizations since last display output. */

static void display(struct csa *csa, int spec)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      int *head = lp->head;
      char *flag = lp->flag;
      double *l = csa->l; /* original lower bounds */
      double *u = csa->u; /* original upper bounds */
      double *beta = csa->beta;
      double *d = csa->d;
      int j, k, nnn;
      double sum;
      /* check if the display output should be skipped */
      if (csa->msg_lev < GLP_MSG_ON) goto skip;
      if (csa->out_dly > 0 &&
         1000.0 * xdifftime(xtime(), csa->tm_beg) < csa->out_dly)
         goto skip;
      if (csa->it_cnt == csa->it_dpy) goto skip;
      if (!spec && csa->it_cnt % csa->out_frq != 0) goto skip;
      /* display search progress depending on search phase */
      switch (csa->phase)
      {  case 1:
            /* compute sum and number of (scaled) dual infeasibilities
             * for original bounds */
            sum = 0.0, nnn = 0;
            for (j = 1; j <= n-m; j++)
            {  k = head[m+j]; /* x[k] = xN[j] */
               if (d[j] > 0.0)
               {  /* xN[j] should have lower bound */
                  if (l[k] == -DBL_MAX)
                  {  sum += d[j];
                     if (d[j] > +1e-7)
                        nnn++;
                  }
               }
               else if (d[j] < 0.0)
               {  /* xN[j] should have upper bound */
                  if (u[k] == +DBL_MAX)
                  {  sum -= d[j];
                     if (d[j] < -1e-7)
                        nnn++;
                  }
               }
            }
            /* on phase I variables have artificial bounds which are
             * meaningless for original LP, so corresponding objective
             * function value is also meaningless */
            xprintf(" %6d: %23s inf = %11.3e (%d)",
               csa->it_cnt, "", sum, nnn);
            break;
         case 2:
            /* compute sum of (scaled) dual infeasibilities */
            sum = 0.0, nnn = 0;
            for (j = 1; j <= n-m; j++)
            {  k = head[m+j]; /* x[k] = xN[j] */
               if (d[j] > 0.0)
               {  /* xN[j] should have its lower bound active */
                  if (l[k] == -DBL_MAX || flag[j])
                     sum += d[j];
               }
               else if (d[j] < 0.0)
               {  /* xN[j] should have its upper bound active */
                  if (l[k] != u[k] && !flag[j])
                     sum -= d[j];
               }
            }
            /* compute number of primal infeasibilities */
            nnn = spy_chuzr_sel(lp, beta, csa->tol_bnd, csa->tol_bnd1,
               NULL);
            xprintf("#%6d: obj = %17.9e inf = %11.3e (%d)",
               csa->it_cnt, (double)csa->dir * spx_eval_obj(lp, beta),
               sum, nnn);
            break;
         default:
            xassert(csa != csa);
      }
      if (csa->inv_cnt)
      {  /* number of basis factorizations performed */
         xprintf(" %d", csa->inv_cnt);
         csa->inv_cnt = 0;
      }
      xprintf("\n");
      csa->it_dpy = csa->it_cnt;
skip: return;
}

/***********************************************************************
*  spy_dual - driver to dual simplex method
*
*  This routine is a driver to the two-phase dual simplex method.
*
*  On exit this routine returns one of the following codes:
*
*  0  LP instance has been successfully solved.
*
*  GLP_EOBJLL
*     Objective lower limit has been reached (maximization).
*
*  GLP_EOBJUL
*     Objective upper limit has been reached (minimization).
*
*  GLP_EITLIM
*     Iteration limit has been exhausted.
*
*  GLP_ETMLIM
*     Time limit has been exhausted.
*
*  GLP_EFAIL
*     The solver failed to solve LP instance. */

static int dual_simplex(struct csa *csa)
{     /* dual simplex method main logic routine */
      SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      SPXNT *nt = csa->nt;
      double *beta = csa->beta;
      double *d = csa->d;
      SPYSE *se = csa->se;
      int *list = csa->list;
      double *trow = csa->trow;
      double *tcol = csa->tcol;
      double *pi = csa->work;
      int msg_lev = csa->msg_lev;
      double tol_bnd = csa->tol_bnd;
      double tol_bnd1 = csa->tol_bnd1;
      double tol_dj = csa->tol_dj;
      double tol_dj1 = csa->tol_dj1;
      int j, k, p_flag, refct, ret;
      check_flags(csa);
loop: /* main loop starts here */
      /* compute factorization of the basis matrix */
      if (!lp->valid)
      {  double cond;
         ret = spx_factorize(lp);
         csa->inv_cnt++;
         if (ret != 0)
         {  if (msg_lev >= GLP_MSG_ERR)
               xprintf("Error: unable to factorize the basis matrix (%d"
                  ")\n", ret);
            csa->p_stat = csa->d_stat = GLP_UNDEF;
            ret = GLP_EFAIL;
            goto fini;
         }
         /* check condition of the basis matrix */
         cond = bfd_condest(lp->bfd);
         if (cond > 1.0 / DBL_EPSILON)
         {  if (msg_lev >= GLP_MSG_ERR)
               xprintf("Error: basis matrix is singular to working prec"
                  "ision (cond = %.3g)\n", cond);
            csa->p_stat = csa->d_stat = GLP_UNDEF;
            ret = GLP_EFAIL;
            goto fini;
         }
         if (cond > 0.001 / DBL_EPSILON)
         {  if (msg_lev >= GLP_MSG_ERR)
               xprintf("Warning: basis matrix is ill-conditioned (cond "
                  "= %.3g)\n", cond);
         }
         /* invalidate basic solution components */
         csa->beta_st = csa->d_st = 0;
      }
      /* compute reduced costs of non-basic variables d = (d[j]) */
      if (!csa->d_st)
      {  spx_eval_pi(lp, pi);
         for (j = 1; j <= n-m; j++)
            d[j] = spx_eval_dj(lp, pi, j);
         csa->d_st = 1; /* just computed */
         /* determine the search phase, if not determined yet (this is
          * performed only once at the beginning of the search for the
          * original bounds) */
         if (!csa->phase)
         {  j = check_feas(csa, 0.97 * tol_dj, 0.97 * tol_dj1, 1);
            if (j > 0)
            {  /* initial basic solution is dual infeasible and cannot
                * be recovered */
               /* start to search for dual feasible solution */
               set_art_bounds(csa);
               csa->phase = 1;
            }
            else
            {  /* initial basic solution is either dual feasible or its
                * dual feasibility has been recovered */
               /* start to search for optimal solution */
               csa->phase = 2;
            }
         }
         /* make sure that current basic solution is dual feasible */
         j = check_feas(csa, tol_dj, tol_dj1, 0);
         if (j)
         {  /* dual feasibility is broken due to excessive round-off
             * errors */
            if (bfd_get_count(lp->bfd))
            {  /* try to provide more accuracy */
               lp->valid = 0;
               goto loop;
            }
            if (msg_lev >= GLP_MSG_ERR)
               xprintf("Warning: numerical instability (dual simplex, p"
                  "hase %s)\n", csa->phase == 1 ? "I" : "II");
            if (csa->dualp)
            {  /* do not continue the search; report failure */
               csa->p_stat = csa->d_stat = GLP_UNDEF;
               ret = -1; /* special case of GLP_EFAIL */
               goto fini;
            }
            /* try to recover dual feasibility */
            j = check_feas(csa, 0.97 * tol_dj, 0.97 * tol_dj1, 1);
            if (j > 0)
            {  /* dual feasibility cannot be recovered (this may happen
                * only on phase II) */
               xassert(csa->phase == 2);
               /* restart to search for dual feasible solution */
               set_art_bounds(csa);
               csa->phase = 1;
            }
         }
      }
      /* at this point the search phase is determined */
      xassert(csa->phase == 1 || csa->phase == 2);
      /* compute values of basic variables beta = (beta[i]) */
      if (!csa->beta_st)
      {  spx_eval_beta(lp, beta);
         csa->beta_st = 1; /* just computed */
      }
      /* reset the dual reference space, if necessary */
      if (se != NULL && !se->valid)
         spy_reset_refsp(lp, se), refct = 1000;
      /* at this point the basis factorization and all basic solution
       * components are valid */
      xassert(lp->valid && csa->beta_st && csa->d_st);
      check_flags(csa);
#if CHECK_ACCURACY
      /* check accuracy of current basic solution components (only for
       * debugging) */
      check_accuracy(csa);
#endif
      /* check if the objective limit has been reached */
      if (csa->phase == 2 && csa->obj_lim != DBL_MAX
         && spx_eval_obj(lp, beta) >= csa->obj_lim)
      {  if (csa->beta_st != 1)
            csa->beta_st = 0;
         if (csa->d_st != 1)
            csa->d_st = 0;
         if (!(csa->beta_st && csa->d_st))
            goto loop;
         display(csa, 1);
         if (msg_lev >= GLP_MSG_ALL)
            xprintf("OBJECTIVE %s LIMIT REACHED; SEARCH TERMINATED\n",
               csa->dir > 0 ? "UPPER" : "LOWER");
         csa->num = spy_chuzr_sel(lp, beta, tol_bnd, tol_bnd1, list);
         csa->p_stat = (csa->num == 0 ? GLP_FEAS : GLP_INFEAS);
         csa->d_stat = GLP_FEAS;
         ret = (csa->dir > 0 ? GLP_EOBJUL : GLP_EOBJLL);
         goto fini;
      }
      /* check if the iteration limit has been exhausted */
      if (csa->it_cnt - csa->it_beg >= csa->it_lim)
      {  if (csa->beta_st != 1)
            csa->beta_st = 0;
         if (csa->d_st != 1)
            csa->d_st = 0;
         if (!(csa->beta_st && csa->d_st))
            goto loop;
         display(csa, 1);
         if (msg_lev >= GLP_MSG_ALL)
            xprintf("ITERATION LIMIT EXCEEDED; SEARCH TERMINATED\n");
         if (csa->phase == 1)
         {  set_orig_bounds(csa);
            check_flags(csa);
            spx_eval_beta(lp, beta);
         }
         csa->num = spy_chuzr_sel(lp, beta, tol_bnd, tol_bnd1, list);
         csa->p_stat = (csa->num == 0 ? GLP_FEAS : GLP_INFEAS);
         csa->d_stat = (csa->phase == 1 ? GLP_INFEAS : GLP_FEAS);
         ret = GLP_EITLIM;
         goto fini;
      }
      /* check if the time limit has been exhausted */
      if (1000.0 * xdifftime(xtime(), csa->tm_beg) >= csa->tm_lim)
      {  if (csa->beta_st != 1)
            csa->beta_st = 0;
         if (csa->d_st != 1)
            csa->d_st = 0;
         if (!(csa->beta_st && csa->d_st))
            goto loop;
         display(csa, 1);
         if (msg_lev >= GLP_MSG_ALL)
            xprintf("TIME LIMIT EXCEEDED; SEARCH TERMINATED\n");
         if (csa->phase == 1)
         {  set_orig_bounds(csa);
            check_flags(csa);
            spx_eval_beta(lp, beta);
         }
         csa->num = spy_chuzr_sel(lp, beta, tol_bnd, tol_bnd1, list);
         csa->p_stat = (csa->num == 0 ? GLP_FEAS : GLP_INFEAS);
         csa->d_stat = (csa->phase == 1 ? GLP_INFEAS : GLP_FEAS);
         ret = GLP_EITLIM;
         goto fini;
      }
      /* display the search progress */
      display(csa, 0);
      /* select eligible basic variables */
      switch (csa->phase)
      {  case 1:
            csa->num = spy_chuzr_sel(lp, beta, 1e-8, 0.0, list);
            break;
         case 2:
            csa->num = spy_chuzr_sel(lp, beta, tol_bnd, tol_bnd1, list);
            break;
         default:
            xassert(csa != csa);
      }
      /* check for optimality */
      if (csa->num == 0)
      {  if (csa->beta_st != 1)
            csa->beta_st = 0;
         if (csa->d_st != 1)
            csa->d_st = 0;
         if (!(csa->beta_st && csa->d_st))
            goto loop;
         /* current basis is optimal */
         display(csa, 1);
         switch (csa->phase)
         {  case 1:
               /* check for dual feasibility */
               set_orig_bounds(csa);
               check_flags(csa);
               if (check_feas(csa, tol_dj, tol_dj1, 0) == 0)
               {  /* dual feasible solution found; switch to phase II */
                  csa->phase = 2;
                  xassert(!csa->beta_st);
                  goto loop;
               }
               /* no dual feasible solution exists */
               if (msg_lev >= GLP_MSG_ALL)
                  xprintf("LP HAS NO DUAL FEASIBLE SOLUTION\n");
               spx_eval_beta(lp, beta);
               csa->num = spy_chuzr_sel(lp, beta, tol_bnd, tol_bnd1,
                  list);
               csa->p_stat = (csa->num == 0 ? GLP_FEAS : GLP_INFEAS);
               csa->d_stat = GLP_NOFEAS;
               ret = 0;
               goto fini;
            case 2:
               /* optimal solution found */
               if (msg_lev >= GLP_MSG_ALL)
                  xprintf("OPTIMAL LP SOLUTION FOUND\n");
               csa->p_stat = csa->d_stat = GLP_FEAS;
               ret = 0;
               goto fini;
            default:
               xassert(csa != csa);
         }
      }
      /* choose xB[p] and xN[q] */
      choose_pivot(csa);
      /* check for dual unboundedness */
      if (csa->q == 0)
      {  if (csa->beta_st != 1)
            csa->beta_st = 0;
         if (csa->d_st != 1)
            csa->d_st = 0;
         if (!(csa->beta_st && csa->d_st))
            goto loop;
         display(csa, 1);
         switch (csa->phase)
         {  case 1:
               /* this should never happen */
               if (msg_lev >= GLP_MSG_ERR)
                  xprintf("Error: dual simplex failed\n");
               csa->p_stat = csa->d_stat = GLP_UNDEF;
               ret = GLP_EFAIL;
               goto fini;
            case 2:
               /* dual unboundedness detected */
               if (msg_lev >= GLP_MSG_ALL)
                  xprintf("LP HAS NO PRIMAL FEASIBLE SOLUTION\n");
               csa->p_stat = GLP_NOFEAS;
               csa->d_stat = GLP_FEAS;
               ret = 0;
               goto fini;
            default:
               xassert(csa != csa);
         }
      }
      /* compute q-th column of the simplex table */
      spx_eval_tcol(lp, csa->q, tcol);
      /* FIXME: tcol[p] and trow[q] should be close to each other */
      xassert(tcol[csa->p] != 0.0);
      /* update values of basic variables for adjacent basis */
      k = head[csa->p]; /* x[k] = xB[p] */
      p_flag = (l[k] != u[k] && beta[csa->p] > u[k]);
      spx_update_beta(lp, beta, csa->p, p_flag, csa->q, tcol);
      csa->beta_st = 2;
      /* update reduced costs of non-basic variables for adjacent
       * basis */
      if (spx_update_d(lp, d, csa->p, csa->q, trow, tcol) <= 1e-9)
      {  /* successful updating */
         csa->d_st = 2;
      }
      else
      {  /* new reduced costs are inaccurate */
         csa->d_st = 0;
      }
      /* update steepest edge weights for adjacent basis, if used */
      if (se != NULL)
      {  if (refct > 0)
         {  if (spy_update_gamma(lp, se, csa->p, csa->q, trow, tcol)
               <= 1e-3)
            {  /* successful updating */
               refct--;
            }
            else
            {  /* new weights are inaccurate; reset reference space */
               se->valid = 0;
            }
         }
         else
         {  /* too many updates; reset reference space */
            se->valid = 0;
         }
      }
      /* update matrix N for adjacent basis, if used */
      if (nt != NULL)
         spx_update_nt(lp, nt, csa->p, csa->q);
      /* change current basis header to adjacent one */
      spx_change_basis(lp, csa->p, p_flag, csa->q);
      /* and update factorization of the basis matrix */
      if (csa->p > 0)
         spx_update_invb(lp, csa->p, head[csa->p]);
      /* dual simplex iteration complete */
      csa->it_cnt++;
      goto loop;
fini: return ret;
}

int spy_dual(glp_prob *P, const glp_smcp *parm)
{     /* driver to dual simplex method */
      struct csa csa_, *csa = &csa_;
      SPXLP lp;
#if USE_AT
      SPXAT at;
#else
      SPXNT nt;
#endif
      SPYSE se;
      int ret, *map, *daeh;
      /* build working LP and its initial basis */
      memset(csa, 0, sizeof(struct csa));
      csa->lp = &lp;
      spx_init_lp(csa->lp, P, EXCL);
      spx_alloc_lp(csa->lp);
      map = talloc(1+P->m+P->n, int);
      spx_build_lp(csa->lp, P, EXCL, SHIFT, map);
      spx_build_basis(csa->lp, P, map);
      switch (P->dir)
      {  case GLP_MIN:
            csa->dir = +1;
            break;
         case GLP_MAX:
            csa->dir = -1;
            break;
         default:
            xassert(P != P);
      }
      csa->b = talloc(1+csa->lp->m, double);
      memcpy(csa->b, csa->lp->b, (1+csa->lp->m) * sizeof(double));
      csa->l = talloc(1+csa->lp->n, double);
      memcpy(csa->l, csa->lp->l, (1+csa->lp->n) * sizeof(double));
      csa->u = talloc(1+csa->lp->n, double);
      memcpy(csa->u, csa->lp->u, (1+csa->lp->n) * sizeof(double));
#if USE_AT
      /* build matrix A in row-wise format */
      csa->at = &at;
      csa->nt = NULL;
      spx_alloc_at(csa->lp, csa->at);
      spx_build_at(csa->lp, csa->at);
#else
      /* build matrix N in row-wise format for initial basis */
      csa->at = NULL;
      csa->nt = &nt;
      spx_alloc_nt(csa->lp, csa->nt);
      spx_init_nt(csa->lp, csa->nt);
      spx_build_nt(csa->lp, csa->nt);
#endif
      /* allocate and initialize working components */
      csa->phase = 0;
      csa->beta = talloc(1+csa->lp->m, double);
      csa->beta_st = 0;
      csa->d = talloc(1+csa->lp->n-csa->lp->m, double);
      csa->d_st = 0;
      switch (parm->pricing)
      {  case GLP_PT_STD:
            csa->se = NULL;
            break;
         case GLP_PT_PSE:
            csa->se = &se;
            spy_alloc_se(csa->lp, csa->se);
            break;
         default:
            xassert(parm != parm);
      }
      csa->list = talloc(1+csa->lp->m, int);
      csa->trow = talloc(1+csa->lp->n-csa->lp->m, double);
      csa->tcol = talloc(1+csa->lp->m, double);
      csa->work = talloc(1+csa->lp->m, double);
      csa->work1 = talloc(1+csa->lp->n-csa->lp->m, double);
      /* initialize control parameters */
      csa->msg_lev = parm->msg_lev;
      csa->dualp = (parm->meth == GLP_DUALP);
      switch (parm->r_test)
      {  case GLP_RT_STD:
            csa->harris = 0;
            break;
         case GLP_RT_HAR:
            csa->harris = 1;
            break;
         default:
            xassert(parm != parm);
      }
      csa->tol_bnd = parm->tol_bnd;
      csa->tol_bnd1 = .001 * parm->tol_bnd;
      csa->tol_dj = parm->tol_dj;
      csa->tol_dj1 = .001 * parm->tol_dj;
      csa->tol_piv = parm->tol_piv;
      switch (P->dir)
      {  case GLP_MIN:
            csa->obj_lim = + parm->obj_ul;
            break;
         case GLP_MAX:
            csa->obj_lim = - parm->obj_ll;
            break;
         default:
            xassert(parm != parm);
      }
      csa->it_lim = parm->it_lim;
      csa->tm_lim = parm->tm_lim;
      csa->out_frq = parm->out_frq;
      csa->out_dly = parm->out_dly;
      /* initialize working parameters */
      csa->tm_beg = xtime();
      csa->it_beg = csa->it_cnt = P->it_cnt;
      csa->it_dpy = -1;
      csa->inv_cnt = 0;
      /* try to solve working LP */
      ret = dual_simplex(csa);
      /* return basis factorization back to problem object */
      P->valid = csa->lp->valid;
      P->bfd = csa->lp->bfd;
      /* set solution status */
      P->pbs_stat = csa->p_stat;
      P->dbs_stat = csa->d_stat;
      /* if the solver failed, do not store basis header and basic
       * solution components to problem object */
      if (ret == GLP_EFAIL)
         goto skip;
      /* convert working LP basis to original LP basis and store it to
       * problem object */
      daeh = talloc(1+csa->lp->n, int);
      spx_store_basis(csa->lp, P, map, daeh);
      /* compute simplex multipliers for final basic solution found by
       * the solver */
      spx_eval_pi(csa->lp, csa->work);
      /* convert working LP solution to original LP solution and store
       * it to problem object */
      spx_store_sol(csa->lp, P, SHIFT, map, daeh, csa->beta, csa->work,
         csa->d);
      tfree(daeh);
      /* save simplex iteration count */
      P->it_cnt = csa->it_cnt;
      /* report auxiliary/structural variable causing unboundedness */
      P->some = 0;
      if (csa->p_stat == GLP_NOFEAS && csa->d_stat == GLP_FEAS)
      {  int k, kk;
         /* xB[p] = x[k] causes dual unboundedness */
         xassert(1 <= csa->p && csa->p <= csa->lp->m);
         k = csa->lp->head[csa->p];
         xassert(1 <= k && k <= csa->lp->n);
         /* convert to number of original variable */
         for (kk = 1; kk <= P->m + P->n; kk++)
         {  if (abs(map[kk]) == k)
            {  P->some = kk;
               break;
            }
         }
         xassert(P->some != 0);
      }
skip: /* deallocate working objects and arrays */
      spx_free_lp(csa->lp);
      tfree(map);
      tfree(csa->b);
      tfree(csa->l);
      tfree(csa->u);
      if (csa->at != NULL)
         spx_free_at(csa->lp, csa->at);
      if (csa->nt != NULL)
         spx_free_nt(csa->lp, csa->nt);
      tfree(csa->beta);
      tfree(csa->d);
      if (csa->se != NULL)
         spy_free_se(csa->lp, csa->se);
      tfree(csa->list);
      tfree(csa->trow);
      tfree(csa->tcol);
      tfree(csa->work);
      tfree(csa->work1);
      /* return to calling program */
      return ret >= 0 ? ret : GLP_EFAIL;
}

/* eof */
