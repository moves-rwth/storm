//
// Created by Jip Spel on 03.09.18.
//

#include "AssumptionMaker.h"

namespace storm {
    namespace analysis {
        template<typename ValueType>
        AssumptionMaker<ValueType>::AssumptionMaker(storm::analysis::AssumptionChecker<ValueType>* assumptionChecker, uint_fast64_t numberOfStates, bool validate) {
            this->numberOfStates = numberOfStates;
            this->assumptionChecker = assumptionChecker;
            this->validate = validate;
            this->expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());
            for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
                expressionManager->declareRationalVariable(std::to_string(i));
            }
        }

        template <typename ValueType>
        std::map<std::shared_ptr<storm::expressions::BinaryRelationExpression>, bool> AssumptionMaker<ValueType>::createAndCheckAssumption(uint_fast64_t val1, uint_fast64_t val2, storm::analysis::Lattice* lattice) {
            std::map<std::shared_ptr<storm::expressions::BinaryRelationExpression>, bool> result;

            storm::expressions::Variable var1 = expressionManager->getVariable(std::to_string(val1));
            storm::expressions::Variable var2 = expressionManager->getVariable(std::to_string(val2));

            std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption1
                    = std::make_shared<storm::expressions::BinaryRelationExpression>(storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                                                                                                  var1.getExpression().getBaseExpressionPointer(), var2.getExpression().getBaseExpressionPointer(),
                                                                                                                                  storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual));
            bool result1 = (validate && assumptionChecker->validateAssumption(assumption1, lattice) && assumptionChecker->valid(assumption1));
            result[assumption1] = result1;

            std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption2
                    = std::make_shared<storm::expressions::BinaryRelationExpression>(storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                                                                                                  var2.getExpression().getBaseExpressionPointer(), var1.getExpression().getBaseExpressionPointer(),
                                                                                                                                  storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual));
            bool result2 = (validate && assumptionChecker->validateAssumption(assumption2, lattice) && assumptionChecker->valid(assumption2));
            result[assumption2] = result2;

            return result;
        }

        template <typename ValueType>
        std::shared_ptr<storm::expressions::BinaryRelationExpression> AssumptionMaker<ValueType>::createEqualAssumption(uint_fast64_t val1, uint_fast64_t val2) {
            storm::expressions::Variable var1 = expressionManager->getVariable(std::to_string(val1));
            storm::expressions::Variable var2 = expressionManager->getVariable(std::to_string(val2));
            return std::make_shared<storm::expressions::BinaryRelationExpression>(storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                                                                                               var1.getExpression().getBaseExpressionPointer(), var2.getExpression().getBaseExpressionPointer(),
                                                                                                                               storm::expressions::BinaryRelationExpression::RelationType::Equal));
        }


        template class AssumptionMaker<storm::RationalFunction>;
    }
}
