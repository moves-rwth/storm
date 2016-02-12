/* spxprim.c */

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
#include "spxchuzc.h"
#include "spxchuzr.h"
#include "spxprob.h"

#define USE_AT 0
/* 1 - use A in row-wise format
 * 0 - use N in row-wise format */

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
      double *c; /* double c[1+n]; */
      /* copy of original objective coefficients */
      SPXAT *at;
      /* mxn-matrix A of constraint coefficients, in sparse row-wise
       * format (NULL if not used) */
      SPXNT *nt;
      /* mx(n-m)-matrix N composed of non-basic columns of constraint
       * matrix A, in sparse row-wise format (NULL if not used) */
      int phase;
      /* search phase:
       * 0 - not determined yet
       * 1 - searching for primal feasible solution
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
      SPXSE *se;
      /* projected steepest edge and Devex pricing data block (NULL if
       * not used) */
      int num;
      /* number of eligible non-basic variables */
      int *list; /* int list[1+n-m]; */
      /* list[1], ..., list[num] are indices j of eligible non-basic
       * variables xN[j] */
      int q;
      /* xN[q] is a non-basic variable chosen to enter the basis */
      double *tcol; /* double tcol[1+m]; */
      /* q-th (pivot) column of the simplex table */
      int p;
      /* xB[p] is a basic variable chosen to leave the basis;
       * p = 0 means that no basic variable reaches its bound;
       * p < 0 means that non-basic variable xN[q] reaches its opposite
       * bound before any basic variable */
      int p_flag;
      /* if this flag is set, the active bound of xB[p] in the adjacent
       * basis should be set to the upper bound */
      double *trow; /* double trow[1+n-m]; */
      /* p-th (pivot) row of the simplex table */
      double *work; /* double work[1+m]; */
      /* working array */
      int p_stat, d_stat;
      /* primal and dual solution statuses */
      /*--------------------------------------------------------------*/
      /* control parameters (see struct glp_smcp) */
      int msg_lev;
      /* message level */
      int harris;
      /* ratio test technique:
       * 0 - textbook ratio test
       * 1 - Harris' two pass ratio test */
      double tol_bnd, tol_bnd1;
      /* primal feasibility tolerances */
      double tol_dj, tol_dj1;
      /* dual feasibility tolerances */
      double tol_piv;
      /* pivot tolerance */
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
       * basis changes (including the case when a non-basic variable
       * jumps to its opposite bound) */
      int it_dpy;
      /* simplex iteration count at most recent display output */
      int inv_cnt;
      /* basis factorization count since most recent display output */
};

/***********************************************************************
*  set_penalty - set penalty function coefficients
*
*  This routine sets up objective coefficients of the penalty function,
*  which is the sum of primal infeasibilities, as follows:
*
*     if beta[i] < l[k] - eps1, set c[k] = -1,
*
*     if beta[i] > u[k] + eps2, set c[k] = +1,
*
*     otherwise, set c[k] = 0,
*
*  where beta[i] is current value of basic variable xB[i] = x[k], l[k]
*  and u[k] are original bounds of x[k], and
*
*     eps1 = tol + tol1 * |l[k]|,
*
*     eps2 = tol + tol1 * |u[k]|.
*
*  The routine returns the number of non-zero objective coefficients,
*  which is the number of basic variables violating their bounds. Thus,
*  if the value returned is zero, the current basis is primal feasible
*  within the specified tolerances. */

static int set_penalty(struct csa *csa, double tol, double tol1)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *c = lp->c;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      double *beta = csa->beta;
      int i, k, count = 0;
      double t, eps;
      /* reset objective coefficients */
      for (k = 0; k <= n; k++)
         c[k] = 0.0;
      /* walk thru the list of basic variables */
      for (i = 1; i <= m; i++)
      {  k = head[i]; /* x[k] = xB[i] */
         /* check lower bound */
         if ((t = l[k]) != -DBL_MAX)
         {  eps = tol + tol1 * (t >= 0.0 ? +t : -t);
            if (beta[i] < t - eps)
            {  /* lower bound is violated */
               c[k] = -1.0, count++;
            }
         }
         /* check upper bound */
         if ((t = u[k]) != +DBL_MAX)
         {  eps = tol + tol1 * (t >= 0.0 ? +t : -t);
            if (beta[i] > t + eps)
            {  /* upper bound is violated */
               c[k] = +1.0, count++;
            }
         }
      }
      return count;
}

