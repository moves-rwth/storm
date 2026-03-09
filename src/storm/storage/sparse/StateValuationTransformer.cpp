#include "storm/storage/sparse/StateValuationTransformer.h"
#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/utility/constants.h"

namespace storm::storage::sparse {

void StateValuationTransform::addBooleanExpression(storm::expressions::Variable const& var, storm::expressions::Expression const& expr) {
    booleanVariables.push_back(var);
    booleanExpressions.push_back(expr);
}

StateValuations StateValuationTransform::buildNewStateValuations(bool extend) {
    StateValuationsBuilder builder;
    if (extend) {
        for (auto it = oldStateValuations.at(0).begin(); it != oldStateValuations.at(0).end(); ++it) {
            builder.addVariable(it.getVariable());
        }
    }
    for (auto const& v : booleanVariables) {
        builder.addVariable(v);
    }

    for (uint64_t state = 0; state < oldStateValuations.getNumberOfStates(); ++state) {
        storm::expressions::ExpressionEvaluator<storm::RationalNumber> evaluator(booleanVariables[0].getManager());
        std::vector<bool> booleanValues;
        std::vector<int64_t> integerValues;
        for (auto sv = oldStateValuations.at(state).begin(); sv != oldStateValuations.at(state).end(); ++sv) {
            if (sv.isVariableAssignment()) {
                auto const& var = sv.getVariable();
                if (sv.isBoolean()) {
                    evaluator.setBooleanValue(var, sv.getBooleanValue());
                    if (extend) {
                        booleanValues.push_back(sv.getBooleanValue());
                    }
                } else if (sv.isInteger()) {
                    evaluator.setIntegerValue(var, sv.getIntegerValue());
                    if (extend) {
                        integerValues.push_back(sv.getIntegerValue());
                    }
                } else {
                    STORM_LOG_ASSERT(sv.isRational(), "Must be RationalVariable");
                    evaluator.setRationalValue(var, sv.getRationalValue());
                    STORM_LOG_THROW(!extend, storm::exceptions::NotSupportedException, "Extending state valuations with rational values is currently not supported.");
                }
            }
            // Label assignments can be safely skipped.
        }
        for (auto const& expr : booleanExpressions) {
            booleanValues.push_back(evaluator.asBool(expr));
        }
        builder.addState(state, std::move(booleanValues), std::move(integerValues));
    }
    return builder.build();
}
}  // namespace storm::storage::sparse