#pragma once

#include "src/solver/LinearEquationSolver.h"
#include "src/solver/MinMaxLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        class StandardMinMaxLinearEquationSolverSettings {
        public:
            StandardMinMaxLinearEquationSolverSettings();
            
            enum class SolutionMethod {
                ValueIteration, PolicyIteration
            };
            
            void setSolutionMethod(SolutionMethod const& solutionMethod);
            
            SolutionMethod const& getSolutionMethod() const;
            
        private:
            SolutionMethod solutionMethod;
        };
        
        template<typename ValueType>
        class StandardMinMaxLinearEquationSolver : public MinMaxLinearEquationSolver<ValueType> {
        public:
            StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings const& settings = StandardMinMaxLinearEquationSolverSettings());
            StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings const& settings = StandardMinMaxLinearEquationSolverSettings());
            
            virtual void solveEquationSystem(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const override;
            virtual void performMatrixVectorMultiplication(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType>* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const override;

            StandardMinMaxLinearEquationSolverSettings const& getSettings() const;
            StandardMinMaxLinearEquationSolverSettings& getSettings();
            
        private:
            /// The settings of this solver.
            StandardMinMaxLinearEquationSolverSettings settings;
            
            /// The factory used to obtain linear equation solvers.
            std::unique_ptr<LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
            
            // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
            // when the solver is destructed.
            std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;
            
            // A reference to the original sparse matrix given to this solver. If the solver takes posession of the matrix
            // the reference refers to localA.
            storm::storage::SparseMatrix<ValueType> const& A;
        };
     
        template<typename ValueType>
        class StandardMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
        public:
            StandardMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
            StandardMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, bool trackScheduler = false);
            StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, bool trackScheduler = false);
            
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const override;
            
            StandardMinMaxLinearEquationSolverSettings& getSettings();
            StandardMinMaxLinearEquationSolverSettings const& getSettings() const;
            
        private:
            StandardMinMaxLinearEquationSolverSettings settings;
            
            std::unique_ptr<LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
        template<typename ValueType>
        class GmmxxMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            GmmxxMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
        };

        template<typename ValueType>
        class EigenMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            EigenMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
        };

        template<typename ValueType>
        class NativeMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            NativeMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
        };

        template<typename ValueType>
        class EliminationMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            EliminationMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
        };

    }
}