/* clqcut.c (clique cut generator) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2008-2016 Andrew Makhorin, Department for Applied
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

#include "cfg.h"
#include "env.h"
#include "prob.h"

/***********************************************************************
*  NAME
*
*  glp_clq_cut - generate clique cut from conflict graph
*
*  SYNOPSIS
*
*  int glp_clq_cut(glp_prob *P, glp_cfg *G, int ind[], double val[]);
*
*  DESCRIPTION
*
*  This routine attempts to generate a clique cut.
*
*  The cut generated by the routine is the following inequality:
*
*     sum a[j] * x[j] <= b,
*
*  which is expected to be violated at the current basic solution.
*
*  If the cut has been successfully generated, the routine stores its
*  non-zero coefficients a[j] and corresponding column indices j in the
*  array locations val[1], ..., val[len] and ind[1], ..., ind[len],
*  where 1 <= len <= n is the number of non-zero coefficients. The
*  right-hand side value b is stored in val[0], and ind[0] is set to 0.
*
*  RETURNS
*
*  If the cut has been successfully generated, the routine returns
*  len, the number of non-zero coefficients in the cut, 1 <= len <= n.
*  Otherwise, the routine returns a non-positive value. */

int glp_clq_cut(glp_prob *P, glp_cfg *G, int ind[], double val[])
{     int n = P->n;
      int *pos = G->pos;
      int *neg = G->neg;
      int nv = G->nv;
      int *ref = G->ref;
      int j, k, v, len;
      double rhs, sum;
      xassert(G->n == n);
      /* find maximum weight clique in conflict graph */
      len = cfg_find_clique(P, G, ind, &sum);
#ifdef GLP_DEBUG
      xprintf("len = %d; sum = %g\n", len, sum);
      cfg_check_clique(G, len, ind);
#endif
      /* check if clique inequality is violated */
      if (sum < 1.07)
         return 0;
      /* expand clique to maximal one */
      len = cfg_expand_clique(G, len, ind);
#ifdef GLP_DEBUG
      xprintf("maximal clique size = %d\n", len);
      cfg_check_clique(G, len, ind);
#endif
      /* construct clique cut (fixed binary variables are removed, so
         this cut is only locally valid) */
      rhs = 1.0;
      for (j = 1; j <= n; j++)
         val[j] = 0.0;
      for (k = 1; k <= len; k++)
      {  /* v is clique vertex */
         v = ind[k];
         xassert(1 <= v && v <= nv);
         /* j is number of corresponding binary variable */
         j = ref[v];
         xassert(1 <= j && j <= n);
         if (pos[j] == v)
         {  /* v corresponds to x[j] */
            if (P->col[j]->type == GLP_FX)
            {  /* x[j] is fixed */
               rhs -= P->col[j]->prim;
            }
            else
            {  /* x[j] is not fixed */
               val[j] += 1.0;
            }
         }
         else if (neg[j] == v)
         {  /* v corresponds to (1 - x[j]) */
            if (P->col[j]->type == GLP_FX)
            {  /* x[j] is fixed */
               rhs -= (1.0 - P->col[j]->prim);
            }
            else
            {  /* x[j] is not fixed */
               val[j] -= 1.0;
               rhs -= 1.0;
            }
         }
         else
            xassert(v != v);
      }
      /* convert cut inequality to sparse format */
      len = 0;
      for (j = 1; j <= n; j++)
      {  if (val[j] != 0.0)
         {  len++;
            ind[len] = j;
            val[len] = val[j];
         }
      }
      ind[0] = 0, val[0] = rhs;
      return len;
}

/* eof */