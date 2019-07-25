#include "AssumptionMaker.h"

namespace storm {
    namespace analysis {
        typedef std::shared_ptr<expressions::BinaryRelationExpression> AssumptionType;
        template<typename ValueType>
        AssumptionMaker<ValueType>::AssumptionMaker(AssumptionChecker<ValueType>* assumptionChecker, uint_fast64_t numberOfStates, bool validate) {
            this->numberOfStates = numberOfStates;
            this->assumptionChecker = assumptionChecker;
            this->validate = validate;
            this->expressionManager = std::make_shared<expressions::ExpressionManager>(expressions::ExpressionManager());
            for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
                expressionManager->declareRationalVariable(std::to_string(i));
            }
        }


        template <typename ValueType>
        std::map<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> AssumptionMaker<ValueType>::createAndCheckAssumption(uint_fast64_t val1, uint_fast64_t val2, Order* order) {
            std::map<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> result;

            expressions::Variable var1 = expressionManager->getVariable(std::to_string(val1));
            expressions::Variable var2 = expressionManager->getVariable(std::to_string(val2));
            std::shared_ptr<expressions::BinaryRelationExpression> assumption1
                    = std::make_shared<expressions::BinaryRelationExpression>(expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                                                                                                  var1.getExpression().getBaseExpressionPointer(), var2.getExpression().getBaseExpressionPointer(),
                                                                                                                                  expressions::BinaryRelationExpression::RelationType::Greater));
            AssumptionStatus result1;
            AssumptionStatus result2;
            AssumptionStatus result3;
            if (validate) {
                result1 = assumptionChecker->validateAssumption(assumption1, order);
            } else {
                result1 = AssumptionStatus::UNKNOWN;
            }
            result[assumption1] = result1;


            std::shared_ptr<expressions::BinaryRelationExpression> assumption2
                    = std::make_shared<expressions::BinaryRelationExpression>(expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                                                                                                  var2.getExpression().getBaseExpressionPointer(), var1.getExpression().getBaseExpressionPointer(),
                                                                                                                                  expressions::BinaryRelationExpression::RelationType::Greater));

            if (validate) {
                result2 = assumptionChecker->validateAssumption(assumption2, order);
            } else {
                result2 = AssumptionStatus::UNKNOWN;
            }
            result[assumption2] = result2;

            std::shared_ptr<expressions::BinaryRelationExpression> assumption3
                    = std::make_shared<expressions::BinaryRelationExpression>(expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                                                                                                  var2.getExpression().getBaseExpressionPointer(), var1.getExpression().getBaseExpressionPointer(),
                                                                                                                                  expressions::BinaryRelationExpression::RelationType::Equal));
            if (validate) {
                result3 = assumptionChecker->validateAssumption(assumption3, order);
            } else {
                result3 = AssumptionStatus::UNKNOWN;
            }
            result[assumption3] = result3;

            return result;
        }

        template class AssumptionMaker<RationalFunction>;
    }
}
