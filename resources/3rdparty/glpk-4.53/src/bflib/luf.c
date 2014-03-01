/* luf.c (sparse LU-factorization) */

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
#include "luf.h"

/***********************************************************************
*  luf_check_all - check LU-factorization before k-th elimination step
*
*  This routine checks that before performing k-th elimination step,
*  1 <= k <= n+1, all components of the LU-factorization are correct.
*
*  In case of k = n+1, i.e. after last elimination step, it is assumed
*  that rows of F and columns of V are *not* built yet.
*
*  NOTE: For testing/debugging only. */

void luf_check_all(LUF *luf, int k)
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int fr_ref = luf->fr_ref;
      int *fr_len = &sva->len[fr_ref-1];
      int fc_ref = luf->fc_ref;
      int *fc_ptr = &sva->ptr[fc_ref-1];
      int *fc_len = &sva->len[fc_ref-1];
      int vr_ref = luf->vr_ref;
      int *vr_ptr = &sva->ptr[vr_ref-1];
      int *vr_len = &sva->len[vr_ref-1];
      int vc_ref = luf->vc_ref;
      int *vc_ptr = &sva->ptr[vc_ref-1];
      int *vc_len = &sva->len[vc_ref-1];
      int *pp_ind = luf->pp_ind;
      int *pp_inv = luf->pp_inv;
      int *qq_ind = luf->qq_ind;
      int *qq_inv = luf->qq_inv;
      int i, ii, i_ptr, i_end, j, jj, j_ptr, j_end;
      xassert(n > 0);
      xassert(1 <= k && k <= n+1);
      /* check permutation matrix P */
      for (i = 1; i <= n; i++)
      {  ii = pp_ind[i];
         xassert(1 <= ii && ii <= n);
         xassert(pp_inv[ii] == i);
      }
      /* check permutation matrix Q */
      for (j = 1; j <= n; j++)
      {  jj = qq_inv[j];
         xassert(1 <= jj && jj <= n);
         xassert(qq_ind[jj] == j);
      }
      /* check row-wise representation of matrix F */
      for (i = 1; i <= n; i++)
         xassert(fr_len[i] == 0);
      /* check column-wise representation of matrix F */
      for (j = 1; j <= n; j++)
      {  /* j-th column of F = jj-th column of L */
         jj = pp_ind[j];
         if (jj < k)
         {  j_ptr = fc_ptr[j];
            j_end = j_ptr + fc_len[j];
            for (; j_ptr < j_end; j_ptr++)
            {  i = sv_ind[j_ptr];
               xassert(1 <= i && i <= n);
               ii = pp_ind[i]; /* f[i,j] = l[ii,jj] */
               xassert(ii > jj);
               xassert(sv_val[j_ptr] != 0.0);
            }
         }
         else /* jj >= k */
            xassert(fc_len[j] == 0);
      }
      /* check row-wise representation of matrix V */
      for (i = 1; i <= n; i++)
      {  /* i-th row of V = ii-th row of U */
         ii = pp_ind[i];
         i_ptr = vr_ptr[i];
         i_end = i_ptr + vr_len[i];
         for (; i_ptr < i_end; i_ptr++)
         {  j = sv_ind[i_ptr];
            xassert(1 <= j && j <= n);
            jj = qq_inv[j]; /* v[i,j] = u[ii,jj] */
            if (ii < k)
               xassert(jj > ii);
            else /* ii >= k */
            {  xassert(jj >= k);
               /* find v[i,j] in j-th column */
               j_ptr = vc_ptr[j];
               j_end = j_ptr + vc_len[j];
               for (; sv_ind[j_ptr] != i; j_ptr++)
                  /* nop */;
               xassert(j_ptr < j_end);
            }
            xassert(sv_val[i_ptr] != 0.0);
         }
      }
      /* check column-wise representation of matrix V */
      for (j = 1; j <= n; j++)
      {  /* j-th column of V = jj-th column of U */
         jj = qq_inv[j];
         if (jj < k)
            xassert(vc_len[j] == 0);
         else /* jj >= k */
         {  j_ptr = vc_ptr[j];
            j_end = j_ptr + vc_len[j];
            for (; j_ptr < j_end; j_ptr++)
            {  i = sv_ind[j_ptr];
               ii = pp_ind[i]; /* v[i,j] = u[ii,jj] */
               xassert(ii >= k);
               /* find v[i,j] in i-th row */
               i_ptr = vr_ptr[i];
               i_end = i_ptr + vr_len[i];
               for (; sv_ind[i_ptr] != j; i_ptr++)
                  /* nop */;
               xassert(i_ptr < i_end);
            }
         }
      }
      return;
}

