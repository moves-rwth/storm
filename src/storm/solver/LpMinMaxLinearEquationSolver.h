#pragma once

#include "storm/solver/LpSolver.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"
#include "storm/utility/solver.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class LpMinMaxLinearEquationSolver : public StandardMinMaxLinearEquationSolver<ValueType> {
        public:
            LpMinMaxLinearEquationSolver(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory);
            LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory);
            LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory);
            
            virtual bool internalSolveEquations(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

            virtual void clearCache() const override;

            virtual MinMaxLinearEquationSolverRequirements getRequirements(boost::optional<storm::solver::OptimizationDirection> const& direction = boost::none) const override;
            
        private:
            std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> lpSolverFactory;
        };
        
        template<typename ValueType>
        class LpMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            LpMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
            LpMinMaxLinearEquationSolverFactory(std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory, bool trackScheduler = false);
            LpMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory, bool trackScheduler = false);
            
            virtual void setMinMaxMethod(MinMaxMethodSelection const& newMethod) override;
            virtual void setMinMaxMethod(MinMaxMethod const& newMethod) override;

            // Make the other create methods visible.
            using MinMaxLinearEquationSolverFactory<ValueType>::create;

            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create() const override;

        private:
            std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> lpSolverFactory;
        };
    }
}
