
#include "LpSolver.h"

#include "src/storm/storage/expressions/Expression.h"
#include "src/storm/storage/expressions/ExpressionManager.h"



namespace storm {
    namespace solver {
        LpSolver::LpSolver() : manager(new storm::expressions::ExpressionManager()), currentModelHasBeenOptimized(false), optimizationDirection(OptimizationDirection::Minimize) {
            // Intentionally left empty.
        }
        
        LpSolver::LpSolver(OptimizationDirection const& optimizationDir) : manager(new storm::expressions::ExpressionManager()), currentModelHasBeenOptimized(false), optimizationDirection(optimizationDir) {
            // Intentionally left empty.
        }
        
        storm::expressions::Expression LpSolver::getConstant(double value) const {
            return manager->rational(value);
        }
    }
}
