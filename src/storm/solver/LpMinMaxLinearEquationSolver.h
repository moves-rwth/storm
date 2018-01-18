#pragma once

#include "storm/solver/LpSolver.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"
#include "storm/utility/solver.h"

namespace storm {
    
    class Environment;
    
    namespace solver {
        
        template<typename ValueType>
        class LpMinMaxLinearEquationSolver : public StandardMinMaxLinearEquationSolver<ValueType> {
        public:
            LpMinMaxLinearEquationSolver(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory);
            LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory);
            LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory);
            
            virtual bool internalSolveEquations(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

            virtual void clearCache() const override;

            virtual MinMaxLinearEquationSolverRequirements getRequirements(Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction = boost::none, bool const& hasInitialScheduler = false) const override;
            
        private:
            std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> lpSolverFactory;
        };
        
        template<typename ValueType>
        class LpMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            LpMinMaxLinearEquationSolverFactory();
            LpMinMaxLinearEquationSolverFactory(std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory);
            LpMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory);
            
            // Make the other create methods visible.
            using MinMaxLinearEquationSolverFactory<ValueType>::create;

            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(Environment const& env) const override;

        private:
            std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> lpSolverFactory;
        };
    }
}
