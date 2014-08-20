/*
 * This is an extension of the original CUSP csr_vector.h SPMV implementation.
 * It is based on the Code and incorporates changes as to cope with the details
 * of the StoRM code.
 * As this is mostly copy & paste, the original license still applies.
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

#include <limits>
#include <cstdint>
#include <algorithm>

#include <math_functions.h>

#include <cusp/detail/device/spmv/csr_vector.h>

#include "storm-cudaplugin-config.h"

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


template <unsigned int VECTORS_PER_BLOCK, unsigned int THREADS_PER_VECTOR, bool UseCache>
__launch_bounds__(VECTORS_PER_BLOCK * THREADS_PER_VECTOR,1)
__global__ void
storm_cuda_opt_spmv_csr_vector_kernel_float(const uint_fast64_t num_rows, const uint_fast64_t * matrixRowIndices, const float * matrixColumnIndicesAndValues, const float * x, float * y)
{
    __shared__ volatile float sdata[VECTORS_PER_BLOCK * THREADS_PER_VECTOR + THREADS_PER_VECTOR / 2];  // padded to avoid reduction conditionals
    __shared__ volatile uint_fast64_t ptrs[VECTORS_PER_BLOCK][2];
    
    const uint_fast64_t THREADS_PER_BLOCK = VECTORS_PER_BLOCK * THREADS_PER_VECTOR;

    const uint_fast64_t thread_id   = THREADS_PER_BLOCK * blockIdx.x + threadIdx.x;    // global thread index
    const uint_fast64_t thread_lane = threadIdx.x & (THREADS_PER_VECTOR - 1);          // thread index within the vector
    const uint_fast64_t vector_id   = thread_id   /  THREADS_PER_VECTOR;               // global vector index
    const uint_fast64_t vector_lane = threadIdx.x /  THREADS_PER_VECTOR;               // vector index within the block
    const uint_fast64_t num_vectors = VECTORS_PER_BLOCK * gridDim.x;                   // total number of active vectors

    for(uint_fast64_t row = vector_id; row < num_rows; row += num_vectors)
    {
        // use two threads to fetch Ap[row] and Ap[row+1]
        // this is considerably faster than the straightforward version
        if(thread_lane < 2)
            ptrs[vector_lane][thread_lane] = matrixRowIndices[row + thread_lane];

        const uint_fast64_t row_start = ptrs[vector_lane][0];                   //same as: row_start = Ap[row];
        const uint_fast64_t row_end   = ptrs[vector_lane][1];                   //same as: row_end   = Ap[row+1];

        // initialize local sum
        float sum = 0;
     
        if (THREADS_PER_VECTOR == 32 && row_end - row_start > 32)
        {
            // ensure aligned memory access to Aj and Ax

            uint_fast64_t jj = row_start - (row_start & (THREADS_PER_VECTOR - 1)) + thread_lane;

            // accumulate local sums
            if(jj >= row_start && jj < row_end) {
#ifdef STORM_CUDAPLUGIN_HAVE_64BIT_FLOAT_ALIGNMENT
				sum += matrixColumnIndicesAndValues[4 * jj + 2] * fetch_x<UseCache>(*reinterpret_cast<uint_fast64_t const*>(matrixColumnIndicesAndValues + 4 * jj), x);
#else
				sum += matrixColumnIndicesAndValues[3 * jj + 2] * fetch_x<UseCache>(*reinterpret_cast<uint_fast64_t const*>(matrixColumnIndicesAndValues + 3 * jj), x);
#endif
                //sum += reinterpret_cast<ValueType const*>(matrixColumnIndicesAndValues)[2*jj + 1] * fetch_x<UseCache>(matrixColumnIndicesAndValues[2*jj], x);
			}
				
            // accumulate local sums
            for(jj += THREADS_PER_VECTOR; jj < row_end; jj += THREADS_PER_VECTOR) {
                //sum += reinterpret_cast<ValueType const*>(matrixColumnIndicesAndValues)[2*jj + 1] * fetch_x<UseCache>(matrixColumnIndicesAndValues[2*jj], x);
#ifdef STORM_CUDAPLUGIN_HAVE_64BIT_FLOAT_ALIGNMENT
				sum += matrixColumnIndicesAndValues[4 * jj + 2] * fetch_x<UseCache>(*reinterpret_cast<uint_fast64_t const*>(matrixColumnIndicesAndValues + 4 * jj), x);
#else
				sum += matrixColumnIndicesAndValues[3 * jj + 2] * fetch_x<UseCache>(*reinterpret_cast<uint_fast64_t const*>(matrixColumnIndicesAndValues + 3 * jj), x);
#endif
			}
        } else {
            // accumulate local sums
            for(uint_fast64_t jj = row_start + thread_lane; jj < row_end; jj += THREADS_PER_VECTOR) {
                //sum += reinterpret_cast<ValueType const*>(matrixColumnIndicesAndValues)[2*jj + 1] * fetch_x<UseCache>(matrixColumnIndicesAndValues[2*jj], x);
#ifdef STORM_CUDAPLUGIN_HAVE_64BIT_FLOAT_ALIGNMENT
				sum += matrixColumnIndicesAndValues[4 * jj + 2] * fetch_x<UseCache>(*reinterpret_cast<uint_fast64_t const*>(matrixColumnIndicesAndValues + 4 * jj), x);
#else
				sum += matrixColumnIndicesAndValues[3 * jj + 2] * fetch_x<UseCache>(*reinterpret_cast<uint_fast64_t const*>(matrixColumnIndicesAndValues + 3 * jj), x);
#endif
			}
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

template <unsigned int ROWS_PER_BLOCK, unsigned int THREADS_PER_ROW, bool Minimize>
__launch_bounds__(ROWS_PER_BLOCK * THREADS_PER_ROW,1)
__global__ void
storm_cuda_opt_vector_reduce_kernel_float(const uint_fast64_t num_rows, const uint_fast64_t * nondeterministicChoiceIndices, float * x, const float * y, const float minMaxInitializer)
{
    __shared__ volatile float sdata[ROWS_PER_BLOCK * THREADS_PER_ROW + THREADS_PER_ROW / 2];  // padded to avoid reduction conditionals
    __shared__ volatile uint_fast64_t ptrs[ROWS_PER_BLOCK][2];
    
    const uint_fast64_t THREADS_PER_BLOCK = ROWS_PER_BLOCK * THREADS_PER_ROW;

    const uint_fast64_t thread_id   = THREADS_PER_BLOCK * blockIdx.x + threadIdx.x;    // global thread index
    const uint_fast64_t thread_lane = threadIdx.x & (THREADS_PER_ROW - 1);          // thread index within the vector
    const uint_fast64_t vector_id   = thread_id   /  THREADS_PER_ROW;               // global vector index
    const uint_fast64_t vector_lane = threadIdx.x /  THREADS_PER_ROW;               // vector index within the block
    const uint_fast64_t num_vectors = ROWS_PER_BLOCK * gridDim.x;                   // total number of active vectors

    for(uint_fast64_t row = vector_id; row < num_rows; row += num_vectors)
    {
        // use two threads to fetch Ap[row] and Ap[row+1]
        // this is considerably faster than the straightforward version
        if(thread_lane < 2)
            ptrs[vector_lane][thread_lane] = nondeterministicChoiceIndices[row + thread_lane];

        const uint_fast64_t row_start = ptrs[vector_lane][0];                   //same as: row_start = Ap[row];
        const uint_fast64_t row_end   = ptrs[vector_lane][1];                   //same as: row_end   = Ap[row+1];

        // initialize local Min/Max
        float localMinMaxElement = minMaxInitializer;

        if (THREADS_PER_ROW == 32 && row_end - row_start > 32)
        {
            // ensure aligned memory access to Aj and Ax

            uint_fast64_t jj = row_start - (row_start & (THREADS_PER_ROW - 1)) + thread_lane;

            // accumulate local sums
            if(jj >= row_start && jj < row_end) {
				if(Minimize) {
					localMinMaxElement = min(localMinMaxElement, y[jj]);
					//localMinMaxElement = (localMinMaxElement > y[jj]) ? y[jj] : localMinMaxElement;
				} else {
					localMinMaxElement = max(localMinMaxElement, y[jj]);
					//localMinMaxElement = (localMinMaxElement < y[jj]) ? y[jj] : localMinMaxElement;
				}
			}

            // accumulate local sums
            for(jj += THREADS_PER_ROW; jj < row_end; jj += THREADS_PER_ROW)
                if(Minimize) {
					localMinMaxElement = min(localMinMaxElement, y[jj]);
					//localMinMaxElement = (localMinMaxElement > y[jj]) ? y[jj] : localMinMaxElement;
				} else {
					localMinMaxElement = max(localMinMaxElement, y[jj]);
					//localMinMaxElement = (localMinMaxElement < y[jj]) ? y[jj] : localMinMaxElement;
				}
        }
        else
        {
            // accumulate local sums
            for(uint_fast64_t jj = row_start + thread_lane; jj < row_end; jj += THREADS_PER_ROW)
                if(Minimize) {
					localMinMaxElement = min(localMinMaxElement, y[jj]);
					//localMinMaxElement = (localMinMaxElement > y[jj]) ? y[jj] : localMinMaxElement;
				} else {
					localMinMaxElement = max(localMinMaxElement, y[jj]);
					//localMinMaxElement = (localMinMaxElement < y[jj]) ? y[jj] : localMinMaxElement;
				}
        }

        // store local sum in shared memory
        sdata[threadIdx.x] = localMinMaxElement;
        
        // reduce local min/max to row min/max
		if (Minimize) {
			/*if (THREADS_PER_ROW > 16) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 16]) ? sdata[threadIdx.x + 16] : localMinMaxElement);
			if (THREADS_PER_ROW >  8) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 8]) ? sdata[threadIdx.x + 8] : localMinMaxElement);
			if (THREADS_PER_ROW >  4) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 4]) ? sdata[threadIdx.x + 4] : localMinMaxElement);
			if (THREADS_PER_ROW >  2) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 2]) ? sdata[threadIdx.x + 2] : localMinMaxElement);
			if (THREADS_PER_ROW >  1) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement > sdata[threadIdx.x + 1]) ? sdata[threadIdx.x + 1] : localMinMaxElement);*/

			if (THREADS_PER_ROW > 16) sdata[threadIdx.x] = localMinMaxElement = min(localMinMaxElement, sdata[threadIdx.x + 16]);
			if (THREADS_PER_ROW >  8) sdata[threadIdx.x] = localMinMaxElement = min(localMinMaxElement, sdata[threadIdx.x +  8]);
			if (THREADS_PER_ROW >  4) sdata[threadIdx.x] = localMinMaxElement = min(localMinMaxElement, sdata[threadIdx.x +  4]);
			if (THREADS_PER_ROW >  2) sdata[threadIdx.x] = localMinMaxElement = min(localMinMaxElement, sdata[threadIdx.x +  2]);
			if (THREADS_PER_ROW >  1) sdata[threadIdx.x] = localMinMaxElement = min(localMinMaxElement, sdata[threadIdx.x +  1]);
		} else {
			/*if (THREADS_PER_ROW > 16) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 16]) ? sdata[threadIdx.x + 16] : localMinMaxElement);
			if (THREADS_PER_ROW >  8) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 8]) ? sdata[threadIdx.x + 8] : localMinMaxElement);
			if (THREADS_PER_ROW >  4) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 4]) ? sdata[threadIdx.x + 4] : localMinMaxElement);
			if (THREADS_PER_ROW >  2) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 2]) ? sdata[threadIdx.x + 2] : localMinMaxElement);
			if (THREADS_PER_ROW >  1) sdata[threadIdx.x] = localMinMaxElement = ((localMinMaxElement < sdata[threadIdx.x + 1]) ? sdata[threadIdx.x + 1] : localMinMaxElement);*/
			if (THREADS_PER_ROW > 16) sdata[threadIdx.x] = localMinMaxElement = max(localMinMaxElement, sdata[threadIdx.x + 16]);
			if (THREADS_PER_ROW > 8) sdata[threadIdx.x] = localMinMaxElement = max(localMinMaxElement, sdata[threadIdx.x + 8]);
			if (THREADS_PER_ROW > 4) sdata[threadIdx.x] = localMinMaxElement = max(localMinMaxElement, sdata[threadIdx.x + 4]);
			if (THREADS_PER_ROW > 2) sdata[threadIdx.x] = localMinMaxElement = max(localMinMaxElement, sdata[threadIdx.x + 2]);
			if (THREADS_PER_ROW > 1) sdata[threadIdx.x] = localMinMaxElement = max(localMinMaxElement, sdata[threadIdx.x + 1]);
		}
       
        // first thread writes the result
        if (thread_lane == 0)
            x[row] = sdata[threadIdx.x];
    }
}

