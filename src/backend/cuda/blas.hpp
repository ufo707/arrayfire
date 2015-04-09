/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <af/defines.h>
#include <af/blas.h>
#include <Array.hpp>

namespace cuda
{

template<typename T>
Array<T> matmul(const Array<T> &lhs, const Array<T> &rhs,
                af_blas_transpose optLhs, af_blas_transpose optRhs);

template<typename T>
Array<T> dot(const Array<T> &lhs, const Array<T> &rhs,
             af_blas_transpose optLhs, af_blas_transpose optRhs);

template<typename T>
void trsm(const Array<T> &lhs, Array<T> &rhs, af_blas_transpose trans);

}