/***********************************************************************
*  luf_build_v_rows - build matrix V in row-wise format
*
*  This routine builds the row-wise representation of matrix V in the
*  left part of SVA using its column-wise representation.
*
*  NOTE: On entry to the routine all rows of matrix V should have zero
*        capacity.
*
*  The working array len should have at least 1+n elements (len[0] is
*  not used). */

void luf_build_v_rows(LUF *luf, int len[/*1+n*/])
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int vr_ref = luf->vr_ref;
      int *vr_ptr = &sva->ptr[vr_ref-1];
      int *vr_len = &sva->len[vr_ref-1];
      int vc_ref = luf->vc_ref;
      int *vc_ptr = &sva->ptr[vc_ref-1];
      int *vc_len = &sva->len[vc_ref-1];
      int i, j, end, nnz, ptr, ptr1;
      /* calculate the number of non-zeros in each row of matrix V and
       * the total number of non-zeros */
      nnz = 0;
      for (i = 1; i <= n; i++)
         len[i] = 0;
      for (j = 1; j <= n; j++)
      {  nnz += vc_len[j];
         for (end = (ptr = vc_ptr[j]) + vc_len[j]; ptr < end; ptr++)
            len[sv_ind[ptr]]++;
      }
      /* we need at least nnz free locations in SVA */
      if (sva->r_ptr - sva->m_ptr < nnz)
      {  sva_more_space(sva, nnz);
         sv_ind = sva->ind;
         sv_val = sva->val;
      }
      /* reserve locations for rows of matrix V */
      for (i = 1; i <= n; i++)
      {  if (len[i] > 0)
            sva_enlarge_cap(sva, vr_ref-1+i, len[i], 0);
         vr_len[i] = len[i];
      }
      /* walk thru column of matrix V and build its rows */
      for (j = 1; j <= n; j++)
      {  for (end = (ptr = vc_ptr[j]) + vc_len[j]; ptr < end; ptr++)
         {  i = sv_ind[ptr];
            sv_ind[ptr1 = vr_ptr[i] + (--len[i])] = j;
            sv_val[ptr1] = sv_val[ptr];
         }
      }
      return;
}

/***********************************************************************
*  luf_build_f_rows - build matrix F in row-wise format
*
*  This routine builds the row-wise representation of matrix F in the
*  right part of SVA using its column-wise representation.
*
*  NOTE: On entry to the routine all rows of matrix F should have zero
*        capacity.
*
*  The working array len should have at least 1+n elements (len[0] is
*  not used). */

void luf_build_f_rows(LUF *luf, int len[/*1+n*/])
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int fr_ref = luf->fr_ref;
      int *fr_ptr = &sva->ptr[fr_ref-1];
      int *fr_len = &sva->len[fr_ref-1];
      int fc_ref = luf->fc_ref;
      int *fc_ptr = &sva->ptr[fc_ref-1];
      int *fc_len = &sva->len[fc_ref-1];
      int i, j, end, nnz, ptr, ptr1;
      /* calculate the number of non-zeros in each row of matrix F and
       * the total number of non-zeros (except diagonal elements) */
      nnz = 0;
      for (i = 1; i <= n; i++)
         len[i] = 0;
      for (j = 1; j <= n; j++)
      {  nnz += fc_len[j];
         for (end = (ptr = fc_ptr[j]) + fc_len[j]; ptr < end; ptr++)
            len[sv_ind[ptr]]++;
      }
      /* we need at least nnz free locations in SVA */
      if (sva->r_ptr - sva->m_ptr < nnz)
      {  sva_more_space(sva, nnz);
         sv_ind = sva->ind;
         sv_val = sva->val;
      }
      /* reserve locations for rows of matrix F */
      for (i = 1; i <= n; i++)
      {  if (len[i] > 0)
            sva_reserve_cap(sva, fr_ref-1+i, len[i]);
         fr_len[i] = len[i];
      }
      /* walk through columns of matrix F and build its rows */
      for (j = 1; j <= n; j++)
      {  for (end = (ptr = fc_ptr[j]) + fc_len[j]; ptr < end; ptr++)
         {  i = sv_ind[ptr];
            sv_ind[ptr1 = fr_ptr[i] + (--len[i])] = j;
            sv_val[ptr1] = sv_val[ptr];
         }
      }
      return;
}