template <bool Minimize, unsigned int THREADS_PER_VECTOR>
void __storm_cuda_opt_vector_reduce_float(const uint_fast64_t num_rows, const uint_fast64_t * nondeterministicChoiceIndices, float * x, const float * y)
{
	float __minMaxInitializer = -std::numeric_limits<float>::max();
	if (Minimize) {
		__minMaxInitializer = std::numeric_limits<float>::max();
	}
	const float minMaxInitializer = __minMaxInitializer;

    const size_t THREADS_PER_BLOCK  = 128;
    const size_t VECTORS_PER_BLOCK  = THREADS_PER_BLOCK / THREADS_PER_VECTOR;

    const size_t MAX_BLOCKS = cusp::detail::device::arch::max_active_blocks(storm_cuda_opt_vector_reduce_kernel_float<VECTORS_PER_BLOCK, THREADS_PER_VECTOR, Minimize>, THREADS_PER_BLOCK, (size_t) 0);
    const size_t NUM_BLOCKS = std::min<size_t>(MAX_BLOCKS, DIVIDE_INTO(num_rows, VECTORS_PER_BLOCK));

    storm_cuda_opt_vector_reduce_kernel_float<VECTORS_PER_BLOCK, THREADS_PER_VECTOR, Minimize> <<<NUM_BLOCKS, THREADS_PER_BLOCK>>> 
        (num_rows, nondeterministicChoiceIndices, x, y, minMaxInitializer);
}

