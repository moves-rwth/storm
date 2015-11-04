#ifndef STORM_SOLVER_NATIVEMINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_NATIVEMINMAXLINEAREQUATIONSOLVER_H_

#include <cstdint>
#include "src/solver/MinMaxLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        /*!
         * A class that uses the gmm++ library to implement the MinMaxLinearEquationSolver interface.
         */
        template<class ValueType>
        class NativeMinMaxLinearEquationSolver : public MinMaxLinearEquationSolver<ValueType> {
        public:
            /*!
             * Constructs a min/max linear equation solver with parameters being set according to the settings
             * object.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             */
            NativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, MinMaxTechniqueSelection preferredTechnique = MinMaxTechniqueSelection::FROMSETTINGS, bool trackPolicy = false);
            
            /*!
             * Constructs a min/max linear equation solver with the given parameters.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             * @param precision The precision to use for convergence detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             */
            NativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, double precision, uint_fast64_t maximalNumberOfIterations, MinMaxTechniqueSelection tech, bool relative = true, bool trackPolicy = false);
            
            virtual void performMatrixVectorMultiplication(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType>* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* newX = nullptr) const override;
            
            virtual void solveEquationSystem(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr, std::vector<storm::storage::sparse::state_type>* initialPolicy = nullptr) const override;

        };
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_NATIVEMINMAXLINEAREQUATIONSOLVER_H_ */
