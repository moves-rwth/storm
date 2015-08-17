
#include "LpSolver.h"

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionManager.h"



namespace storm {
    namespace solver {
        LpSolver::LpSolver() : manager(new storm::expressions::ExpressionManager()), currentModelHasBeenOptimized(false), modelSense(ModelSense::Minimize) {
            // Intentionally left empty.
        }
        
        LpSolver::LpSolver(ModelSense const& modelSense) : manager(new storm::expressions::ExpressionManager()), currentModelHasBeenOptimized(false), modelSense(modelSense) {
            // Intentionally left empty.
        }
        
        storm::expressions::Expression LpSolver::getConstant(double value) const {
            return manager->rational(value);
        }
    }
}