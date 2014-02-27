/* fhvint.c (interface to FHV-factorization) */

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
#include "fhvint.h"

FHVINT *fhvint_create(void)
{     /* create interface to FHV-factorization */
      FHVINT *fi;
      fi = talloc(1, FHVINT);
      fi->valid = 0;
      fi->fhv = NULL;
      fi->lufint = NULL;
      fi->nfs_max = 0;
      return fi;
}

int fhvint_factorize(FHVINT *fi, int n, int (*col)(void *info, int j,
      int ind[], double val[]), void *info)
{     /* compute FHV-factorization of specified matrix A */
      FHV *fhv;
      LUFINT *lufint;
      int nfs_max, old_n_max, n_max, k, ret;
      xassert(n > 0);
      fi->valid = 0;
      /* get required value of nfs_max */
      nfs_max = fi->nfs_max;
      if (nfs_max == 0)
         nfs_max = 100;
      xassert(nfs_max > 0);
      /* create interface to LU-factorization, if necessary */
      lufint = fi->lufint;
      if (lufint == NULL)
      {  lufint = fi->lufint = lufint_create();
         lufint->sva_n_max = 4 * n + nfs_max;
         lufint->sva_size = 10 * n;
         lufint->delta_n0 = 0;
         lufint->delta_n = 100;
         lufint->sgf_updat = 1;
      }
      /* compute LU-factorization of specified matrix A */
      old_n_max = lufint->n_max;
      ret = lufint_factorize(lufint, n, col, info);
      n_max = lufint->n_max;
      /* create FHV-factorization, if necessary */
      fhv = fi->fhv;
      if (fhv == NULL)
      {  fhv = fi->fhv = talloc(1, FHV);
         fhv->luf = lufint->luf;
         fhv->nfs_max = 0;
         fhv->hh_ind = NULL;
         fhv->p0_ind = NULL;
         fhv->p0_inv = NULL;
      }
      /* allocate/reallocate FHV-factorization, if necessary */
      if (fhv->nfs_max != nfs_max)
      {  fhv->nfs_max = nfs_max;
         if (fhv->hh_ind != NULL)
            tfree(fhv->hh_ind);
         fhv->hh_ind = talloc(1+nfs_max, int);
      }
      if (old_n_max < n_max)
      {  if (fhv->p0_ind != NULL)
            tfree(fhv->p0_ind);
         if (fhv->p0_inv != NULL)
            tfree(fhv->p0_inv);
         fhv->p0_ind = talloc(1+n_max, int);
         fhv->p0_inv = talloc(1+n_max, int);
      }
      /* H := I */
      fhv->nfs = 0;
      fhv->hh_ref = sva_alloc_vecs(fi->lufint->sva, nfs_max);
      /* P0 := P */
      for (k = 1; k <= n; k++)
      {  fhv->p0_ind[k] = fi->lufint->luf->pp_ind[k];
         fhv->p0_inv[k] = fi->lufint->luf->pp_inv[k];
      }
      /* set validation flag */
      if (ret == 0)
         fi->valid = 1;
      return ret;
}

int fhvint_update(FHVINT *fi, int j, int len, const int ind[],
      const double val[])
{     /* update FHV-factorization after replacing j-th column of A */
      SGF *sgf = fi->lufint->sgf;
      int *ind1 = sgf->rs_next;
      double *val1 = sgf->vr_max;
      double *work = sgf->work;
      int ret;
      xassert(fi->valid);
      ret = fhv_ft_update(fi->fhv, j, len, ind, val, ind1, val1, work);
      if (ret != 0)
         fi->valid = 0;
      return ret;
}

void fhvint_ftran(FHVINT *fi, double x[])
{     /* solve system A * x = b */
      FHV *fhv = fi->fhv;
      LUF *luf = fhv->luf;
      int n = luf->n;
      int *pp_ind = luf->pp_ind;
      int *pp_inv = luf->pp_inv;
      SGF *sgf = fi->lufint->sgf;
      double *work = sgf->work;
      xassert(fi->valid);
      /* A = F * H * V */
      /* x = inv(A) * b = inv(V) * inv(H) * inv(F) * b */
      luf->pp_ind = fhv->p0_ind;
      luf->pp_inv = fhv->p0_inv;
      luf_f_solve(luf, x);
      luf->pp_ind = pp_ind;
      luf->pp_inv = pp_inv;
      fhv_h_solve(fhv, x);
      luf_v_solve(luf, x, work);
      memcpy(&x[1], &work[1], n * sizeof(double));
      return;
}

void fhvint_btran(FHVINT *fi, double x[])
{     /* solve system A'* x = b */
      FHV *fhv = fi->fhv;
      LUF *luf = fhv->luf;
      int n = luf->n;
      int *pp_ind = luf->pp_ind;
      int *pp_inv = luf->pp_inv;
      SGF *sgf = fi->lufint->sgf;
      double *work = sgf->work;
      xassert(fi->valid);
      /* A' = (F * H * V)' = V'* H'* F' */
      /* x = inv(A') * b = inv(F') * inv(H') * inv(V') * b */
      luf_vt_solve(luf, x, work);
      fhv_ht_solve(fhv, work);
      luf->pp_ind = fhv->p0_ind;
      luf->pp_inv = fhv->p0_inv;
      luf_ft_solve(luf, work);
      luf->pp_ind = pp_ind;
      luf->pp_inv = pp_inv;
      memcpy(&x[1], &work[1], n * sizeof(double));
      return;
}

void fhvint_delete(FHVINT *fi)
{     /* delete interface to FHV-factorization */
      FHV *fhv = fi->fhv;
      LUFINT *lufint = fi->lufint;
      if (fhv != NULL)
      {  tfree(fhv->hh_ind);
         tfree(fhv->p0_ind);
         tfree(fhv->p0_inv);
         tfree(fhv);
      }
      if (lufint != NULL)
         lufint_delete(fi->lufint);
      tfree(fi);
      return;
}

/* eof */
