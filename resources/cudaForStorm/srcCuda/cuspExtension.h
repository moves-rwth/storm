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
#include <limits>
#include <algorithm>

namespace cusp
{
namespace detail
{
namespace device
{

//////////////////////////////////////////////////////////////////////////////
// CSR SpMV kernels based on a vector model (one warp per row)
//////////////////////////////////////////////////////////////////////////////
//
// spmv_csr_vector_device
//   Each row of the CSR matrix is assigned to a warp.  The warp computes
//   y[i] = A[i,:] * x, i.e. the dot product of the i-th row of A with 
//   the x vector, in parallel.  This division of work implies that 
//   the CSR index and data arrays (Aj and Ax) are accessed in a contiguous
//   manner (but generally not aligned).  On GT200 these accesses are
//   coalesced, unlike kernels based on the one-row-per-thread division of 
//   work.  Since an entire 32-thread warp is assigned to each row, many 
//   threads will remain idle when their row contains a small number 
//   of elements.  This code relies on implicit synchronization among 
//   threads in a warp.
//
// spmv_csr_vector_tex_device
//   Same as spmv_csr_vector_tex_device, except that the texture cache is 
//   used for accessing the x vector.
//  
//  Note: THREADS_PER_VECTOR must be one of [2,4,8,16,32]


template <typename IndexType, typename ValueType, unsigned int VECTORS_PER_BLOCK, unsigned int THREADS_PER_VECTOR, bool UseCache>
__launch_bounds__(VECTORS_PER_BLOCK * THREADS_PER_VECTOR,1)
__global__ void
storm_cuda_opt_spmv_csr_vector_kernel(const IndexType num_rows, const IndexType * matrixRowIndices, const IndexType * matrixColumnIndicesAndValues, const ValueType * x, ValueType * y)
{
    __shared__ volatile ValueType sdata[VECTORS_PER_BLOCK * THREADS_PER_VECTOR + THREADS_PER_VECTOR / 2];  // padded to avoid reduction conditionals
    __shared__ volatile IndexType ptrs[VECTORS_PER_BLOCK][2];
    
    const IndexType THREADS_PER_BLOCK = VECTORS_PER_BLOCK * THREADS_PER_VECTOR;

    const IndexType thread_id   = THREADS_PER_BLOCK * blockIdx.x + threadIdx.x;    // global thread index
    const IndexType thread_lane = threadIdx.x & (THREADS_PER_VECTOR - 1);          // thread index within the vector
    const IndexType vector_id   = thread_id   /  THREADS_PER_VECTOR;               // global vector index
    const IndexType vector_lane = threadIdx.x /  THREADS_PER_VECTOR;               // vector index within the block
    const IndexType num_vectors = VECTORS_PER_BLOCK * gridDim.x;                   // total number of active vectors

    for(IndexType row = vector_id; row < num_rows; row += num_vectors)
    {
        // use two threads to fetch Ap[row] and Ap[row+1]
        // this is considerably faster than the straightforward version
        if(thread_lane < 2)
            ptrs[vector_lane][thread_lane] = matrixRowIndices[row + thread_lane];

        const IndexType row_start = ptrs[vector_lane][0];                   //same as: row_start = Ap[row];
        const IndexType row_end   = ptrs[vector_lane][1];                   //same as: row_end   = Ap[row+1];

        // initialize local sum
        ValueType sum = 0;
     
        if (THREADS_PER_VECTOR == 32 && row_end - row_start > 32)
        {
            // ensure aligned memory access to Aj and Ax

            IndexType jj = row_start - (row_start & (THREADS_PER_VECTOR - 1)) + thread_lane;

            // accumulate local sums
            if(jj >= row_start && jj < row_end)
                sum += matrixColumnIndicesAndValues[(2 * jj) + 1] * fetch_x<UseCache>(matrixColumnIndicesAndValues[2 * jj], x);

            // accumulate local sums
            for(jj += THREADS_PER_VECTOR; jj < row_end; jj += THREADS_PER_VECTOR)
                sum += matrixColumnIndicesAndValues[(2 * jj) + 1] * fetch_x<UseCache>(matrixColumnIndicesAndValues[2 * jj], x);
        }
        else
        {
            // accumulate local sums
            for(IndexType jj = row_start + thread_lane; jj < row_end; jj += THREADS_PER_VECTOR)
                sum += matrixColumnIndicesAndValues[(2 * jj) + 1] * fetch_x<UseCache>(matrixColumnIndicesAndValues[2 * jj], x);
        }

        // store local sum in shared memory
        sdata[threadIdx.x] = sum;
        
        // reduce local sums to row sum
        if (THREADS_PER_VECTOR > 16) sdata[threadIdx.x] = sum = sum + sdata[threadIdx.x + 16];
        if (THREADS_PER_VECTOR >  8) sdata[threadIdx.x] = sum = sum + sdata[threadIdx.x +  8];
        if (THREADS_PER_VECTOR >  4) sdata[threadIdx.x] = sum = sum + sdata[threadIdx.x +  4];
        if (THREADS_PER_VECTOR >  2) sdata[threadIdx.x] = sum = sum + sdata[threadIdx.x +  2];
        if (THREADS_PER_VECTOR >  1) sdata[threadIdx.x] = sum = sum + sdata[threadIdx.x +  1];
       
        // first thread writes the result
        if (thread_lane == 0)
            y[row] = sdata[threadIdx.x];
    }
}

template <typename IndexType, typename ValueType, unsigned int ROWS_PER_BLOCK, unsigned int THREADS_PER_ROW, bool Minimize>
__launch_bounds__(ROWS_PER_BLOCK * THREADS_PER_ROW,1)
__global__ void
storm_cuda_opt_vector_reduce_kernel(const IndexType num_rows, const IndexType * nondeterministicChoiceIndices, ValueType * x, const ValueType * y, const ValueType minMaxInitializer)
{
    __shared__ volatile ValueType sdata[ROWS_PER_BLOCK * THREADS_PER_ROW + THREADS_PER_ROW / 2];  // padded to avoid reduction conditionals
    __shared__ volatile IndexType ptrs[ROWS_PER_BLOCK][2];
    
    const IndexType THREADS_PER_BLOCK = ROWS_PER_BLOCK * THREADS_PER_ROW;

    const IndexType thread_id   = THREADS_PER_BLOCK * blockIdx.x + threadIdx.x;    // global thread index
    const IndexType thread_lane = threadIdx.x & (THREADS_PER_ROW - 1);          // thread index within the vector
    const IndexType vector_id   = thread_id   /  THREADS_PER_ROW;               // global vector index
    const IndexType vector_lane = threadIdx.x /  THREADS_PER_ROW;               // vector index within the block
    const IndexType num_vectors = ROWS_PER_BLOCK * gridDim.x;                   // total number of active vectors

    for(IndexType row = vector_id; row < num_rows; row += num_vectors)
    {
        // use two threads to fetch Ap[row] and Ap[row+1]
        // this is considerably faster than the straightforward version
        if(thread_lane < 2)
            ptrs[vector_lane][thread_lane] = nondeterministicChoiceIndices[row + thread_lane];

        const IndexType row_start = ptrs[vector_lane][0];                   //same as: row_start = Ap[row];
        const IndexType row_end   = ptrs[vector_lane][1];                   //same as: row_end   = Ap[row+1];

        // initialize local Min/Max
        ValueType localMinMaxElement = minMaxInitializer;

        if (THREADS_PER_ROW == 32 && row_end - row_start > 32)
        {
            // ensure aligned memory access to Aj and Ax

            IndexType jj = row_start - (row_start & (THREADS_PER_ROW - 1)) + thread_lane;

            // accumulate local sums
            if(jj >= row_start && jj < row_end) {
				if(Minimize) {
					localMinMaxElement = (localMinMaxElement > y[jj]) ? y[jj] : localMinMaxElement;
				} else {
					localMinMaxElement = (localMinMaxElement < y[jj]) ? y[jj] : localMinMaxElement;
				}
			}

            // accumulate local sums
            for(jj += THREADS_PER_ROW; jj < row_end; jj += THREADS_PER_ROW)
                if(Minimize) {
					localMinMaxElement = (localMinMaxElement > y[jj]) ? y[jj] : localMinMaxElement;
				} else {
					localMinMaxElement = (localMinMaxElement < y[jj]) ? y[jj] : localMinMaxElement;
				}
        }
        else
        {
            // accumulate local sums
            for(IndexType jj = row_start + thread_lane; jj < row_end; jj += THREADS_PER_ROW)
                if(Minimize) {
					localMinMaxElement = (localMinMaxElement > y[jj]) ? y[jj] : localMinMaxElement;
				} else {
					localMinMaxElement = (localMinMaxElement < y[jj]) ? y[jj] : localMinMaxElement;
				}
        }

        // store local sum in shared memory
        sdata[threadIdx.x] = localMinMaxElement;
        
        // reduce local min/max to row min/max
		if (Minimize) {
			if (THREADS_PER_ROW > 16) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 16]) ? sdata[threadIdx.x + 16] : localMinMaxElement);
			if (THREADS_PER_ROW >  8) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 8]) ? sdata[threadIdx.x + 8] : localMinMaxElement);
			if (THREADS_PER_ROW >  4) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 4]) ? sdata[threadIdx.x + 4] : localMinMaxElement);
			if (THREADS_PER_ROW >  2) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 2]) ? sdata[threadIdx.x + 2] : localMinMaxElement);
			if (THREADS_PER_ROW >  1) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 1]) ? sdata[threadIdx.x + 1] : localMinMaxElement);
		} else {
			if (THREADS_PER_ROW > 16) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 16]) ? sdata[threadIdx.x + 16] : localMinMaxElement);
			if (THREADS_PER_ROW >  8) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 8]) ? sdata[threadIdx.x + 8] : localMinMaxElement);
			if (THREADS_PER_ROW >  4) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 4]) ? sdata[threadIdx.x + 4] : localMinMaxElement);
			if (THREADS_PER_ROW >  2) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 2]) ? sdata[threadIdx.x + 2] : localMinMaxElement);
			if (THREADS_PER_ROW >  1) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 1]) ? sdata[threadIdx.x + 1] : localMinMaxElement);
		}
       
        // first thread writes the result
        if (thread_lane == 0)
            x[row] = sdata[threadIdx.x];
    }
}