/***********************************************************************
*  check_feas - check primal feasibility of basic solution
*
*  This routine checks if the specified values of all basic variables
*  beta = (beta[i]) are within their bounds.
*
*  Let l[k] and u[k] be original bounds of basic variable xB[i] = x[k].
*  The actual bounds of x[k] are determined as follows:
*
*  1) if phase = 1 and c[k] < 0, x[k] violates its lower bound, so its
*     actual bounds are artificial: -inf < x[k] <= l[k];
*
*  2) if phase = 1 and c[k] > 0, x[k] violates its upper bound, so its
*     actual bounds are artificial: u[k] <= x[k] < +inf;
*
*  3) in all other cases (if phase = 1 and c[k] = 0, or if phase = 2)
*     actual bounds are original: l[k] <= x[k] <= u[k].
*
*  The parameters tol and tol1 are bound violation tolerances. The
*  actual bounds l'[k] and u'[k] are considered as non-violated within
*  the specified tolerance if
*
*     l'[k] - eps1 <= beta[i] <= u'[k] + eps2,
*
*  where eps1 = tol + tol1 * |l'[k]|, eps2 = tol + tol1 * |u'[k]|.
*
*  The routine returns one of the following codes:
*
*  0 - solution is feasible (no actual bounds are violated);
*
*  1 - solution is infeasible, however, only artificial bounds are
*      violated (this is possible only if phase = 1);
*
*  2 - solution is infeasible and at least one original bound is
*      violated. */

static int check_feas(struct csa *csa, int phase, double tol, double
      tol1)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      double *c = lp->c;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      double *beta = csa->beta;
      int i, k, orig, ret = 0;
      double lk, uk, eps;
      xassert(phase == 1 || phase == 2);
      /* walk thru the list of basic variables */
      for (i = 1; i <= m; i++)
      {  k = head[i]; /* x[k] = xB[i] */
         /* determine actual bounds of x[k] */
         if (phase == 1 && c[k] < 0.0)
         {  /* -inf < x[k] <= l[k] */
            lk = -DBL_MAX, uk = l[k];
            orig = 0; /* artificial bounds */
         }
         else if (phase == 1 && c[k] > 0.0)
         {  /* u[k] <= x[k] < +inf */
            lk = u[k], uk = +DBL_MAX;
            orig = 0; /* artificial bounds */
         }
         else
         {  /* l[k] <= x[k] <= u[k] */
            lk = l[k], uk = u[k];
            orig = 1; /* original bounds */
         }
         /* check actual lower bound */
         if (lk != -DBL_MAX)
         {  eps = tol + tol1 * (lk >= 0.0 ? +lk : -lk);
            if (beta[i] < lk - eps)
            {  /* actual lower bound is violated */
               if (orig)
               {  ret = 2;
                  break;
               }
               ret = 1;
            }
         }
         /* check actual upper bound */
         if (uk != +DBL_MAX)
         {  eps = tol + tol1 * (uk >= 0.0 ? +uk : -uk);
            if (beta[i] > uk + eps)
            {  /* actual upper bound is violated */
               if (orig)
               {  ret = 2;
                  break;
               }
               ret = 1;
            }
         }
      }
      return ret;
}

/***********************************************************************
*  adjust_penalty - adjust penalty function coefficients
*
*  On searching for primal feasible solution it may happen that some
*  basic variable xB[i] = x[k] has non-zero objective coefficient c[k]
*  indicating that xB[i] violates its lower (if c[k] < 0) or upper (if
*  c[k] > 0) original bound, but due to primal degenarcy the violation
*  is close to zero.
*
*  This routine identifies such basic variables and sets objective
*  coefficients at these variables to zero that allows avoiding zero-
*  step simplex iterations.
*
*  The parameters tol and tol1 are bound violation tolerances. The
*  original bounds l[k] and u[k] are considered as non-violated within
*  the specified tolerance if
*
*     l[k] - eps1 <= beta[i] <= u[k] + eps2,
*
*  where beta[i] is value of basic variable xB[i] = x[k] in the current
*  basis, eps1 = tol + tol1 * |l[k]|, eps2 = tol + tol1 * |u[k]|.
*
*  The routine returns the number of objective coefficients which were
*  set to zero. */

