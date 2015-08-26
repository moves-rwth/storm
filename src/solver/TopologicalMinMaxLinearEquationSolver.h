#ifndef STORM_SOLVER_TOPOLOGICALVALUEITERATIONMINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_TOPOLOGICALVALUEITERATIONMINMAXLINEAREQUATIONSOLVER_H_

#include "src/solver/NativeMinMaxLinearEquationSolver.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/NotSupportedException.h"

#include <utility>
#include <vector>

#include "storm-config.h"
#ifdef STORM_HAVE_CUDA
#include "cudaForStorm.h"
#endif

namespace storm {
    namespace solver {
        
        /*!
         * A class that uses SCC Decompositions to solve a min/max linear equation system.
         */
        template<class ValueType>
		class TopologicalMinMaxLinearEquationSolver : public NativeMinMaxLinearEquationSolver<ValueType> {
        public:
            /*!
             * Constructs a min-max linear equation solver with parameters being set according to the settings
             * object.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             */
            TopologicalMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
            
            /*!
             * Constructs a min/max linear equation solver with the given parameters.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             * @param precision The precision to use for convergence detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             */
            TopologicalMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, double precision, uint_fast64_t maximalNumberOfIterations, bool relative = true);
            
            virtual void solveEquationSystem(bool minimize, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const override;
		private:

			bool enableCuda;
			/*!
			 * Given a topological sort of a SCC Decomposition, this will calculate the optimal grouping of SCCs with respect to the size of the GPU memory.
			 */
			std::vector<std::pair<bool, storm::storage::StateBlock>> getOptimalGroupingFromTopologicalSccDecomposition(storm::storage::StronglyConnectedComponentDecomposition<ValueType> const& sccDecomposition, std::vector<uint_fast64_t> const& topologicalSort, storm::storage::SparseMatrix<ValueType> const& matrix) const;
        };

		template <typename IndexType, typename ValueType>
		bool __basicValueIteration_mvReduce_minimize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<IndexType, ValueType>> const& columnIndicesAndValues, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount) {
			//
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Unsupported template arguments.");
		}
		template <>
		inline bool __basicValueIteration_mvReduce_minimize<uint_fast64_t, double>(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount) {
#ifdef STORM_HAVE_CUDA
			return basicValueIteration_mvReduce_uint64_double_minimize(maxIterationCount, precision, relativePrecisionCheck, matrixRowIndices, columnIndicesAndValues, x, b, nondeterministicChoiceIndices, iterationCount);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without CUDA support.");
#endif
		}
		template <>
		inline bool __basicValueIteration_mvReduce_minimize<uint_fast64_t, float>(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, float>> const& columnIndicesAndValues, std::vector<float>& x, std::vector<float> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount) {
#ifdef STORM_HAVE_CUDA
			return basicValueIteration_mvReduce_uint64_float_minimize(maxIterationCount, precision, relativePrecisionCheck, matrixRowIndices, columnIndicesAndValues, x, b, nondeterministicChoiceIndices, iterationCount);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without CUDA support.");
#endif
		}

		template <typename IndexType, typename ValueType>
		bool __basicValueIteration_mvReduce_maximize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<IndexType, ValueType>> const& columnIndicesAndValues, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount) {
			//
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Unsupported template arguments.");
		}
		template <>
		inline bool __basicValueIteration_mvReduce_maximize<uint_fast64_t, double>(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount) {
#ifdef STORM_HAVE_CUDA
			return basicValueIteration_mvReduce_uint64_double_maximize(maxIterationCount, precision, relativePrecisionCheck, matrixRowIndices, columnIndicesAndValues, x, b, nondeterministicChoiceIndices, iterationCount);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without CUDA support.");
#endif
		}
		template <>
		inline bool __basicValueIteration_mvReduce_maximize<uint_fast64_t, float>(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, float>> const& columnIndicesAndValues, std::vector<float>& x, std::vector<float> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount) {
#ifdef STORM_HAVE_CUDA
			return basicValueIteration_mvReduce_uint64_float_maximize(maxIterationCount, precision, relativePrecisionCheck, matrixRowIndices, columnIndicesAndValues, x, b, nondeterministicChoiceIndices, iterationCount);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without CUDA support.");
#endif
		}
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_TOPOLOGICALVALUEITERATIONMINMAXLINEAREQUATIONSOLVER_H_ */