template <bool Minimize>
void storm_cuda_opt_vector_reduce_float(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * nondeterministicChoiceIndices, float * x, const float * y)
{
    const uint_fast64_t rows_per_group = num_entries / num_rows;

    if (rows_per_group <=  2) { __storm_cuda_opt_vector_reduce_float<Minimize, 2>(num_rows, nondeterministicChoiceIndices, x, y); return; }
    if (rows_per_group <=  4) { __storm_cuda_opt_vector_reduce_float<Minimize, 4>(num_rows, nondeterministicChoiceIndices, x, y); return; }
    if (rows_per_group <=  8) { __storm_cuda_opt_vector_reduce_float<Minimize, 8>(num_rows, nondeterministicChoiceIndices, x, y); return; }
    if (rows_per_group <= 16) { __storm_cuda_opt_vector_reduce_float<Minimize,16>(num_rows, nondeterministicChoiceIndices, x, y); return; }
    
    __storm_cuda_opt_vector_reduce_float<Minimize,32>(num_rows, nondeterministicChoiceIndices, x, y);
}

template <bool UseCache, unsigned int THREADS_PER_VECTOR>
void __storm_cuda_opt_spmv_csr_vector_float(const uint_fast64_t num_rows, const uint_fast64_t * matrixRowIndices, const float * matrixColumnIndicesAndValues, const float* x, float* y)
{
    const size_t THREADS_PER_BLOCK  = 128;
    const size_t VECTORS_PER_BLOCK  = THREADS_PER_BLOCK / THREADS_PER_VECTOR;

    const size_t MAX_BLOCKS = cusp::detail::device::arch::max_active_blocks(storm_cuda_opt_spmv_csr_vector_kernel_float<VECTORS_PER_BLOCK, THREADS_PER_VECTOR, UseCache>, THREADS_PER_BLOCK, (size_t) 0);
    const size_t NUM_BLOCKS = std::min<size_t>(MAX_BLOCKS, DIVIDE_INTO(num_rows, VECTORS_PER_BLOCK));
    
    if (UseCache)
        bind_x(x);

    storm_cuda_opt_spmv_csr_vector_kernel_float<VECTORS_PER_BLOCK, THREADS_PER_VECTOR, UseCache> <<<NUM_BLOCKS, THREADS_PER_BLOCK>>> 
        (num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y);

    if (UseCache)
        unbind_x(x);
}

