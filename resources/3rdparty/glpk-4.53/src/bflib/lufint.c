/* lufint.c (interface to LU-factorization) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2012, 2013 Andrew Makhorin, Department for Applied
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
#include "lufint.h"

LUFINT *lufint_create(void)
{     /* create interface to LU-factorization */
      LUFINT *fi;
      fi = talloc(1, LUFINT);
      fi->n_max = 0;
      fi->valid = 0;
      fi->sva = NULL;
      fi->luf = NULL;
      fi->sgf = NULL;
      fi->sva_n_max = fi->sva_size = 0;
      fi->delta_n0 = fi->delta_n = 0;
      fi->sgf_updat = 0;
      fi->sgf_piv_tol = 0.10;
      fi->sgf_piv_lim = 4;
      fi->sgf_suhl = 1;
      fi->sgf_eps_tol = DBL_EPSILON;
      return fi;
}

static int setup_v_cols(LUF *luf, int (*col)(void *info, int j,
      int ind[], double val[]), void *info, int ind[], double val[])
{     /* setup matrix V = A in column-wise format */
      int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int vc_ref = luf->vc_ref;
      int *vc_ptr = &sva->ptr[vc_ref-1];
      int *vc_len = &sva->len[vc_ref-1];
      int *vc_cap = &sva->cap[vc_ref-1];
      int j, len, ptr, nnz;
      nnz = 0;
      for (j = 1; j <= n; j++)
      {  /* get j-th column */
         len = col(info, j, ind, val);
         xassert(0 <= len && len <= n);
         /* enlarge j-th column capacity */
         if (vc_cap[j] < len)
         {  if (sva->r_ptr - sva->m_ptr < len)
            {  sva_more_space(sva, len);
               sv_ind = sva->ind;
               sv_val = sva->val;
            }
            sva_enlarge_cap(sva, vc_ref-1+j, len, 0);
         }
         /* store j-th column */
         ptr = vc_ptr[j];
         memcpy(&sv_ind[ptr], &ind[1], len * sizeof(int));
         memcpy(&sv_val[ptr], &val[1], len * sizeof(double));
         vc_len[j] = len;
         nnz += len;
      }
      return nnz;
}

int lufint_factorize(LUFINT *fi, int n, int (*col)(void *info, int j,
      int ind[], double val[]), void *info)
{     /* compute LU-factorization of specified matrix A */
      SVA *sva;
      LUF *luf;
      SGF *sgf;
      int k;
      xassert(n > 0);
      fi->valid = 0;
      /* create sparse vector area (SVA), if necessary */
      sva = fi->sva;
      if (sva == NULL)
      {  int sva_n_max = fi->sva_n_max;
         int sva_size = fi->sva_size;
         if (sva_n_max == 0)
            sva_n_max = 4 * n;
         if (sva_size == 0)
            sva_size = 10 * n;
         sva = fi->sva = sva_create_area(sva_n_max, sva_size);
      }
      /* allocate/reallocate underlying objects, if necessary */
      if (fi->n_max < n)
      {  int n_max = fi->n_max;
         if (n_max == 0)
            n_max = fi->n_max = n + fi->delta_n0;
         else
            n_max = fi->n_max = n + fi->delta_n;
         xassert(n_max >= n);
         /* allocate/reallocate LU-factorization (LUF) */
         luf = fi->luf;
         if (luf == NULL)
         {  luf = fi->luf = talloc(1, LUF);
            memset(luf, 0, sizeof(LUF));
            luf->sva = sva;
         }
         else
         {  tfree(luf->vr_piv);
            tfree(luf->pp_ind);
            tfree(luf->pp_inv);
            tfree(luf->qq_ind);
            tfree(luf->qq_inv);
         }
         luf->vr_piv = talloc(1+n_max, double);
         luf->pp_ind = talloc(1+n_max, int);
         luf->pp_inv = talloc(1+n_max, int);
         luf->qq_ind = talloc(1+n_max, int);
         luf->qq_inv = talloc(1+n_max, int);
         /* allocate/reallocate factorizer workspace (SGF) */
         sgf = fi->sgf;
         if (sgf == NULL)
         {  sgf = fi->sgf = talloc(1, SGF);
            memset(sgf, 0, sizeof(SGF));
            sgf->luf = luf;
         }
         else
         {  tfree(sgf->rs_head);
            tfree(sgf->rs_prev);
            tfree(sgf->rs_next);
            tfree(sgf->cs_head);
            tfree(sgf->cs_prev);
            tfree(sgf->cs_next);
            tfree(sgf->vr_max);
            tfree(sgf->flag);
            tfree(sgf->work);
         }
         sgf->rs_head = talloc(1+n_max, int);
         sgf->rs_prev = talloc(1+n_max, int);
         sgf->rs_next = talloc(1+n_max, int);
         sgf->cs_head = talloc(1+n_max, int);
         sgf->cs_prev = talloc(1+n_max, int);
         sgf->cs_next = talloc(1+n_max, int);
         sgf->vr_max = talloc(1+n_max, double);
         sgf->flag = talloc(1+n_max, char);
         sgf->work = talloc(1+n_max, double);
      }
      luf = fi->luf;
      sgf = fi->sgf;
#if 1 /* FIXME */
      /* initialize SVA */
      sva->n = 0;
      sva->m_ptr = 1;
      sva->r_ptr = sva->size + 1;
      sva->head = sva->tail = 0;
#endif
      /* allocate sparse vectors in SVA */
      luf->n = n;
      luf->fr_ref = sva_alloc_vecs(sva, n);
      luf->fc_ref = sva_alloc_vecs(sva, n);
      luf->vr_ref = sva_alloc_vecs(sva, n);
      luf->vc_ref = sva_alloc_vecs(sva, n);
      /* setup matrix V = A in column-wise format */
      setup_v_cols(luf, col, info, sgf->rs_prev, sgf->work);
      /* setup factorizer control parameters */
      sgf->updat = fi->sgf_updat;
      sgf->piv_tol = fi->sgf_piv_tol;
      sgf->piv_lim = fi->sgf_piv_lim;
      sgf->suhl = fi->sgf_suhl;
      sgf->eps_tol = fi->sgf_eps_tol;
      /* compute LU-factorization of specified matrix A */
      k = sgf_factorize(sgf, 1);
      if (k == 0)
         fi->valid = 1;
      return k;
}

void lufint_delete(LUFINT *fi)
{     /* delete interface to LU-factorization */
      SVA *sva = fi->sva;
      LUF *luf = fi->luf;
      SGF *sgf = fi->sgf;
      if (sva != NULL)
         sva_delete_area(sva);
      if (luf != NULL)
      {  tfree(luf->vr_piv);
         tfree(luf->pp_ind);
         tfree(luf->pp_inv);
         tfree(luf->qq_ind);
         tfree(luf->qq_inv);
         tfree(luf);
      }
      if (sgf != NULL)
      {  tfree(sgf->rs_head);
         tfree(sgf->rs_prev);
         tfree(sgf->rs_next);
         tfree(sgf->cs_head);
         tfree(sgf->cs_prev);
         tfree(sgf->cs_next);
         tfree(sgf->vr_max);
         tfree(sgf->flag);
         tfree(sgf->work);
         tfree(sgf);
      }
      tfree(fi);
      return;
}

/* eof */