/***********************************************************************
*  luf_build_v_cols - build matrix V in column-wise format
*
*  This routine builds the column-wise representation of matrix V in
*  the left (if the flag updat is set) or right (if the flag updat is
*  clear) part of SVA using its row-wise representation.
*
*  NOTE: On entry to the routine all columns of matrix V should have
*        zero capacity.
*
*  The working array len should have at least 1+n elements (len[0] is
*  not used). */

void luf_build_v_cols(LUF *luf, int updat, int len[/*1+n*/])
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int vr_ref = luf->vr_ref;
      int *vr_ptr = &sva->ptr[vr_ref-1];
      int *vr_len = &sva->len[vr_ref-1];
      int vc_ref = luf->vc_ref;
      int *vc_ptr = &sva->ptr[vc_ref-1];
      int *vc_len = &sva->len[vc_ref-1];
      int i, j, end, nnz, ptr, ptr1;
      /* calculate the number of non-zeros in each column of matrix V
       * and the total number of non-zeros (except pivot elements) */
      nnz = 0;
      for (j = 1; j <= n; j++)
         len[j] = 0;
      for (i = 1; i <= n; i++)
      {  nnz += vr_len[i];
         for (end = (ptr = vr_ptr[i]) + vr_len[i]; ptr < end; ptr++)
            len[sv_ind[ptr]]++;
      }
      /* we need at least nnz free locations in SVA */
      if (sva->r_ptr - sva->m_ptr < nnz)
      {  sva_more_space(sva, nnz);
         sv_ind = sva->ind;
         sv_val = sva->val;
      }
      /* reserve locations for columns of matrix V */
      for (j = 1; j <= n; j++)
      {  if (len[j] > 0)
         {  if (updat)
               sva_enlarge_cap(sva, vc_ref-1+j, len[j], 0);
            else
               sva_reserve_cap(sva, vc_ref-1+j, len[j]);
         }
         vc_len[j] = len[j];
      }
      /* walk through rows of matrix V and build its columns */
      for (i = 1; i <= n; i++)
      {  for (end = (ptr = vr_ptr[i]) + vr_len[i]; ptr < end; ptr++)
         {  j = sv_ind[ptr];
            sv_ind[ptr1 = vc_ptr[j] + (--len[j])] = i;
            sv_val[ptr1] = sv_val[ptr];
         }
      }
      return;
}

/***********************************************************************
*  luf_check_f_rc - check rows and columns of matrix F
*
*  This routine checks that the row- and column-wise representations
*  of matrix F are identical.
*
*  NOTE: For testing/debugging only. */

void luf_check_f_rc(LUF *luf)
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int fr_ref = luf->fr_ref;
      int *fr_ptr = &sva->ptr[fr_ref-1];
      int *fr_len = &sva->len[fr_ref-1];
      int fc_ref = luf->fc_ref;
      int *fc_ptr = &sva->ptr[fc_ref-1];
      int *fc_len = &sva->len[fc_ref-1];
      int i, i_end, i_ptr, j, j_end, j_ptr;
      /* walk thru rows of matrix F */
      for (i = 1; i <= n; i++)
      {  for (i_end = (i_ptr = fr_ptr[i]) + fr_len[i];
            i_ptr < i_end; i_ptr++)
         {  j = sv_ind[i_ptr];
            /* find element f[i,j] in j-th column of matrix F */
            for (j_end = (j_ptr = fc_ptr[j]) + fc_len[j];
               sv_ind[j_ptr] != i; j_ptr++)
               /* nop */;
            xassert(j_ptr < j_end);
            xassert(sv_val[i_ptr] == sv_val[j_ptr]);
            /* mark element f[i,j] */
            sv_ind[j_ptr] = -i;
         }
      }
      /* walk thru column of matix F and check that all elements has
         been marked */
      for (j = 1; j <= n; j++)
      {  for (j_end = (j_ptr = fc_ptr[j]) + fc_len[j];
            j_ptr < j_end; j_ptr++)
         {  xassert((i = sv_ind[j_ptr]) < 0);
            /* unmark element f[i,j] */
            sv_ind[j_ptr] = -i;
         }
      }
      return;
}

