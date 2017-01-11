// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2010-2014 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_LAPACK_COMMON_H
#define STORMEIGEN_LAPACK_COMMON_H

#include "../blas/common.h"

#define STORMEIGEN_LAPACK_FUNC(FUNC,ARGLIST)               \
  extern "C" { int STORMEIGEN_BLAS_FUNC(FUNC) ARGLIST; }   \
  int STORMEIGEN_BLAS_FUNC(FUNC) ARGLIST

typedef StormEigen::Map<StormEigen::Transpositions<Eigen::Dynamic,Eigen::Dynamic,int> > PivotsType;

#if ISCOMPLEX
#define STORMEIGEN_LAPACK_ARG_IF_COMPLEX(X) X,
#else
#define STORMEIGEN_LAPACK_ARG_IF_COMPLEX(X)
#endif


#endif // STORMEIGEN_LAPACK_COMMON_H