void storm_cuda_opt_spmv_csr_vector_float(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * matrixRowIndices, const float * matrixColumnIndicesAndValues, const float* x, float* y)
{
    const uint_fast64_t nnz_per_row = num_entries / num_rows;

    if (nnz_per_row <=  2) { __storm_cuda_opt_spmv_csr_vector_float<false, 2>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <=  4) { __storm_cuda_opt_spmv_csr_vector_float<false, 4>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <=  8) { __storm_cuda_opt_spmv_csr_vector_float<false, 8>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <= 16) { __storm_cuda_opt_spmv_csr_vector_float<false,16>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    
    __storm_cuda_opt_spmv_csr_vector_float<false,32>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y);
}

void storm_cuda_opt_spmv_csr_vector_tex(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * matrixRowIndices, const float * matrixColumnIndicesAndValues, const float* x, float* y)
{
    const uint_fast64_t nnz_per_row = num_entries / num_rows;

    if (nnz_per_row <=  2) { __storm_cuda_opt_spmv_csr_vector_float<true, 2>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <=  4) { __storm_cuda_opt_spmv_csr_vector_float<true, 4>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <=  8) { __storm_cuda_opt_spmv_csr_vector_float<true, 8>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }
    if (nnz_per_row <= 16) { __storm_cuda_opt_spmv_csr_vector_float<true,16>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y); return; }

    __storm_cuda_opt_spmv_csr_vector_float<true,32>(num_rows, matrixRowIndices, matrixColumnIndicesAndValues, x, y);
}