template <bool Minimize, unsigned int THREADS_PER_VECTOR, typename IndexType, typename ValueType>
void __storm_cuda_opt_vector_reduce(const IndexType num_rows, const IndexType * nondeterministicChoiceIndices, ValueType * x, const ValueType * y)
{
	ValueType __minMaxInitializer = 0;
	if (Minimize) {
		__minMaxInitializer = std::numeric_limits<ValueType>::max();
	}
	const ValueType minMaxInitializer = __minMaxInitializer;

    const size_t THREADS_PER_BLOCK  = 128;
    const size_t VECTORS_PER_BLOCK  = THREADS_PER_BLOCK / THREADS_PER_VECTOR;

    const size_t MAX_BLOCKS = cusp::detail::device::arch::max_active_blocks(storm_cuda_opt_vector_reduce_kernel<IndexType, ValueType, VECTORS_PER_BLOCK, THREADS_PER_VECTOR, Minimize>, THREADS_PER_BLOCK, (size_t) 0);
    const size_t NUM_BLOCKS = std::min<size_t>(MAX_BLOCKS, DIVIDE_INTO(num_rows, VECTORS_PER_BLOCK));

    storm_cuda_opt_vector_reduce_kernel<IndexType, ValueType, VECTORS_PER_BLOCK, THREADS_PER_VECTOR, Minimize> <<<NUM_BLOCKS, THREADS_PER_BLOCK>>> 
        (num_rows, nondeterministicChoiceIndices, x, y, minMaxInitializer);
}

