//
// Created by Jip Spel on 24.09.18.
//

#include "ValueTypeToExpression.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace expressions {
        template <typename ValueType>
        ValueTypeToExpression<ValueType>::ValueTypeToExpression(std::shared_ptr<ExpressionManager> manager) : manager(manager) {
            // Intentionally left empty.
        }

        template <typename ValueType>
        std::shared_ptr<ExpressionManager> ValueTypeToExpression<ValueType>::getManager()  {
            return manager;
        }

        template <typename ValueType>
        Expression ValueTypeToExpression<ValueType>::toExpression(ValueType function) {
            auto varsFunction = function.gatherVariables();
            for (auto var : varsFunction) {
                auto varsManager = manager->getVariables();
                bool found = find_if(varsManager.begin(), varsManager.end(),
                                     [&](auto val) -> bool {
                                        return val.getName() == var.name();
                                     }) != varsManager.end();
                if (!found) {
                    manager->declareIntegerVariable(var.name());
                }
            }

            auto denominator = function.denominator();
            if (!denominator.isConstant()) {
                STORM_LOG_DEBUG("Expecting the denominator to be constant");
            }

            storm::expressions::Expression denominatorVal = manager->integer(std::stoi(storm::utility::to_string(denominator.constantPart())));
            storm::expressions::Expression result;
            if (function.isConstant()) {
                result = manager->integer(std::stoi(storm::utility::to_string(function.constantPart())));
            } else {
                auto nominator = function.nominatorAsPolynomial().polynomialWithCoefficient();
                result = manager->integer(std::stoi(storm::utility::to_string(nominator.constantPart())));
                for (auto itr = nominator.begin(); itr != nominator.end(); ++itr) {
                    varsFunction.clear();
                    (*itr).gatherVariables(varsFunction);
                    // TODO: improve transformation from coefficient to expression
                    storm::expressions::Expression nominatorPartExpr = manager->integer(
                            std::stoi(storm::utility::to_string((*itr).coeff())));
                    for (auto var : varsFunction) {
                        auto degree = nominator.degree(var);
                        nominatorPartExpr = nominatorPartExpr * (manager->getVariable(var.name())^manager->integer(degree));
                    }
                    if (varsFunction.size() >= 1) {
                        result = result + nominatorPartExpr;
                    }
                }
                result = result / denominatorVal;
            }
            return result;
        }
        template class ValueTypeToExpression<storm::RationalFunction>;
    }
}