/***********************************************************************
*  luf_check_v_rc - check rows and columns of matrix V
*
*  This routine checks that the row- and column-wise representations
*  of matrix V are identical.
*
*  NOTE: For testing/debugging only. */

void luf_check_v_rc(LUF *luf)
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int vr_ref = luf->vr_ref;
      int *vr_ptr = &sva->ptr[vr_ref-1];
      int *vr_len = &sva->len[vr_ref-1];
      int vc_ref = luf->vc_ref;
      int *vc_ptr = &sva->ptr[vc_ref-1];
      int *vc_len = &sva->len[vc_ref-1];
      int i, i_end, i_ptr, j, j_end, j_ptr;
      /* walk thru rows of matrix V */
      for (i = 1; i <= n; i++)
      {  for (i_end = (i_ptr = vr_ptr[i]) + vr_len[i];
            i_ptr < i_end; i_ptr++)
         {  j = sv_ind[i_ptr];
            /* find element v[i,j] in j-th column of matrix V */
            for (j_end = (j_ptr = vc_ptr[j]) + vc_len[j];
               sv_ind[j_ptr] != i; j_ptr++)
               /* nop */;
            xassert(j_ptr < j_end);
            xassert(sv_val[i_ptr] == sv_val[j_ptr]);
            /* mark element v[i,j] */
            sv_ind[j_ptr] = -i;
         }
      }
      /* walk thru column of matix V and check that all elements has
         been marked */
      for (j = 1; j <= n; j++)
      {  for (j_end = (j_ptr = vc_ptr[j]) + vc_len[j];
            j_ptr < j_end; j_ptr++)
         {  xassert((i = sv_ind[j_ptr]) < 0);
            /* unmark element v[i,j] */
            sv_ind[j_ptr] = -i;
         }
      }
      return;
}

/***********************************************************************
*  luf_f_solve - solve system F * x = b
*
*  This routine solves the system F * x = b, where the matrix F is the
*  left factor of the sparse LU-factorization.
*
*  On entry the array x should contain elements of the right-hand side
*  vector b in locations x[1], ..., x[n], where n is the order of the
*  matrix F. On exit this array will contain elements of the solution
*  vector x in the same locations. */

void luf_f_solve(LUF *luf, double x[/*1+n*/])
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int fc_ref = luf->fc_ref;
      int *fc_ptr = &sva->ptr[fc_ref-1];
      int *fc_len = &sva->len[fc_ref-1];
      int *pp_inv = luf->pp_inv;
      int j, k, ptr, end;
      double x_j;
      for (k = 1; k <= n; k++)
      {  /* k-th column of L = j-th column of F */
         j = pp_inv[k];
         /* x[j] is already computed */
         /* walk thru j-th column of matrix F and substitute x[j] into
          * other equations */
         if ((x_j = x[j]) != 0.0)
         {  for (end = (ptr = fc_ptr[j]) + fc_len[j];
               ptr < end; ptr++)
               x[sv_ind[ptr]] -= sv_val[ptr] * x_j;
         }
      }
      return;
}

/***********************************************************************
*  luf_ft_solve - solve system F' * x = b
*
*  This routine solves the system F' * x = b, where F' is a matrix
*  transposed to the matrix F, which is the left factor of the sparse
*  LU-factorization.
*
*  On entry the array x should contain elements of the right-hand side
*  vector b in locations x[1], ..., x[n], where n is the order of the
*  matrix F. On exit this array will contain elements of the solution
*  vector x in the same locations. */

