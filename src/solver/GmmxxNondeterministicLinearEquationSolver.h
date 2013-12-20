#ifndef STORM_SOLVER_GMMXXNONDETERMINISTICLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_GMMXXNONDETERMINISTICLINEAREQUATIONSOLVER_H_

#include "src/solver/NondeterministicLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        /*!
         * A class that uses the gmm++ library to implement the NondeterministicLinearEquationSolver interface.
         */
        template<class Type>
        class GmmxxNondeterministicLinearEquationSolver : public NondeterministicLinearEquationSolver<Type> {
        public:
            /*!
             * Constructs a nondeterministic linear equation solver with parameters being set according to the settings
             * object.
             */
            GmmxxNondeterministicLinearEquationSolver();
            
            /*!
             * Constructs a nondeterminstic linear equation solver with the given parameters.
             *
             * @param precision The precision to use for convergence detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             */
            GmmxxNondeterministicLinearEquationSolver(double precision, uint_fast64_t maximalNumberOfIterations, bool relative = true);

            virtual NondeterministicLinearEquationSolver<Type>* clone() const;
            
            virtual void performMatrixVectorMultiplication(bool minimize, storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<Type>* b = nullptr, uint_fast64_t n = 1, std::vector<Type>* multiplyResult = nullptr) const override;
            
            virtual void solveEquationSystem(bool minimize, storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<Type>* multiplyResult = nullptr, std::vector<Type>* newX = nullptr) const override;
            
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

#endif /* STORM_SOLVER_GMMXXNONDETERMINISTICLINEAREQUATIONSOLVER_H_ */