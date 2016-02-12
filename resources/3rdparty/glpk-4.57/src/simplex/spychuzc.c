/* spychuzc.c */

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
#include "spychuzc.h"

/***********************************************************************
*  spy_chuzc_std - choose non-basic variable (dual textbook ratio test)
*
*  This routine implements an improved dual textbook ratio test to
*  choose non-basic variable xN[q].
*
*  Current reduced costs of non-basic variables should be placed in the
*  array locations d[1], ..., d[n-m]. Note that d[j] is a value of dual
*  basic variable lambdaN[j] in the current basis.
*
*  The parameter s specifies the sign of bound violation for basic
*  variable xB[p] chosen: s = +1.0 means that xB[p] violates its lower
*  bound, so dual non-basic variable lambdaB[p] = lambda^+B[p]
*  increases, and s = -1.0 means that xB[p] violates its upper bound,
*  so dual non-basic variable lambdaB[p] = lambda^-B[p] decreases.
*  (Thus, the dual ray parameter theta = s * lambdaB[p] >= 0.)
*
*  Elements of p-th simplex table row t[p] = (t[p,j]) corresponding
*  to basic variable xB[p] should be placed in the array locations
*  trow[1], ..., trow[n-m].
*
*  The parameter tol_piv specifies a tolerance for elements of the
*  simplex table row t[p]. If |t[p,j]| < tol_piv, dual basic variable
*  lambdaN[j] is skipped, i.e. it is assumed that it does not depend on
*  the dual ray parameter theta.
*
*  The parameters tol and tol1 specify tolerances used to increase the
*  choice freedom by simulating an artificial degeneracy as follows.
*  If lambdaN[j] = lambda^+N[j] >= 0 and d[j] <= +delta[j], or if
*  lambdaN[j] = lambda^-N[j] <= 0 and d[j] >= -delta[j], where
*  delta[j] = tol + tol1 * |cN[j]|, cN[j] is objective coefficient at
*  xN[j], then it is assumed that reduced cost d[j] is equal to zero.
*
*  The routine determines the index 1 <= q <= n-m of non-basic variable
*  xN[q], for which corresponding dual basic variable lambda^+N[j] or
*  lambda^-N[j] reaches its zero bound first on increasing the dual ray
*  parameter theta, and returns p on exit. And if theta may increase
*  unlimitedly, the routine returns zero. */

int spy_chuzc_std(SPXLP *lp, const double d[/*1+n-m*/],
      double s, const double trow[/*1+n-m*/], double tol_piv,
      double tol, double tol1)
{     int m = lp->m;
      int n = lp->n;
      double *c = lp->c;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      char *flag = lp->flag;
      int j, k, q;
      double alfa, biga, delta, teta, teta_min;
      xassert(s == +1.0 || s == -1.0);
      /* nothing is chosen so far */
      q = 0, teta_min = DBL_MAX, biga = 0.0;
      /* walk thru the list of non-basic variables */
      for (j = 1; j <= n-m; j++)
      {  k = head[m+j]; /* x[k] = xN[j] */
         /* if xN[j] is fixed variable, skip it */
         if (l[k] == u[k])
            continue;
         alfa = s * trow[j];
         if (alfa >= +tol_piv && !flag[j])
         {  /* xN[j] is either free or has its lower bound active, so
             * lambdaN[j] = d[j] >= 0 decreases down to zero */
            delta = tol + tol1 * (c[k] >= 0.0 ? +c[k] : -c[k]);
            /* determine theta on which lambdaN[j] reaches zero */
            teta = (d[j] < +delta ? 0.0 : d[j] / alfa);
         }
         else if (alfa <= -tol_piv && (l[k] == -DBL_MAX || flag[j]))
         {  /* xN[j] is either free or has its upper bound active, so
             * lambdaN[j] = d[j] <= 0 increases up to zero */
            delta = tol + tol1 * (c[k] >= 0.0 ? +c[k] : -c[k]);
            /* determine theta on which lambdaN[j] reaches zero */
            teta = (d[j] > -delta ? 0.0 : d[j] / alfa);
         }
         else
         {  /* lambdaN[j] cannot reach zero on increasing theta */
            continue;
         }
         /* choose non-basic variable xN[q] by corresponding dual basic
          * variable lambdaN[q] for which theta is minimal */
         xassert(teta >= 0.0);
         alfa = (alfa >= 0.0 ? +alfa : -alfa);
         if (teta_min > teta || (teta_min == teta && biga < alfa))
            q = j, teta_min = teta, biga = alfa;
      }
      return q;
}

