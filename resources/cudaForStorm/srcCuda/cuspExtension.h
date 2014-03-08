/*
 * This is an extension of the original CUSP csr_vector.h SPMV implementation.
 * It is based on the Code and incorporates changes as to cope with the details
 * of the StoRM code.
 * As this is mostly copy & past, the original license still applies.
 */

/*
 *  Copyright 2008-2009 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


#pragma once

#include <cusp/detail/device/spmv/csr_vector.h>

namespace cusp
{
namespace detail
{
namespace device
{

template <bool UseCache, unsigned int THREADS_PER_VECTOR, typename IndexType, typename ValueType>
void __storm_cuda_spmv_csr_vector(const IndexType num_rows, const IndexType * matrixRowIndices, const IndexType * matrixColumnIndices, const ValueType * matrixValues, const ValueType* x, ValueType* y)
{
    const size_t THREADS_PER_BLOCK  = 128;
    const size_t VECTORS_PER_BLOCK  = THREADS_PER_BLOCK / THREADS_PER_VECTOR;

    const size_t MAX_BLOCKS = cusp::detail::device::arch::max_active_blocks(spmv_csr_vector_kernel<IndexType, ValueType, VECTORS_PER_BLOCK, THREADS_PER_VECTOR, UseCache>, THREADS_PER_BLOCK, (size_t) 0);
    const size_t NUM_BLOCKS = std::min<size_t>(MAX_BLOCKS, DIVIDE_INTO(num_rows, VECTORS_PER_BLOCK));
    
    if (UseCache)
        bind_x(x);

    spmv_csr_vector_kernel<IndexType, ValueType, VECTORS_PER_BLOCK, THREADS_PER_VECTOR, UseCache> <<<NUM_BLOCKS, THREADS_PER_BLOCK>>> 
        (num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y);

    if (UseCache)
        unbind_x(x);
}

template <typename IndexType, typename ValueType>
void storm_cuda_spmv_csr_vector(const IndexType num_rows, const IndexType num_entries, const IndexType * matrixRowIndices, const IndexType * matrixColumnIndices, const ValueType * matrixValues, const ValueType* x, ValueType* y)
{
    const IndexType nnz_per_row = num_entries / num_rows;

    if (nnz_per_row <=  2) { __storm_cuda_spmv_csr_vector<false, 2>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <=  4) { __storm_cuda_spmv_csr_vector<false, 4>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <=  8) { __storm_cuda_spmv_csr_vector<false, 8>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <= 16) { __storm_cuda_spmv_csr_vector<false,16>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    
    __storm_cuda_spmv_csr_vector<false,32>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y);
}

template <typename IndexType, typename ValueType>
void storm_cuda_spmv_csr_vector_tex(const IndexType num_rows, const IndexType num_entries, const IndexType * matrixRowIndices, const IndexType * matrixColumnIndices, const ValueType * matrixValues, const ValueType* x, ValueType* y)
{
    const IndexType nnz_per_row = num_entries / num_rows;

    if (nnz_per_row <=  2) { __storm_cuda_spmv_csr_vector<true, 2>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <=  4) { __storm_cuda_spmv_csr_vector<true, 4>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <=  8) { __storm_cuda_spmv_csr_vector<true, 8>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <= 16) { __storm_cuda_spmv_csr_vector<true,16>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }

    __storm_cuda_spmv_csr_vector<true,32>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y);
}

} // end namespace device
} // end namespace detail
} // end namespace cusp