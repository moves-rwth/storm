#ifndef STORM_SOLVER_TOPOLOGICALCUDAMINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_TOPOLOGICALCUDAMINMAXLINEAREQUATIONSOLVER_H_

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

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
class TopologicalCudaMinMaxLinearEquationSolver : public MinMaxLinearEquationSolver<ValueType> {
   public:
    TopologicalCudaMinMaxLinearEquationSolver();

    /*!
     * Constructs a min-max linear equation solver with parameters being set according to the settings
     * object.
     *
     * @param A The matrix defining the coefficients of the linear equation system.
     */
    TopologicalCudaMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);

    virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) override;
    virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& matrix) override;

    virtual bool internalSolveEquations(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x,
                                        std::vector<ValueType> const& b) const override;

    void setSchedulerFixedForRowGroup(storm::storage::BitVector&& states) override;

   private:
    storm::storage::SparseMatrix<ValueType> const* A;
    std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;

    bool enableCuda;
    /*!
     * Given a topological sort of a SCC Decomposition, this will calculate the optimal grouping of SCCs with respect to the size of the GPU memory.
     */
    std::vector<std::pair<bool, storm::storage::StateBlock>> getOptimalGroupingFromTopologicalSccDecomposition(
        storm::storage::StronglyConnectedComponentDecomposition<ValueType> const& sccDecomposition, std::vector<uint_fast64_t> const& topologicalSort,
        storm::storage::SparseMatrix<ValueType> const& matrix) const;
};

template<typename IndexType, typename ValueType>
bool __basicValueIteration_mvReduce_minimize(uint_fast64_t const, double const, bool const, std::vector<uint_fast64_t> const&,
                                             std::vector<storm::storage::MatrixEntry<IndexType, ValueType>> const&, std::vector<ValueType>& x,
                                             std::vector<ValueType> const&, std::vector<uint_fast64_t> const&, size_t&) {
    //
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Unsupported template arguments.");
}
template<>
inline bool __basicValueIteration_mvReduce_minimize<uint_fast64_t, double>(
    uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices,
    std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b,
    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount) {
    (void)maxIterationCount;
    (void)precision;
    (void)relativePrecisionCheck;
    (void)matrixRowIndices;
    (void)columnIndicesAndValues;
    (void)x;
    (void)b;
    (void)nondeterministicChoiceIndices;
    (void)iterationCount;

#ifdef STORM_HAVE_CUDA
    return basicValueIteration_mvReduce_uint64_double_minimize(maxIterationCount, precision, relativePrecisionCheck, matrixRowIndices, columnIndicesAndValues,
                                                               x, b, nondeterministicChoiceIndices, iterationCount);
#else
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Storm is compiled without CUDA support.");
#endif
}

template<typename IndexType, typename ValueType>
bool __basicValueIteration_mvReduce_maximize(uint_fast64_t const, double const, bool const, std::vector<uint_fast64_t> const&,
                                             std::vector<storm::storage::MatrixEntry<IndexType, ValueType>> const&, std::vector<ValueType>&,
                                             std::vector<ValueType> const&, std::vector<uint_fast64_t> const&, size_t&) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Unsupported template arguments.");
}
template<>
inline bool __basicValueIteration_mvReduce_maximize<uint_fast64_t, double>(
    uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices,
    std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b,
    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount) {
    (void)maxIterationCount;
    (void)precision;
    (void)relativePrecisionCheck;
    (void)matrixRowIndices;
    (void)columnIndicesAndValues;
    (void)x;
    (void)b;
    (void)nondeterministicChoiceIndices;
    (void)iterationCount;

#ifdef STORM_HAVE_CUDA
    return basicValueIteration_mvReduce_uint64_double_maximize(maxIterationCount, precision, relativePrecisionCheck, matrixRowIndices, columnIndicesAndValues,
                                                               x, b, nondeterministicChoiceIndices, iterationCount);
#else
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Storm is compiled without CUDA support.");
#endif
}

template<typename ValueType>
class TopologicalCudaMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
   public:
    TopologicalCudaMinMaxLinearEquationSolverFactory(bool trackScheduler = false);

   protected:
    virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(Environment const& env) const override;
};

}  // namespace solver
}  // namespace storm

#endif /* STORM_SOLVER_TOPOLOGICALCUDAMINMAXLINEAREQUATIONSOLVER_H_ */