void luf_ft_solve(LUF *luf, double x[/*1+n*/])
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      int fr_ref = luf->fr_ref;
      int *fr_ptr = &sva->ptr[fr_ref-1];
      int *fr_len = &sva->len[fr_ref-1];
      int *pp_inv = luf->pp_inv;
      int i, k, ptr, end;
      double x_i;
      for (k = n; k >= 1; k--)
      {  /* k-th column of L' = i-th row of F */
         i = pp_inv[k];
         /* x[i] is already computed */
         /* walk thru i-th row of matrix F and substitute x[i] into
          * other equations */
         if ((x_i = x[i]) != 0.0)
         {  for (end = (ptr = fr_ptr[i]) + fr_len[i];
               ptr < end; ptr++)
               x[sv_ind[ptr]] -= sv_val[ptr] * x_i;
         }
      }
      return;
}

/***********************************************************************
*  luf_v_solve - solve system V * x = b
*
*  This routine solves the system V * x = b, where the matrix V is the
*  right factor of the sparse LU-factorization.
*
*  On entry the array b should contain elements of the right-hand side
*  vector b in locations b[1], ..., b[n], where n is the order of the
*  matrix V. On exit the array x will contain elements of the solution
*  vector x in locations x[1], ..., x[n]. Note that the array b will be
*  clobbered on exit. */

void luf_v_solve(LUF *luf, double b[/*1+n*/], double x[/*1+n*/])
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      double *vr_piv = luf->vr_piv;
      int vc_ref = luf->vc_ref;
      int *vc_ptr = &sva->ptr[vc_ref-1];
      int *vc_len = &sva->len[vc_ref-1];
      int *pp_inv = luf->pp_inv;
      int *qq_ind = luf->qq_ind;
      int i, j, k, ptr, end;
      double x_j;
      for (k = n; k >= 1; k--)
      {  /* k-th row of U = i-th row of V */
         /* k-th column of U = j-th column of V */
         i = pp_inv[k];
         j = qq_ind[k];
         /* compute x[j] = b[i] / u[k,k], where u[k,k] = v[i,j];
          * walk through j-th column of matrix V and substitute x[j]
          * into other equations */
         if ((x_j = x[j] = b[i] / vr_piv[i]) != 0.0)
         {  for (end = (ptr = vc_ptr[j]) + vc_len[j];
               ptr < end; ptr++)
               b[sv_ind[ptr]] -= sv_val[ptr] * x_j;
         }
      }
      return;
}

/***********************************************************************
*  luf_vt_solve - solve system V' * x = b
*
*  This routine solves the system V' * x = b, where V' is a matrix
*  transposed to the matrix V, which is the right factor of the sparse
*  LU-factorization.
*
*  On entry the array b should contain elements of the right-hand side
*  vector b in locations b[1], ..., b[n], where n is the order of the
*  matrix V. On exit the array x will contain elements of the solution
*  vector x in locations x[1], ..., x[n]. Note that the array b will be
*  clobbered on exit. */

void luf_vt_solve(LUF *luf, double b[/*1+n*/], double x[/*1+n*/])
{     int n = luf->n;
      SVA *sva = luf->sva;
      int *sv_ind = sva->ind;
      double *sv_val = sva->val;
      double *vr_piv = luf->vr_piv;
      int vr_ref = luf->vr_ref;
      int *vr_ptr = &sva->ptr[vr_ref-1];
      int *vr_len = &sva->len[vr_ref-1];
      int *pp_inv = luf->pp_inv;
      int *qq_ind = luf->qq_ind;
      int i, j, k, ptr, end;
      double x_i;
      for (k = 1; k <= n; k++)
      {  /* k-th row of U' = j-th column of V */
         /* k-th column of U' = i-th row of V */
         i = pp_inv[k];
         j = qq_ind[k];
         /* compute x[i] = b[j] / u'[k,k], where u'[k,k] = v[i,j];
          * walk through i-th row of matrix V and substitute x[i] into
          * other equations */
         if ((x_i = x[i] = b[j] / vr_piv[i]) != 0.0)
         {  for (end = (ptr = vr_ptr[i]) + vr_len[i];
               ptr < end; ptr++)
               b[sv_ind[ptr]] -= sv_val[ptr] * x_i;
         }
      }
      return;
}

/* eof */
