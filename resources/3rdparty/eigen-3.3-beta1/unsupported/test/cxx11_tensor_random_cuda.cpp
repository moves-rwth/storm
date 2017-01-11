// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2014 Benoit Steiner <benoit.steiner.goog@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#define STORMEIGEN_TEST_NO_LONGDOUBLE
#define STORMEIGEN_TEST_NO_COMPLEX
#define STORMEIGEN_TEST_FUNC cxx11_tensor_random_cuda
#define STORMEIGEN_DEFAULT_DENSE_INDEX_TYPE int
#define STORMEIGEN_USE_GPU

#include "main.h"
#include <StormEigen/CXX11/Tensor>

static void test_default()
{
  Tensor<std::complex<float>, 1> vec(6);
  vec.setRandom();

  // Fixme: we should check that the generated numbers follow a uniform
  // distribution instead.
  for (int i = 1; i < 6; ++i) {
    VERIFY_IS_NOT_EQUAL(vec(i), vec(i-1));
  }
}


void test_cxx11_tensor_random_cuda()
{
  CALL_SUBTEST(test_default());
}
