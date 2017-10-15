#pragma once

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class StandardMinMaxLinearEquationSolver : public MinMaxLinearEquationSolver<ValueType> {
        public:
            StandardMinMaxLinearEquationSolver(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) override;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& matrix) override;
            
            virtual ~StandardMinMaxLinearEquationSolver() = default;
            
            virtual void repeatedMultiply(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const override;

            virtual void clearCache() const override;

        protected:
            
            // possibly cached data
            mutable std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linEqSolverA;
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowVector; // A.rowCount() entries
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector; // A.rowCount() entries
            
            /// The factory used to obtain linear equation solvers.
            std::unique_ptr<LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
            
            // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
            // when the solver is destructed.
            std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;
            
            // A reference to the original sparse matrix given to this solver. If the solver takes posession of the matrix
            // the reference refers to localA.
            storm::storage::SparseMatrix<ValueType> const* A;
        };
     
        template<typename ValueType>
        class StandardMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
        public:
            StandardMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
            StandardMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
            StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
            
            // Make the other create methods visible.
            using MinMaxLinearEquationSolverFactory<ValueType>::create;
            
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create() const override;

        protected:
            std::unique_ptr<LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
            
        private:
            void createLinearEquationSolverFactory() const;
        };
        
        template<typename ValueType>
        class GmmxxMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            GmmxxMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
        };

        template<typename ValueType>
        class EigenMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            EigenMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
        };

        template<typename ValueType>
        class NativeMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            NativeMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
        };

        template<typename ValueType>
        class EliminationMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            EliminationMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
        };

    }
}
