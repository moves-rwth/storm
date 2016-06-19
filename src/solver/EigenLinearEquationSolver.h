#pragma once

#include "src/solver/LinearEquationSolver.h"

#include "src/utility/eigen.h"

namespace storm {
    namespace solver {
        
        /*!
         * A class that uses the Eigen library to implement the LinearEquationSolver interface.
         */
        template<typename ValueType>
        class EigenLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            enum class SolutionMethod {
                SparseLU, Bicgstab
            };
            
            enum class Preconditioner {
                Ilu, Diagonal, None
            };
            
            EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, SolutionMethod method, double precision, uint64_t maximalNumberOfIterations, Preconditioner preconditioner);

            EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
            
            virtual void solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr) const override;
            
            virtual void performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const override;
            
        private:
            // A pointer to the original sparse matrix given to this solver.
            storm::storage::SparseMatrix<ValueType> const* originalA;
            
            // The (eigen) matrix associated with this equation solver.
            std::unique_ptr<Eigen::SparseMatrix<ValueType>> eigenA;

            // The method to use for solving linear equation systems.
            SolutionMethod method;
            
            // The required precision for the iterative methods.
            double precision;
            
            // The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;
            
            // The preconditioner to use when solving the linear equation system.
            Preconditioner preconditioner;
        };
        
        template<>
        class EigenLinearEquationSolver<storm::RationalNumber> : public LinearEquationSolver<storm::RationalNumber> {
        public:
            enum class SolutionMethod {
                SparseLU
            };
            
            EigenLinearEquationSolver(storm::storage::SparseMatrix<storm::RationalNumber> const& A, SolutionMethod method = SolutionMethod::SparseLU);
            
            virtual void solveEquationSystem(std::vector<storm::RationalNumber>& x, std::vector<storm::RationalNumber> const& b, std::vector<storm::RationalNumber>* multiplyResult = nullptr) const override;
            
            virtual void performMatrixVectorMultiplication(std::vector<storm::RationalNumber>& x, std::vector<storm::RationalNumber> const* b, uint_fast64_t n = 1, std::vector<storm::RationalNumber>* multiplyResult = nullptr) const override;
            
        private:
            // A pointer to the original sparse matrix given to this solver.
            storm::storage::SparseMatrix<storm::RationalNumber> const* originalA;
            
            // The (eigen) matrix associated with this equation solver.
            std::unique_ptr<Eigen::SparseMatrix<storm::RationalNumber>> eigenA;
            
            // The method to use for solving linear equation systems.
            SolutionMethod method;
        };
        
        template<>
        class EigenLinearEquationSolver<storm::RationalFunction> : public LinearEquationSolver<storm::RationalFunction> {
        public:
            enum class SolutionMethod {
                SparseLU
            };
            
            EigenLinearEquationSolver(storm::storage::SparseMatrix<storm::RationalFunction> const& A, SolutionMethod method = SolutionMethod::SparseLU);
            
            virtual void solveEquationSystem(std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const& b, std::vector<storm::RationalFunction>* multiplyResult = nullptr) const override;
            
            virtual void performMatrixVectorMultiplication(std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const* b, uint_fast64_t n = 1, std::vector<storm::RationalFunction>* multiplyResult = nullptr) const override;
            
        private:
            // A pointer to the original sparse matrix given to this solver.
            storm::storage::SparseMatrix<storm::RationalFunction> const* originalA;
            
            // The (eigen) matrix associated with this equation solver.
            std::unique_ptr<Eigen::SparseMatrix<storm::RationalFunction>> eigenA;
            
            // The method to use for solving linear equation systems.
            SolutionMethod method;
        };
    }
}