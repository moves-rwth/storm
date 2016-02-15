/* spxchuzr.h */

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

#ifndef SPXCHUZR_H
#define SPXCHUZR_H

#include "spxlp.h"

#define spx_chuzr_std _glp_spx_chuzr_std
int spx_chuzr_std(SPXLP *lp, int phase, const double beta[/*1+m*/],
      int q, double s, const double tcol[/*1+m*/], int *p_flag,
      double tol_piv, double tol, double tol1);
/* choose basic variable (textbook ratio test) */

#define spx_chuzr_harris _glp_spx_chuzr_harris
int spx_chuzr_harris(SPXLP *lp, int phase, const double beta[/*1+m*/],
      int q, double s, const double tcol[/*1+m*/], int *p_flag,
      double tol_piv, double tol, double tol1);
/* choose basic variable (Harris' ratio test) */

#endif

/* eof */
