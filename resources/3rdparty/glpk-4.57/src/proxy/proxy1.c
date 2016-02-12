/* proxy1.c */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2013 Andrew Makhorin, Department for Applied
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
#include "glpios.h"
#include "proxy.h"

void ios_proxy_heur(glp_tree *T)
{     glp_prob *prob;
      int j, status;
      double *xstar, zstar;
      /* this heuristic is applied only once on the root level */
      if (!(T->curr->level == 0 && T->curr->solved == 1))
         goto done;
      prob = glp_create_prob();
      glp_copy_prob(prob, T->mip, 0);
      xstar = xcalloc(1+prob->n, sizeof(double));
      for (j = 1; j <= prob->n; j++)
         xstar[j] = 0.0;
      if (T->mip->mip_stat != GLP_FEAS)
         status = proxy(prob, &zstar, xstar, NULL, 0.0,
            T->parm->ps_tm_lim, 1);
      else
      {  double *xinit = xcalloc(1+prob->n, sizeof(double));
         for (j = 1; j <= prob->n; j++)
            xinit[j] = T->mip->col[j]->mipx;
         status = proxy(prob, &zstar, xstar, xinit, 0.0,
            T->parm->ps_tm_lim, 1);
         xfree(xinit);
      }
      if (status == 0)
         glp_ios_heur_sol(T, xstar);
      xfree(xstar);
      glp_delete_prob(prob);
done: return;
}

/* eof */