/***********************************************************************
*  spy_chuzc_harris - choose non-basic var. (dual Harris' ratio test)
*
*  This routine implements dual Harris' ratio test to choose non-basic
*  variable xN[q].
*
*  All the parameters, except tol and tol1, as well as the returned
*  value have the same meaning as for the routine spx_chuzr_std (see
*  above).
*
*  The parameters tol and tol1 specify tolerances on zero bound
*  violations for reduced costs of non-basic variables. For reduced
*  cost d[j] the tolerance is delta[j] = tol + tol1 |cN[j]|, where
*  cN[j] is objective coefficient at non-basic variable xN[j]. */

int spy_chuzc_harris(SPXLP *lp, const double d[/*1+n-m*/],
      double s, const double trow[/*1+n-m*/], double tol_piv,
      double tol, double tol1)
{     int m = lp->m;
      int n = lp->n;
      double *c = lp->c;
      double *l = lp->l;
      double *u = lp->u;
      int *head = lp->head;
      char *flag = lp->flag;
      int j, k, q;
      double alfa, biga, delta, teta, teta_min;
      xassert(s == +1.0 || s == -1.0);
      /*--------------------------------------------------------------*/
      /* first pass: determine teta_min for relaxed bounds            */
      /*--------------------------------------------------------------*/
      teta_min = DBL_MAX;
      /* walk thru the list of non-basic variables */
      for (j = 1; j <= n-m; j++)
      {  k = head[m+j]; /* x[k] = xN[j] */
         /* if xN[j] is fixed variable, skip it */
         if (l[k] == u[k])
            continue;
         alfa = s * trow[j];
         if (alfa >= +tol_piv && !flag[j])
         {  /* xN[j] is either free or has its lower bound active, so
             * lambdaN[j] = d[j] >= 0 decreases down to zero */
            delta = tol + tol1 * (c[k] >= 0.0 ? +c[k] : -c[k]);
            /* determine theta on which lambdaN[j] reaches -delta */
            teta = ((d[j] < 0.0 ? 0.0 : d[j]) + delta) / alfa;
         }
         else if (alfa <= -tol_piv && (l[k] == -DBL_MAX || flag[j]))
         {  /* xN[j] is either free or has its upper bound active, so
             * lambdaN[j] = d[j] <= 0 increases up to zero */
            delta = tol + tol1 * (c[k] >= 0.0 ? +c[k] : -c[k]);
            /* determine theta on which lambdaN[j] reaches +delta */
            teta = ((d[j] > 0.0 ? 0.0 : d[j]) - delta) / alfa;
         }
         else
         {  /* lambdaN[j] cannot reach zero on increasing theta */
            continue;
         }
         xassert(teta >= 0.0);
         if (teta_min > teta)
            teta_min = teta;
      }
      /*--------------------------------------------------------------*/
      /* second pass: choose non-basic variable xN[q]                 */
      /*--------------------------------------------------------------*/
      if (teta_min == DBL_MAX)
      {  /* theta may increase unlimitedly */
         q = 0;
         goto done;
      }
      /* nothing is chosen so far */
      q = 0, biga = 0.0;
      /* walk thru the list of non-basic variables */
      for (j = 1; j <= n-m; j++)
      {  k = head[m+j]; /* x[k] = xN[j] */
         /* if xN[j] is fixed variable, skip it */
         if (l[k] == u[k])
            continue;
         alfa = s * trow[j];
         if (alfa >= +tol_piv && !flag[j])
         {  /* xN[j] is either free or has its lower bound active, so
             * lambdaN[j] = d[j] >= 0 decreases down to zero */
            /* determine theta on which lambdaN[j] reaches zero */
            teta = d[j] / alfa;
         }
         else if (alfa <= -tol_piv && (l[k] == -DBL_MAX || flag[j]))
         {  /* xN[j] is either free or has its upper bound active, so
             * lambdaN[j] = d[j] <= 0 increases up to zero */
            /* determine theta on which lambdaN[j] reaches zero */
            teta = d[j] / alfa;
         }
         else
         {  /* lambdaN[j] cannot reach zero on increasing theta */
            continue;
         }
         /* choose non-basic variable for which theta is not greater
          * than theta_min determined for relaxed bounds and which has
          * best (largest in magnitude) pivot */
         alfa = (alfa >= 0.0 ? +alfa : -alfa);
         if (teta <= teta_min && biga < alfa)
            q = j, biga = alfa;
      }
      /* something must be chosen */
      xassert(1 <= q && q <= n-m);
done: return q;
}

/* eof */
