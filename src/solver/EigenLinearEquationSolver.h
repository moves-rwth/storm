#pragma once

#include "src/solver/LinearEquationSolver.h"

#include "src/utility/eigen.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class EigenLinearEquationSolverSettings {
        public:
            enum class SolutionMethod {
                SparseLU, BiCGSTAB, DGMRES, GMRES
            };
            
            enum class Preconditioner {
                Ilu, Diagonal, None
            };
            
            EigenLinearEquationSolverSettings();
            
            void setSolutionMethod(SolutionMethod const& method);
            void setPreconditioner(Preconditioner const& preconditioner);
            void setPrecision(ValueType precision);
            void setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations);
            void setNumberOfIterationsUntilRestart(uint64_t restart);

            SolutionMethod getSolutionMethod() const;
            Preconditioner getPreconditioner() const;
            ValueType getPrecision() const;
            uint64_t getMaximalNumberOfIterations() const;
            uint64_t getNumberOfIterationsUntilRestart() const;
            
        private:
            SolutionMethod method;
            Preconditioner preconditioner;
            double precision;
            uint64_t maximalNumberOfIterations;
            uint_fast64_t restart;
        };
        
        template<>
        class EigenLinearEquationSolverSettings<storm::RationalNumber> {
        public:
            EigenLinearEquationSolverSettings();
        };

        template<>
        class EigenLinearEquationSolverSettings<storm::RationalFunction> {
        public:
            EigenLinearEquationSolverSettings();
        };
        
        /*!
         * A class that uses the Eigen library to implement the LinearEquationSolver interface.
         */
        template<typename ValueType>
        class EigenLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, EigenLinearEquationSolverSettings<ValueType> const& settings = EigenLinearEquationSolverSettings<ValueType>());
            EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, EigenLinearEquationSolverSettings<ValueType> const& settings = EigenLinearEquationSolverSettings<ValueType>());
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;
            
            virtual void solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;
            virtual void multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const override;

            EigenLinearEquationSolverSettings<ValueType>& getSettings();
            EigenLinearEquationSolverSettings<ValueType> const& getSettings() const;
                        
        private:
            virtual uint64_t getMatrixRowCount() const override;
            virtual uint64_t getMatrixColumnCount() const override;
            
            // The (eigen) matrix associated with this equation solver.
            std::unique_ptr<Eigen::SparseMatrix<ValueType>> eigenA;

            // The settings used by the solver.
            EigenLinearEquationSolverSettings<ValueType> settings;
        };
        
        template<typename ValueType>
        class EigenLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
        public:
            virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const override;
            
            EigenLinearEquationSolverSettings<ValueType>& getSettings();
            EigenLinearEquationSolverSettings<ValueType> const& getSettings() const;

            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;

        private:
            EigenLinearEquationSolverSettings<ValueType> settings;
        };
    }
}