// NON-OPT

template <bool UseCache, unsigned int THREADS_PER_VECTOR>
void __storm_cuda_spmv_csr_vector_float(const uint_fast64_t num_rows, const uint_fast64_t * matrixRowIndices, const uint_fast64_t * matrixColumnIndices, const float * matrixValues, const float* x, float* y)
{
    const size_t THREADS_PER_BLOCK  = 128;
    const size_t VECTORS_PER_BLOCK  = THREADS_PER_BLOCK / THREADS_PER_VECTOR;

    const size_t MAX_BLOCKS = cusp::detail::device::arch::max_active_blocks(spmv_csr_vector_kernel<uint_fast64_t, float, VECTORS_PER_BLOCK, THREADS_PER_VECTOR, UseCache>, THREADS_PER_BLOCK, (size_t) 0);
    const size_t NUM_BLOCKS = std::min<size_t>(MAX_BLOCKS, DIVIDE_INTO(num_rows, VECTORS_PER_BLOCK));
    
    if (UseCache)
        bind_x(x);

    spmv_csr_vector_kernel<uint_fast64_t, float, VECTORS_PER_BLOCK, THREADS_PER_VECTOR, UseCache> <<<NUM_BLOCKS, THREADS_PER_BLOCK>>> 
        (num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y);

    if (UseCache)
        unbind_x(x);
}

void storm_cuda_spmv_csr_vector_float(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * matrixRowIndices, const uint_fast64_t * matrixColumnIndices, const float * matrixValues, const float* x, float* y)
{
    const uint_fast64_t nnz_per_row = num_entries / num_rows;

    if (nnz_per_row <=  2) { __storm_cuda_spmv_csr_vector_float<false, 2>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <=  4) { __storm_cuda_spmv_csr_vector_float<false, 4>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <=  8) { __storm_cuda_spmv_csr_vector_float<false, 8>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <= 16) { __storm_cuda_spmv_csr_vector_float<false,16>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    
    __storm_cuda_spmv_csr_vector_float<false,32>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y);
}

void storm_cuda_spmv_csr_vector_tex_float(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * matrixRowIndices, const uint_fast64_t * matrixColumnIndices, const float * matrixValues, const float* x, float* y)
{
    const uint_fast64_t nnz_per_row = num_entries / num_rows;

    if (nnz_per_row <=  2) { __storm_cuda_spmv_csr_vector_float<true, 2>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <=  4) { __storm_cuda_spmv_csr_vector_float<true, 4>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <=  8) { __storm_cuda_spmv_csr_vector_float<true, 8>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }
    if (nnz_per_row <= 16) { __storm_cuda_spmv_csr_vector_float<true,16>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y); return; }

    __storm_cuda_spmv_csr_vector_float<true,32>(num_rows, matrixRowIndices, matrixColumnIndices, matrixValues, x, y);
}

} // end namespace device
} // end namespace detail
} // end namespace cusp