#ifndef STORM_ASSUMPTIONMAKER_H
#define STORM_ASSUMPTIONMAKER_H

#include "AssumptionChecker.h"
#include "Order.h"

#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        class AssumptionMaker {
            typedef std::shared_ptr<expressions::BinaryRelationExpression> AssumptionType;
        public:
            // TODO: @Svenja, could you update the documentation of the public methods?
            /*!
             * Constructs AssumptionMaker based on the order extender, the assumption checker and number of states of the mode
             *
             * @param orderExtender The OrderExtender which needs the assumptions made by the AssumptionMaker.
             * @param checker The AssumptionChecker which checks the assumptions at sample points.
             * @param numberOfStates The number of states of the model.
             */
            AssumptionMaker(storage::SparseMatrix<ValueType> matrix);

            /*!
             * Creates assumptions, and checks them, only VALID and UNKNOWN assumptions are returned.
             * If one assumption is VALID, this assumption will be returned as only assumption.
             * Possible results: AssumptionStatus::VALID, AssumptionStatus::UNKNOWN.
             *
             * @param val1 First state number
             * @param val2 Second state number
             * @param order The order on which the assumptions are checked
             * @return Map with at most three assumptions, and the validation
             */
            std::map<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> createAndCheckAssumptions(uint_fast64_t val1, uint_fast64_t val2, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region) const;

            void initializeCheckingOnSamples(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Dtmc<ValueType>> model, storage::ParameterRegion<ValueType> region, uint_fast64_t numberOfSamples);

            void setSampleValues(std::vector<std::vector<ConstantType>>const & samples);

        private:
            std::pair<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> createAndCheckAssumption(expressions::Variable var1, expressions::Variable var2, expressions::BinaryRelationExpression::RelationType relationType, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region) const;

            AssumptionChecker<ValueType, ConstantType> assumptionChecker;

            std::shared_ptr<expressions::ExpressionManager> expressionManager;

            uint_fast64_t numberOfStates;

        };
    }
}
#endif //STORM_ASSUMPTIONMAKER_H