static int adjust_penalty(struct csa *csa, double tol, double tol1)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      double *c = lp->c;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      double *beta = csa->beta;
      int i, k, count = 0;
      double t, eps;
      xassert(csa->phase == 1);
      /* walk thru the list of basic variables */
      for (i = 1; i <= m; i++)
      {  k = head[i]; /* x[k] = xB[i] */
         if (c[k] < 0.0)
         {  /* x[k] violates its original lower bound l[k] */
            xassert((t = l[k]) != -DBL_MAX);
            eps = tol + tol1 * (t >= 0.0 ? +t : -t);
            if (beta[i] >= t - eps)
            {  /* however, violation is close to zero */
               c[k] = 0.0, count++;
            }
         }
         else if (c[k] > 0.0)
         {  /* x[k] violates its original upper bound u[k] */
            xassert((t = u[k]) != +DBL_MAX);
            eps = tol + tol1 * (t >= 0.0 ? +t : -t);
            if (beta[i] <= t + eps)
            {  /* however, violation is close to zero */
               c[k] = 0.0, count++;
            }
         }
      }
      return count;
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
      SPXSE *se = csa->se;
      int j;
      double err, *gamma;
      xassert(se != NULL);
      gamma = talloc(1+n-m, double);
      for (j = 1; j <= n-m; j++)
         gamma[j] = spx_eval_gamma_j(lp, se, j);
      err = err_in_vec(n-m, gamma, se->gamma);
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
*  choose_pivot - choose xN[q] and xB[p]
*
*  Given the list of eligible non-basic variables this routine first
*  chooses non-basic variable xN[q]. This choice is always possible,
*  because the list is assumed to be non-empty. Then the routine
*  computes q-th column T[*,q] of the simplex table T[i,j] and chooses
*  basic variable xB[p]. If the pivot T[p,q] is small in magnitude,
*  the routine attempts to choose another xN[q] and xB[p] in order to
*  avoid badly conditioned adjacent bases. */

static void choose_pivot(struct csa *csa)
{     SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *beta = csa->beta;
      double *d = csa->d;
      SPXSE *se = csa->se;
      int *list = csa->list;
      int nnn, try, q, t, p_flag, p;
      double *tcol = csa->work;
      /* initial number of eligible non-basic variables */
      nnn = csa->num;
      /* nothing has been chosen so far */
      csa->q = 0;
      try = 0;
try:  /* choose non-basic variable xN[q] */
      xassert(nnn > 0);
      try++;
      if (se == NULL)
      {  /* Dantzig's rule */
         q = spx_chuzc_std(lp, d, nnn, list);
      }
      else
      {  /* projected steepest edge */
         q = spx_chuzc_pse(lp, se, d, nnn, list);
      }
      xassert(1 <= q && q <= n-m);
      /* compute q-th column of the simplex table */
      spx_eval_tcol(lp, q, tcol);
      /* choose basic variable xB[p] */
      if (!csa->harris)
      {  /* textbook ratio test */
         p = spx_chuzr_std(lp, csa->phase, beta, q,
            d[q] < 0.0 ? +1. : -1., tcol, &p_flag, csa->tol_piv,
            .30 * csa->tol_bnd, .30 * csa->tol_bnd1);
      }
      else
      {  /* Harris' two-pass ratio test */
         p = spx_chuzr_harris(lp, csa->phase, beta, q,
            d[q] < 0.0 ? +1. : -1., tcol, &p_flag , csa->tol_piv,
            .50 * csa->tol_bnd, .50 * csa->tol_bnd1);
      }
      /* either keep previous choice or accept new choice depending on
       * which one is better */
      if (csa->q == 0 || p <= 0 ||
         fabs(tcol[p]) > fabs(csa->tcol[csa->p]))
      {  csa->q = q;
         memcpy(&csa->tcol[1], &tcol[1], m * sizeof(double));
         csa->p = p;
         csa->p_flag = p_flag;
      }
      /* check if current choice is acceptable */
      if (csa->p <= 0 || fabs(csa->tcol[csa->p]) >= 0.001)
         goto done;
      if (nnn == 1)
         goto done;
      if (try == 5)
         goto done;
      /* try to choose other xN[q] and xB[p] */
      /* find xN[q] in the list */
      for (t = 1; t <= nnn; t++)
         if (list[t] == q) break;
      xassert(t <= nnn);
      /* move xN[q] to the end of the list */
      list[t] = list[nnn], list[nnn] = q;
      /* and exclude it from consideration */
      nnn--;
      /* repeat the choice */
      goto try;
done: /* the choice has been made */
      return;
}

