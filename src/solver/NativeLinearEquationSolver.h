#ifndef STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_

#include "LinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        /*!
         * A class that uses StoRM's native matrix operations to implement the LinearEquationSolver interface.
         */
        template<typename ValueType>
        class NativeLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            // An enumeration specifying the available solution methods.
            enum class SolutionMethod {
                Jacobi
            };
            
            /*!
             * Constructs a linear equation solver with parameters being set according to the settings object.
             */
            NativeLinearEquationSolver();

            /*!
             * Constructs a linear equation solver with the given parameters.
             *
             * @param method The method to use for linear equation solving.
             * @param precision The precision to use for convergence detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             */
            NativeLinearEquationSolver(SolutionMethod method, double precision, uint_fast64_t maximalNumberOfIterations, bool relative = true);
            
            virtual LinearEquationSolver<ValueType>* clone() const override;
            
            virtual void solveEquationSystem(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr) const override;
            
            virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const override;

        private:
            /*!
             * Retrieves the string representation of the solution method associated with this solver.
             *
             * @return The string representation of the solution method associated with this solver.
             */
            std::string methodToString() const;
            
            // The method to use for solving linear equation systems.
            SolutionMethod method;
            
            // The required precision for the iterative methods.
            double precision;
            
            // Sets whether the relative or absolute error is to be considered for convergence detection.
            bool relative;
            
            // The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;
        };
    }
}

#endif /* STORM_SOLVER_NATIVELINEAREQUATIONSOLVER_H_ */
