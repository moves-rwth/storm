#ifndef STORM_SOLVER_GMMXXMINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_GMMXXMINMAXLINEAREQUATIONSOLVER_H_

#include <memory>

#include "src/utility/gmm.h"

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
            GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, MinMaxTechniqueSelection preferredTechnique = MinMaxTechniqueSelection::FROMSETTINGS, bool trackScheduler = false);
            
            /*!
             * Constructs a min/max linear equation solver with the given parameters.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             * @param precision The precision to use for convergence detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             */
            GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, double precision, uint_fast64_t maximalNumberOfIterations, MinMaxTechniqueSelection tech, bool relative, bool trackScheduler = false);

            virtual void performMatrixVectorMultiplication(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType>* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const override;
            
            virtual void solveEquationSystem(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const override;
            
        private:
            // The (gmm++) matrix associated with this equation solver.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmxxMatrix;

            // A reference to the row group indices of the original matrix.
            std::vector<uint_fast64_t> const& rowGroupIndices;
            
        };
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_GMMXXMINMAXLINEAREQUATIONSOLVER_H_ */