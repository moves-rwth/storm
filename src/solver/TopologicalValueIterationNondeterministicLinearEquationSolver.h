#ifndef STORM_SOLVER_TOPOLOGICALVALUEITERATIONNONDETERMINISTICLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_TOPOLOGICALVALUEITERATIONNONDETERMINISTICLINEAREQUATIONSOLVER_H_

#include "src/solver/NativeNondeterministicLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        /*!
         * A class that uses SCC Decompositions to solve a linear equation system
         */
        template<class ValueType>
		class TopologicalValueIterationNondeterministicLinearEquationSolver : public NativeNondeterministicLinearEquationSolver<ValueType> {
        public:
            /*!
             * Constructs a nondeterministic linear equation solver with parameters being set according to the settings
             * object.
             */
			TopologicalValueIterationNondeterministicLinearEquationSolver();
            
            /*!
             * Constructs a nondeterminstic linear equation solver with the given parameters.
             *
             * @param precision The precision to use for convergence detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             */
			TopologicalValueIterationNondeterministicLinearEquationSolver(double precision, uint_fast64_t maximalNumberOfIterations, bool relative = true);
            
            virtual NondeterministicLinearEquationSolver<ValueType>* clone() const override;
            
            virtual void solveEquationSystem(bool minimize, storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const override;
        };
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_NATIVENONDETERMINISTICLINEAREQUATIONSOLVER_H_ */
