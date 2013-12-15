#ifndef STORM_SOLVER_NATIVENONDETERMINISTICLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_NATIVENONDETERMINISTICLINEAREQUATIONSOLVER_H_

#include "src/solver/AbstractNondeterministicLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        /*!
         * A class that uses the gmm++ library to implement the AbstractNondeterminsticLinearEquationSolver interface.
         */
        template<class ValueType>
        class NativeNondeterministicLinearEquationSolver : public AbstractNondeterministicLinearEquationSolver<ValueType> {
        public:
            /*!
             * Constructs a nondeterministic linear equation solver with parameters being set according to the settings
             * object.
             */
            NativeNondeterministicLinearEquationSolver();
            
            /*!
             * Constructs a nondeterminstic linear equation solver with the given parameters.
             *
             * @param precision The precision to use for convergence detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             */
            NativeNondeterministicLinearEquationSolver(double precision, uint_fast64_t maximalNumberOfIterations, bool relative = true);
            
            virtual AbstractNondeterministicLinearEquationSolver<ValueType>* clone() const override;
            
            virtual void performMatrixVectorMultiplication(bool minimize, storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<ValueType>* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* newX = nullptr) const override;
            
            virtual void solveEquationSystem(bool minimize, storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const override;

        private:
            // The required precision for the iterative methods.
            double precision;
            
            // Sets whether the relative or absolute error is to be considered for convergence detection.
            bool relative;
            
            // The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;
        };
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_NATIVENONDETERMINISTICLINEAREQUATIONSOLVER_H_ */