template <bool Minimize, typename IndexType, typename ValueType>
void storm_cuda_opt_vector_reduce(const IndexType num_rows, const IndexType num_entries, const IndexType * nondeterministicChoiceIndices, ValueType * x, const ValueType * y)
{
    const IndexType rows_per_group = num_entries / num_rows;

    if (rows_per_group <=  2) { __storm_cuda_opt_vector_reduce<Minimize, 2>(num_rows, nondeterministicChoiceIndices, x, y); return; }
    if (rows_per_group <=  4) { __storm_cuda_opt_vector_reduce<Minimize, 4>(num_rows, nondeterministicChoiceIndices, x, y); return; }
    if (rows_per_group <=  8) { __storm_cuda_opt_vector_reduce<Minimize, 8>(num_rows, nondeterministicChoiceIndices, x, y); return; }
    if (rows_per_group <= 16) { __storm_cuda_opt_vector_reduce<Minimize,16>(num_rows, nondeterministicChoiceIndices, x, y); return; }
    
    __storm_cuda_opt_vector_reduce<Minimize,32>(num_rows, nondeterministicChoiceIndices, x, y);
}

template <bool UseCache, unsigned int THREADS_PER_VECTOR, typename IndexType, typename ValueType>
void __storm_cuda_opt_spmv_csr_vector(const IndexType num_rows, const IndexType * matrixRowIndices, const IndexType * matrixColumnIndicesAndValues, const ValueType* x, ValueType* y)
{
    const size_t THREADS_PER_BLOCK  = 128;
    const size_t VECTORS_PER_BLOCK  = THREADS_PER_BLOCK / THREADS_PER_VECTOR;

    const size_t MAX_BLOCKS = cusp::detail::device::arch::max_active_blocks(storm_cuda_opt_spmv_csr_vector_kernel<IndexType, ValueType, VECTORS_PER_BLOCK, THREADS_PER_VECTOR, UseCache>, THREADS_PER_BLOCK, (size_t) 0);
    const size_t NUM_BLOCKS = std::min<size_t>(MAX_BLOCKS, DIVIDE_INTO(num_rows, VECTORS_PER_BLOCK));
    
    if (UseCache)
        bind_x(x);

    storm_cuda_opt_spmv_csr_vector_kernel<IndexType, ValueType, VECTORS_PER_BLOCK, THREADS_PER_VECTOR, UseCache> <<<NUM_BLOCKS, THREADS_PER_BLOCK>>> 
        (num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y);

    if (UseCache)
        unbind_x(x);
}

