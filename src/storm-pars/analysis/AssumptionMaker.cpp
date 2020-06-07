#include "AssumptionMaker.h"

namespace storm {
    namespace analysis {
        typedef std::shared_ptr<expressions::BinaryRelationExpression> AssumptionType;
        template<typename ValueType, typename ConstantType>
        AssumptionMaker<ValueType, ConstantType>::AssumptionMaker(AssumptionChecker<ValueType, ConstantType>* assumptionChecker, uint_fast64_t numberOfStates) {
            this->numberOfStates = numberOfStates;
            this->assumptionChecker = assumptionChecker;
            this->expressionManager = std::make_shared<expressions::ExpressionManager>(expressions::ExpressionManager());
            for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
                expressionManager->declareRationalVariable(std::to_string(i));
            }
        }

        template <typename ValueType, typename ConstantType>
        std::map<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> AssumptionMaker<ValueType, ConstantType>::createAndCheckAssumptions(uint_fast64_t val1, uint_fast64_t val2, std::shared_ptr<Order> order) {
            std::map<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> result;
            expressions::Variable var1 = expressionManager->getVariable(std::to_string(val1));
            expressions::Variable var2 = expressionManager->getVariable(std::to_string(val2));
            result.insert(createAndCheckAssumption(var1, var2, expressions::BinaryRelationExpression::RelationType::Greater, order));
            result.insert(createAndCheckAssumption(var2, var1, expressions::BinaryRelationExpression::RelationType::Greater, order));
            result.insert(createAndCheckAssumption(var1, var2, expressions::BinaryRelationExpression::RelationType::Equal, order));

            return result;
        }

        template <typename ValueType, typename ConstantType>
        std::pair<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> AssumptionMaker<ValueType, ConstantType>::createAndCheckAssumption(expressions::Variable var1, expressions::Variable var2, expressions::BinaryRelationExpression::RelationType relationType, std::shared_ptr<Order> order) {
            auto assumption = std::make_shared<expressions::BinaryRelationExpression>(expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(), var2.getExpression().getBaseExpressionPointer(), var1.getExpression().getBaseExpressionPointer(), relationType));
            AssumptionStatus validationResult;

            validationResult = assumptionChecker->validateAssumption(assumption, order);

            return std::pair<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus>(assumption, validationResult);
        }

        template class AssumptionMaker<RationalFunction, double>;
        template class AssumptionMaker<RationalFunction, RationalNumber>;
    }
}
