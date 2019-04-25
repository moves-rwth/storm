
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
        storm::expressions::Variable LpSolver<ValueType>::addContinuousVariable(std::string const& name, boost::optional<ValueType> const& lowerBound, boost::optional<ValueType> const& upperBound, ValueType objectiveFunctionCoefficient) {
            if (lowerBound) {
                if (upperBound) {
                    return addBoundedContinuousVariable(name, lowerBound.get(), upperBound.get(), objectiveFunctionCoefficient);
                } else {
                    return addLowerBoundedContinuousVariable(name, lowerBound.get(), objectiveFunctionCoefficient);
                }
            } else {
                if (upperBound) {
                    return addUpperBoundedContinuousVariable(name, upperBound.get(), objectiveFunctionCoefficient);
                } else {
                    return addUnboundedContinuousVariable(name, objectiveFunctionCoefficient);
                }
            }
        }
        
        template<typename ValueType>
        storm::expressions::Variable LpSolver<ValueType>::addIntegerVariable(std::string const& name, boost::optional<ValueType> const& lowerBound, boost::optional<ValueType> const& upperBound, ValueType objectiveFunctionCoefficient) {
            if (lowerBound) {
                if (upperBound) {
                    return addBoundedIntegerVariable(name, lowerBound.get(), upperBound.get(), objectiveFunctionCoefficient);
                } else {
                    return addLowerBoundedIntegerVariable(name, lowerBound.get(), objectiveFunctionCoefficient);
                }
            } else {
                if (upperBound) {
                    return addUpperBoundedIntegerVariable(name, upperBound.get(), objectiveFunctionCoefficient);
                } else {
                    return addUnboundedIntegerVariable(name, objectiveFunctionCoefficient);
                }
            }
        }
        
        template<typename ValueType>
        storm::expressions::Expression LpSolver<ValueType>::getConstant(ValueType value) const {
            return manager->rational(value);
        }
        
        template class LpSolver<double>;
        template class LpSolver<storm::RationalNumber>;
        
    }
}