template <typename IndexType, typename ValueType>
void storm_cuda_opt_spmv_csr_vector(const IndexType num_rows, const IndexType num_entries, const IndexType * matrixRowIndices, const IndexType * matrixColumnIndicesAndValues, const ValueType* x, ValueType* y)
{
    const IndexType nnz_per_row = num_entries / num_rows;

    if (nnz_per_row <=  2) { __storm_cuda_opt_spmv_csr_vector<false, 2>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <=  4) { __storm_cuda_opt_spmv_csr_vector<false, 4>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <=  8) { __storm_cuda_opt_spmv_csr_vector<false, 8>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <= 16) { __storm_cuda_opt_spmv_csr_vector<false,16>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    
    __storm_cuda_opt_spmv_csr_vector<false,32>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y);
}

template <typename IndexType, typename ValueType>
void storm_cuda_opt_spmv_csr_vector_tex(const IndexType num_rows, const IndexType num_entries, const IndexType * matrixRowIndices, const IndexType * matrixColumnIndicesAndValues, const ValueType* x, ValueType* y)
{
    const IndexType nnz_per_row = num_entries / num_rows;

    if (nnz_per_row <=  2) { __storm_cuda_opt_spmv_csr_vector<true, 2>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <=  4) { __storm_cuda_opt_spmv_csr_vector<true, 4>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <=  8) { __storm_cuda_opt_spmv_csr_vector<true, 8>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <= 16) { __storm_cuda_opt_spmv_csr_vector<true,16>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }

    __storm_cuda_opt_spmv_csr_vector<true,32>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y);
}

// NON-OPT

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