#include "SolveGoal.h"

#include "src/storage/SparseMatrix.h"
#include "src/utility/solver.h"
#include "src/solver/MinMaxLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        template<typename VT>
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<VT>> configureMinMaxLinearEquationSolver(BoundedGoal<VT> const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<VT> const& factory, storm::storage::SparseMatrix<VT> const&  matrix) {
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<VT>> p = factory.create(matrix);
            p->setOptimizationDirection(goal.direction());
            p->setEarlyTerminationCriterion(std::make_unique<TerminateAfterFilteredExtremumPassesThresholdValue<double>>(goal.relevantColumns(), goal.threshold, goal.minimize()));
            return p;
        }
        
        template<typename VT> 
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<VT>> configureMinMaxLinearEquationSolver(SolveGoal const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<VT> const& factory, storm::storage::SparseMatrix<VT> const&  matrix) {
            if(goal.isBounded()) {
                return configureMinMaxLinearEquationSolver(static_cast<BoundedGoal<VT> const&>(goal), factory, matrix);
            }  
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<VT>> p = factory.create(matrix);
            p->setOptimizationDirection(goal.direction());
            return p;
        }   
    
        template std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<double>> configureMinMaxLinearEquationSolver(BoundedGoal<double> const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<double> const& factory, storm::storage::SparseMatrix<double> const&  matrix);
        template std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<double>> configureMinMaxLinearEquationSolver(SolveGoal const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<double> const& factory, storm::storage::SparseMatrix<double> const&  matrix);
        
    }
}
