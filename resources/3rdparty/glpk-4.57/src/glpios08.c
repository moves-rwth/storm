/* glpios08.c (clique cut generator) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2008, 2013 Andrew Makhorin, Department for Applied
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
#include "glpios.h"

void *ios_clq_init(glp_tree *T)
{     /* initialize clique cut generator */
      glp_prob *P = T->mip;
      CFG *G;
      int j, n1, n2;
      xprintf("Constructing conflict graph...\n");
      G = cfg_build_graph(P);
      n1 = n2 = 0;
      for (j = 1; j <= P->n; j++)
      {  if (G->pos[j])
            n1 ++;
         if (G->neg[j])
            n2++;
      }
      if (n1 == 0 && n2 == 0)
      {  xprintf("No conflicts found\n");
         cfg_delete_graph(G);
         G = NULL;
      }
      else
         xprintf("Conflict graph has %d + %d = %d vertices\n",
            n1, n2, G->nv);
      return G;
}

void ios_clq_gen(glp_tree *T, void *G_)
{     /* attempt to generate clique cut */
      glp_prob *P = T->mip;
      int n = P->n;
      CFG *G = G_;
      int *pos = G->pos;
      int *neg = G->neg;
      int nv = G->nv;
      int *ref = G->ref;
      int j, k, v, len, *ind;
      double rhs, sum, *val;
      xassert(G->n == n);
      /* allocate working arrays */
      ind = talloc(1+n, int);
      val = talloc(1+n, double);
      /* find maximum weight clique in conflict graph */
      len = cfg_find_clique(P, G, ind, &sum);
#ifdef GLP_DEBUG
      xprintf("len = %d; sum = %g\n", len, sum);
      cfg_check_clique(G, len, ind);
#endif
      /* check if clique inequality is violated */
      if (sum < 1.07)
         goto skip;
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
      /* add cut inequality to local cut pool */
      glp_ios_add_row(T, NULL, GLP_RF_CLQ, 0, len, ind, val, GLP_UP,
         rhs);
skip: /* free working arrays */
      tfree(ind);
      tfree(val);
      return;
}

void ios_clq_term(void *G_)
{     /* terminate clique cut generator */
      CFG *G = G_;
      xassert(G != NULL);
      cfg_delete_graph(G);
      return;
}

/* eof */
