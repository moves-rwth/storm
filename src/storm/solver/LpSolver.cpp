
#include "LpSolver.h"

#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"



namespace storm {
    namespace solver {

        template<typename ValueType>
        LpSolver<ValueType>::LpSolver() : manager(new storm::expressions::ExpressionManager()), currentModelHasBeenOptimized(false), optimizationDirection(OptimizationDirection::Minimize) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        LpSolver<ValueType>::LpSolver(OptimizationDirection const& optimizationDir) : manager(new storm::expressions::ExpressionManager()), currentModelHasBeenOptimized(false), optimizationDirection(optimizationDir) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        storm::expressions::Expression LpSolver<ValueType>::getConstant(ValueType value) const {
            return manager->rational(value);
        }
        
        template class LpSolver<double>;
        template class LpSolver<storm::RationalNumber>;
        
    }
}
