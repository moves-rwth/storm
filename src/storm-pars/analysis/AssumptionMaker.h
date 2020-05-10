#ifndef STORM_ASSUMPTIONMAKER_H
#define STORM_ASSUMPTIONMAKER_H

#include "AssumptionChecker.h"
#include "Order.h"

#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        class AssumptionMaker {
            typedef std::shared_ptr<expressions::BinaryRelationExpression> AssumptionType;
        public:
            /*!
             * Constructs AssumptionMaker based on the order extender, the assumption checker and number of states of the mode
             *
             * @param orderExtender The OrderExtender which needs the assumptions made by the AssumptionMaker.
             * @param checker The AssumptionChecker which checks the assumptions at sample points.
             * @param numberOfStates The number of states of the model.
             */
            AssumptionMaker(AssumptionChecker<ValueType, ConstantType>* checker, uint_fast64_t numberOfStates);

            /*!
             * Creates assumptions, and checks them if validate in constructor is true.
             * Possible results: AssumptionStatus::VALID, AssumptionStatus::INVALID, AssumptionStatus::UNKNOWN
             * If validate is false, the result is always AssumptionStatus::UNKNOWN
             *
             * @param val1 First state number
             * @param val2 Second state number
             * @param order The order on which the assumptions are checked
             * @return Map with three assumptions, and the validation
             */
            std::map<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> createAndCheckAssumptions(uint_fast64_t val1, uint_fast64_t val2, Order* order);

        private:
            std::pair<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> createAndCheckAssumption(expressions::Variable var1, expressions::Variable var2, expressions::BinaryRelationExpression::RelationType relationType, Order* order);

            AssumptionChecker<ValueType, ConstantType>* assumptionChecker;

            std::shared_ptr<expressions::ExpressionManager> expressionManager;

            uint_fast64_t numberOfStates;

        };
    }
}
#endif //STORM_ASSUMPTIONMAKER_H