/***********************************************************************
*  sum_infeas - compute sum of primal infeasibilities
*
*  This routine compute the sum of primal infeasibilities, which is the
*  current penalty function value. */

static double sum_infeas(SPXLP *lp, const double beta[/*1+m*/])
{     int m = lp->m;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      int i, k;
      double sum = 0.0;
      for (i = 1; i <= m; i++)
      {  k = head[i]; /* x[k] = xB[i] */
         if (l[k] != -DBL_MAX && beta[i] < l[k])
            sum += l[k] - beta[i];
         if (u[k] != +DBL_MAX && beta[i] > u[k])
            sum += beta[i] - u[k];
      }
      return sum;
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
*  original objective value;
*
*  sum of (scaled) primal infeasibilities;
*
*  number of infeasibilities (phase I) or non-optimalities (phase II);
*
*  number of basic factorizations since last display output. */

static void display(struct csa *csa, int spec)
{     int nnn, k;
      double obj, sum, *save;
      /* check if the display output should be skipped */
      if (csa->msg_lev < GLP_MSG_ON) goto skip;
      if (csa->out_dly > 0 &&
         1000.0 * xdifftime(xtime(), csa->tm_beg) < csa->out_dly)
         goto skip;
      if (csa->it_cnt == csa->it_dpy) goto skip;
      if (!spec && csa->it_cnt % csa->out_frq != 0) goto skip;
      /* compute original objective value */
      save = csa->lp->c;
      csa->lp->c = csa->c;
      obj = csa->dir * spx_eval_obj(csa->lp, csa->beta);
      csa->lp->c = save;
      /* compute sum of (scaled) primal infeasibilities */
      sum = sum_infeas(csa->lp, csa->beta);
      /* compute number of infeasibilities/non-optimalities */
      switch (csa->phase)
      {  case 1:
            nnn = 0;
            for (k = 1; k <= csa->lp->n; k++)
               if (csa->lp->c[k] != 0.0) nnn++;
            break;
         case 2:
            xassert(csa->d_st);
            nnn = spx_chuzc_sel(csa->lp, csa->d, csa->tol_dj,
               csa->tol_dj1, NULL);
            break;
         default:
            xassert(csa != csa);
      }
      /* display search progress */
      xprintf("%c%6d: obj = %17.9e inf = %11.3e (%d)",
         csa->phase == 2 ? '*' : ' ', csa->it_cnt, obj, sum, nnn);
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
*  spx_primal - driver to primal simplex method
*
*  This routine is a driver to the two-phase primal simplex method.
*
*  On exit this routine returns one of the following codes:
*
*  0  LP instance has been successfully solved.
*
*  GLP_EITLIM
*     Iteration limit has been exhausted.
*
*  GLP_ETMLIM
*     Time limit has been exhausted.
*
*  GLP_EFAIL
*     The solver failed to solve LP instance. */

static int primal_simplex(struct csa *csa)
{     /* primal simplex method main logic routine */
      SPXLP *lp = csa->lp;
      int m = lp->m;
      int n = lp->n;
      double *c = lp->c;
      int *head = lp->head;
      SPXAT *at = csa->at;
      SPXNT *nt = csa->nt;
      double *beta = csa->beta;
      double *d = csa->d;
      SPXSE *se = csa->se;
      int *list = csa->list;
      double *tcol = csa->tcol;
      double *trow = csa->trow;
      double *pi = csa->work;
      double *rho = csa->work;
      int msg_lev = csa->msg_lev;
      double tol_bnd = csa->tol_bnd;
      double tol_bnd1 = csa->tol_bnd1;
      double tol_dj = csa->tol_dj;
      double tol_dj1 = csa->tol_dj1;
      int j, refct, ret;
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
      /* compute values of basic variables beta = (beta[i]) */
      if (!csa->beta_st)
      {  spx_eval_beta(lp, beta);
         csa->beta_st = 1; /* just computed */
         /* determine the search phase, if not determined yet */
         if (!csa->phase)
         {  if (set_penalty(csa, 0.97 * tol_bnd, 0.97 * tol_bnd1))
            {  /* current basic solution is primal infeasible */
               /* start to minimize the sum of infeasibilities */
               csa->phase = 1;
            }
            else
            {  /* current basic solution is primal feasible */
               /* start to minimize the original objective function */
               csa->phase = 2;
               memcpy(c, csa->c, (1+n) * sizeof(double));
            }
            /* working objective coefficients have been changed, so
             * invalidate reduced costs */
            csa->d_st = 0;
         }
         /* make sure that the current basic solution remains primal
          * feasible (or pseudo-feasible on phase I) */
         if (check_feas(csa, csa->phase, tol_bnd, tol_bnd1))
         {  /* excessive bound violations due to round-off errors */
            if (msg_lev >= GLP_MSG_ERR)
               xprintf("Warning: numerical instability (primal simplex,"
                  " phase %s)\n", csa->phase == 1 ? "I" : "II");
            /* restart the search */
            lp->valid = 0;
            csa->phase = 0;
            goto loop;
         }
      }
      /* at this point the search phase is determined */
      xassert(csa->phase == 1 || csa->phase == 2);
      if (csa->phase == 1)
      {  /* adjust penalty function coefficients */
         if (adjust_penalty(csa, tol_bnd, tol_bnd1))
         {  /* some coefficients were changed, so invalidate reduced
             * costs of non-basic variables */
            csa->d_st = 0;
         }
      }
      /* compute reduced costs of non-basic variables d = (d[j]) */
      if (!csa->d_st)
      {  spx_eval_pi(lp, pi);
         for (j = 1; j <= n-m; j++)
            d[j] = spx_eval_dj(lp, pi, j);
         csa->d_st = 1; /* just computed */
      }
      /* reset the reference space, if necessary */
      if (se != NULL && !se->valid)
         spx_reset_refsp(lp, se), refct = 1000;
      /* at this point the basis factorization and all basic solution
       * components are valid */
      xassert(lp->valid && csa->beta_st && csa->d_st);
#if CHECK_ACCURACY
      /* check accuracy of current basic solution components (only for
       * debugging) */
      check_accuracy(csa);
#endif
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
         csa->p_stat = (csa->phase == 2 ? GLP_FEAS : GLP_INFEAS);
         csa->d_stat = GLP_UNDEF; /* will be set below */
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
         csa->p_stat = (csa->phase == 2 ? GLP_FEAS : GLP_INFEAS);
         csa->d_stat = GLP_UNDEF; /* will be set below */
         ret = GLP_ETMLIM;
         goto fini;
      }
      /* display the search progress */
      display(csa, 0);
      /* select eligible non-basic variables */
      switch (csa->phase)
      {  case 1:
            csa->num = spx_chuzc_sel(lp, d, 1e-8, 0.0, list);
            break;
         case 2:
            csa->num = spx_chuzc_sel(lp, d, tol_dj, tol_dj1, list);
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
               /* check for primal feasibility */
               if (!check_feas(csa, 2, tol_bnd, tol_bnd1))
               {  /* feasible solution found; switch to phase II */
                  memcpy(c, csa->c, (1+n) * sizeof(double));
                  csa->phase = 2;
                  csa->d_st = 0;
                  goto loop;
               }
               /* no feasible solution exists */
               if (msg_lev >= GLP_MSG_ALL)
                  xprintf("LP HAS NO PRIMAL FEASIBLE SOLUTION\n");
               csa->p_stat = GLP_NOFEAS;
               csa->d_stat = GLP_UNDEF; /* will be set below */
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
      /* choose xN[q] and xB[p] */
      choose_pivot(csa);
      /* check for unboundedness */
      if (csa->p == 0)
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
                  xprintf("Error: primal simplex failed\n");
               csa->p_stat = csa->d_stat = GLP_UNDEF;
               ret = GLP_EFAIL;
               goto fini;
            case 2:
               /* primal unboundedness detected */
               if (msg_lev >= GLP_MSG_ALL)
                  xprintf("LP HAS UNBOUNDED PRIMAL SOLUTION\n");
               csa->p_stat = GLP_FEAS;
               csa->d_stat = GLP_NOFEAS;
               ret = 0;
               goto fini;
            default:
               xassert(csa != csa);
         }
      }
      /* update values of basic variables for adjacent basis */
      spx_update_beta(lp, beta, csa->p, csa->p_flag, csa->q, tcol);
      csa->beta_st = 2;
      /* p < 0 means that xN[q] jumps to its opposite bound */
      if (csa->p < 0)
         goto skip;
      /* xN[q] enters and xB[p] leaves the basis */
      /* compute p-th row of inv(B) */
      spx_eval_rho(lp, csa->p, rho);
      /* compute p-th (pivot) row of the simplex table */
      if (at != NULL)
         spx_eval_trow1(lp, at, rho, trow);
      else
         spx_nt_prod(lp, nt, trow, 1, -1.0, rho);
      /* FIXME: tcol[p] and trow[q] should be close to each other */
      xassert(trow[csa->q] != 0.0);
      /* update reduced costs of non-basic variables for adjacent
       * basis */
      if (spx_update_d(lp, d, csa->p, csa->q, trow, tcol) <= 1e-9)
      {  /* successful updating */
         csa->d_st = 2;
         if (csa->phase == 1)
         {  /* adjust reduced cost of xN[q] in adjacent basis, since
             * its penalty coefficient changes (see below) */
            d[csa->q] -= c[head[csa->p]];
         }
      }
      else
      {  /* new reduced costs are inaccurate */
         csa->d_st = 0;
      }
      if (csa->phase == 1)
      {  /* xB[p] leaves the basis replacing xN[q], so set its penalty
          * coefficient to zero */
         c[head[csa->p]] = 0.0;
      }
      /* update steepest edge weights for adjacent basis, if used */
      if (se != NULL)
      {  if (refct > 0)
         {  if (spx_update_gamma(lp, se, csa->p, csa->q, trow, tcol)
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
skip: /* change current basis header to adjacent one */
      spx_change_basis(lp, csa->p, csa->p_flag, csa->q);
      /* and update factorization of the basis matrix */
      if (csa->p > 0)
         spx_update_invb(lp, csa->p, head[csa->p]);
      /* simplex iteration complete */
      csa->it_cnt++;
      goto loop;
fini: /* restore original objective function */
      memcpy(c, csa->c, (1+n) * sizeof(double));
      /* compute reduced costs of non-basic variables and determine
       * solution dual status, if necessary */
      if (csa->p_stat != GLP_UNDEF && csa->d_stat == GLP_UNDEF)
      {  xassert(ret != GLP_EFAIL);
         spx_eval_pi(lp, pi);
         for (j = 1; j <= n-m; j++)
            d[j] = spx_eval_dj(lp, pi, j);
         csa->num = spx_chuzc_sel(lp, d, tol_dj, tol_dj1, NULL);
         csa->d_stat = (csa->num == 0 ? GLP_FEAS : GLP_INFEAS);
      }
      return ret;
}

int spx_primal(glp_prob *P, const glp_smcp *parm)
{     /* driver to primal simplex method */
      struct csa csa_, *csa = &csa_;
      SPXLP lp;
#if USE_AT
      SPXAT at;
#else
      SPXNT nt;
#endif
      SPXSE se;
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
      csa->c = talloc(1+csa->lp->n, double);
      memcpy(csa->c, csa->lp->c, (1+csa->lp->n) * sizeof(double));
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
            spx_alloc_se(csa->lp, csa->se);
            break;
         default:
            xassert(parm != parm);
      }
      csa->list = talloc(1+csa->lp->n-csa->lp->m, int);
      csa->tcol = talloc(1+csa->lp->m, double);
      csa->trow = talloc(1+csa->lp->n-csa->lp->m, double);
      csa->work = talloc(1+csa->lp->m, double);
      /* initialize control parameters */
      csa->msg_lev = parm->msg_lev;
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
      ret = primal_simplex(csa);
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
      if (csa->p_stat == GLP_FEAS && csa->d_stat == GLP_NOFEAS)
      {  int k, kk;
         /* xN[q] = x[k] causes unboundedness */
         xassert(1 <= csa->q && csa->q <= csa->lp->n - csa->lp->m);
         k = csa->lp->head[csa->lp->m + csa->q];
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
      tfree(csa->c);
      if (csa->at != NULL)
         spx_free_at(csa->lp, csa->at);
      if (csa->nt != NULL)
         spx_free_nt(csa->lp, csa->nt);
      tfree(csa->beta);
      tfree(csa->d);
      if (csa->se != NULL)
         spx_free_se(csa->lp, csa->se);
      tfree(csa->list);
      tfree(csa->tcol);
      tfree(csa->trow);
      tfree(csa->work);
      /* return to calling program */
      return ret;
}

/* eof */
