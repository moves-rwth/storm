#ifndef STORM_SOLVER_GMMXXMINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_GMMXXMINMAXLINEAREQUATIONSOLVER_H_

#include "gmm/gmm_matrix.h"

#include "src/solver/MinMaxLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        /*!
         * A class that uses the gmm++ library to implement the MinMaxLinearEquationSolver interface.
         */
        template<class ValueType>
        class GmmxxMinMaxLinearEquationSolver : public MinMaxLinearEquationSolver<ValueType> {
        public:
            /*!
             * Constructs a min/max linear equation solver with parameters being set according to the settings
             * object.
             *
             * @param A The matrix defining the coefficients of the linear equation system.             
             */
            GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
            
            /*!
             * Constructs a min/max linear equation solver with the given parameters.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             * @param precision The precision to use for convergence detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             */
            GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, double precision, uint_fast64_t maximalNumberOfIterations, bool useValueIteration, bool relative = true);

            virtual void performMatrixVectorMultiplication(bool minimize, std::vector<ValueType>& x, std::vector<ValueType>* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const override;
            
            virtual void solveEquationSystem(bool minimize, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const override;
            
        private:
            // The (gmm++) matrix associated with this equation solver.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmxxMatrix;

			// A reference to the original sparse matrix.
			storm::storage::SparseMatrix<ValueType> const& stormMatrix;
            
            // A reference to the row group indices of the original matrix.
            std::vector<uint_fast64_t> const& rowGroupIndices;
            
            // The required precision for the iterative methods.
            double precision;
            
            // Sets whether the relative or absolute error is to be considered for convergence detection.
            bool relative;
            
            // The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;

			// Whether value iteration or policy iteration is to be used.
			bool useValueIteration;
        };
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_GMMXXMINMAXLINEAREQUATIONSOLVER_H_ */