#include "storm/generator/TransientVariableInformation.h"

#include "storm/storage/jani/Model.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/jani/ParallelComposition.h"
#include "storm/storage/jani/eliminator/ArrayEliminator.h"

#include "storm/exceptions/OutOfRangeException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

#include <cmath>

namespace storm {
namespace generator {

template<>
TransientVariableData<storm::RationalFunction>::TransientVariableData(storm::expressions::Variable const& variable,
                                                                      boost::optional<storm::RationalFunction> const& lowerBound,
                                                                      boost::optional<storm::RationalFunction> const& upperBound,
                                                                      storm::RationalFunction const& defaultValue, bool global)
    : variable(variable), lowerBound(lowerBound), upperBound(upperBound), defaultValue(defaultValue), global(global) {
    // There is no '<=' for rational functions. Therefore, do not check the bounds for this ValueType
}

template<typename VariableType>
TransientVariableData<VariableType>::TransientVariableData(storm::expressions::Variable const& variable, boost::optional<VariableType> const& lowerBound,
                                                           boost::optional<VariableType> const& upperBound, VariableType const& defaultValue, bool global)
    : variable(variable), lowerBound(lowerBound), upperBound(upperBound), defaultValue(defaultValue), global(global) {
    STORM_LOG_THROW(!lowerBound.is_initialized() || lowerBound.get() <= defaultValue, storm::exceptions::OutOfRangeException,
                    "The default value for transient variable " << variable.getName() << " is smaller than its lower bound.");
    STORM_LOG_THROW(!upperBound.is_initialized() || defaultValue <= upperBound.get(), storm::exceptions::OutOfRangeException,
                    "The default value for transient variable " << variable.getName() << " is higher than its upper bound.");
}

template<typename VariableType>
TransientVariableData<VariableType>::TransientVariableData(storm::expressions::Variable const& variable, VariableType const& defaultValue, bool global)
    : variable(variable), defaultValue(defaultValue) {
    // Intentionally left empty.
}

template<typename ValueType>
TransientVariableInformation<ValueType>::TransientVariableInformation(
    storm::jani::Model const& model, std::vector<std::reference_wrapper<storm::jani::Automaton const>> const& parallelAutomata) {
    createVariablesForVariableSet(model.getGlobalVariables(), true);

    for (auto const& automatonRef : parallelAutomata) {
        createVariablesForAutomaton(automatonRef.get());
    }

    sortVariables();
}

template<typename ValueType>
void TransientVariableInformation<ValueType>::registerArrayVariableReplacements(storm::jani::ArrayEliminatorData const& arrayEliminatorData) {
    arrayVariableToElementInformations.clear();
    // Find for each replaced array variable the corresponding references in this variable information
    for (auto const& arrayVariable : arrayEliminatorData.eliminatedArrayVariables) {
        if (arrayVariable->isTransient()) {
            auto findRes = arrayEliminatorData.replacements.find(arrayVariable->getExpressionVariable());
            STORM_LOG_ASSERT(findRes != arrayEliminatorData.replacements.end(), "No replacement for array variable.");
            auto const& replacements = findRes->second;
            auto const& innerType = arrayVariable->getType().asArrayType().getBaseTypeRecursive();
            if (innerType.isBasicType() && innerType.asBasicType().isBooleanType()) {
                auto replInfo = convertArrayReplacement(replacements, booleanVariableInformation);
                this->arrayVariableToElementInformations.emplace(arrayVariable->getExpressionVariable(), std::move(replInfo));
            } else if ((innerType.isBasicType() && innerType.asBasicType().isIntegerType()) ||
                       (innerType.isBoundedType() && innerType.asBoundedType().isIntegerType())) {
                auto replInfo = convertArrayReplacement(replacements, integerVariableInformation);
                this->arrayVariableToElementInformations.emplace(arrayVariable->getExpressionVariable(), std::move(replInfo));
            } else if ((innerType.isBasicType() && innerType.asBasicType().isRealType()) ||
                       (innerType.isBoundedType() && innerType.asBoundedType().isRealType())) {
                auto replInfo = convertArrayReplacement(replacements, rationalVariableInformation);
                this->arrayVariableToElementInformations.emplace(arrayVariable->getExpressionVariable(), std::move(replInfo));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unhandled type of base variable.");
            }
        }
    }
}

template<typename ValueType>
TransientVariableData<bool> const& TransientVariableInformation<ValueType>::getBooleanArrayVariableReplacement(
    storm::expressions::Variable const& arrayVariable, std::vector<uint64_t> const& arrayIndexVector) const {
    return booleanVariableInformation[arrayVariableToElementInformations.at(arrayVariable).getVariableInformationIndex(arrayIndexVector)];
}

template<typename ValueType>
TransientVariableData<int64_t> const& TransientVariableInformation<ValueType>::getIntegerArrayVariableReplacement(
    storm::expressions::Variable const& arrayVariable, std::vector<uint64_t> const& arrayIndexVector) const {
    return integerVariableInformation[arrayVariableToElementInformations.at(arrayVariable).getVariableInformationIndex(arrayIndexVector)];
}

template<typename ValueType>
TransientVariableData<ValueType> const& TransientVariableInformation<ValueType>::getRationalArrayVariableReplacement(
    storm::expressions::Variable const& arrayVariable, std::vector<uint64_t> const& arrayIndexVector) const {
    return rationalVariableInformation[arrayVariableToElementInformations.at(arrayVariable).getVariableInformationIndex(arrayIndexVector)];
}

template<typename ValueType>
void TransientVariableInformation<ValueType>::createVariablesForAutomaton(storm::jani::Automaton const& automaton) {
    createVariablesForVariableSet(automaton.getVariables(), false);
}

template<typename ValueType>
void TransientVariableInformation<ValueType>::createVariablesForVariableSet(storm::jani::VariableSet const& variableSet, bool global) {
    for (auto const& variable : variableSet.getBooleanVariables()) {
        if (variable.isTransient()) {
            booleanVariableInformation.emplace_back(variable.getExpressionVariable(), variable.getInitExpression().evaluateAsBool(), global);
        }
    }
    for (auto const& variable : variableSet.getBoundedIntegerVariables()) {
        if (variable.isTransient()) {
            boost::optional<int64_t> lowerBound;
            boost::optional<int64_t> upperBound;
            auto const& type = variable.getType().asBoundedType();
            if (type.hasLowerBound()) {
                lowerBound = type.getLowerBound().evaluateAsInt();
            }
            if (type.hasUpperBound()) {
                upperBound = type.getUpperBound().evaluateAsInt();
            }
            integerVariableInformation.emplace_back(variable.getExpressionVariable(), lowerBound, upperBound, variable.getInitExpression().evaluateAsInt(),
                                                    global);
        }
    }
    for (auto const& variable : variableSet.getUnboundedIntegerVariables()) {
        if (variable.isTransient()) {
            integerVariableInformation.emplace_back(variable.getExpressionVariable(), variable.getInitExpression().evaluateAsInt(), global);
        }
    }
    for (auto const& variable : variableSet.getRealVariables()) {
        if (variable.isTransient()) {
            rationalVariableInformation.emplace_back(variable.getExpressionVariable(),
                                                     storm::utility::convertNumber<ValueType>(variable.getInitExpression().evaluateAsRational()), global);
        }
    }
}

template<typename ValueType>
void TransientVariableInformation<ValueType>::sortVariables() {
    // Sort the variables so we can make some assumptions when iterating over them (in the next-state generators).
    std::sort(booleanVariableInformation.begin(), booleanVariableInformation.end(),
              [](TransientVariableData<bool> const& a, TransientVariableData<bool> const& b) { return a.variable < b.variable; });
    std::sort(integerVariableInformation.begin(), integerVariableInformation.end(),
              [](TransientVariableData<int64_t> const& a, TransientVariableData<int64_t> const& b) { return a.variable < b.variable; });
    std::sort(rationalVariableInformation.begin(), rationalVariableInformation.end(),
              [](TransientVariableData<ValueType> const& a, TransientVariableData<ValueType> const& b) { return a.variable < b.variable; });
}

template<typename ValueType>
void TransientVariableInformation<ValueType>::setDefaultValuesInEvaluator(storm::expressions::ExpressionEvaluator<ValueType>& evaluator) const {
    for (auto const& variableData : booleanVariableInformation) {
        evaluator.setBooleanValue(variableData.variable, variableData.defaultValue);
    }
    for (auto const& variableData : integerVariableInformation) {
        evaluator.setIntegerValue(variableData.variable, variableData.defaultValue);
    }
    for (auto const& variableData : rationalVariableInformation) {
        evaluator.setRationalValue(variableData.variable, variableData.defaultValue);
    }
}

template struct TransientVariableInformation<double>;
template struct TransientVariableInformation<storm::RationalNumber>;
template struct TransientVariableInformation<storm::RationalFunction>;

}  // namespace generator
}  // namespace storm
