#pragma once

#include "cuspExtensionFloat.h"
#include "cuspExtensionDouble.h"

namespace cusp {
namespace detail {
namespace device {

template <typename ValueType>
void storm_cuda_opt_spmv_csr_vector(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * matrixRowIndices, const ValueType * matrixColumnIndicesAndValues, const ValueType* x, ValueType* y) {
	//
	throw;
}
template <>
void storm_cuda_opt_spmv_csr_vector<double>(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * matrixRowIndices, const double * matrixColumnIndicesAndValues, const double* x, double* y) {
	storm_cuda_opt_spmv_csr_vector_double(num_rows, num_entries, matrixRowIndices, matrixColumnIndicesAndValues, x, y);
}
template <>
void storm_cuda_opt_spmv_csr_vector<float>(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * matrixRowIndices, const float * matrixColumnIndicesAndValues, const float* x, float* y) {
	storm_cuda_opt_spmv_csr_vector_float(num_rows, num_entries, matrixRowIndices, matrixColumnIndicesAndValues, x, y);
}

template <bool Minimize, typename ValueType>
void storm_cuda_opt_vector_reduce(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * nondeterministicChoiceIndices, ValueType * x, const ValueType * y) {
	//
	throw;
}
template <>
void storm_cuda_opt_vector_reduce<true, double>(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * nondeterministicChoiceIndices, double * x, const double * y) {
	storm_cuda_opt_vector_reduce_double<true>(num_rows, num_entries, nondeterministicChoiceIndices, x, y);
}
template <>
void storm_cuda_opt_vector_reduce<false, double>(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * nondeterministicChoiceIndices, double * x, const double * y) {
	storm_cuda_opt_vector_reduce_double<false>(num_rows, num_entries, nondeterministicChoiceIndices, x, y);
}

template <>
void storm_cuda_opt_vector_reduce<true, float>(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * nondeterministicChoiceIndices, float * x, const float * y) {
	storm_cuda_opt_vector_reduce_float<true>(num_rows, num_entries, nondeterministicChoiceIndices, x, y);
}
template <>
void storm_cuda_opt_vector_reduce<false, float>(const uint_fast64_t num_rows, const uint_fast64_t num_entries, const uint_fast64_t * nondeterministicChoiceIndices, float * x, const float * y) {
	storm_cuda_opt_vector_reduce_float<false>(num_rows, num_entries, nondeterministicChoiceIndices, x, y);
}

} // end namespace device
} // end namespace detail
} // end namespace cusp