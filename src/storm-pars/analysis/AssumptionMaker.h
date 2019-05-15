#ifndef STORM_ASSUMPTIONMAKER_H
#define STORM_ASSUMPTIONMAKER_H

#include "AssumptionChecker.h"
#include "Lattice.h"
#include "LatticeExtender.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm-pars/utility/ModelInstantiator.h"


namespace storm {
    namespace analysis {

        template<typename ValueType>
        class AssumptionMaker {
            typedef std::shared_ptr<expressions::BinaryRelationExpression> AssumptionType;
        public:
            /*!
             * Constructs AssumptionMaker based on the lattice extender, the assumption checker and number of states of the mode
             *
             * @param latticeExtender The LatticeExtender which needs the assumptions made by the AssumptionMaker.
             * @param checker The AssumptionChecker which checks the assumptions at sample points.
             * @param numberOfStates The number of states of the model.
             */
            AssumptionMaker(AssumptionChecker<ValueType>* checker, uint_fast64_t numberOfStates, bool validate);

            /*!
             * Creates assumptions, and checks them if validate in constructor is true.
             * Possible results: AssumptionStatus::VALID, AssumptionStatus::INVALID, AssumptionStatus::UNKNOWN
             * If validate is false result is always AssumptionStatus::UNKNOWN
             *
             * @param val1 First state number
             * @param val2 Second state number
             * @param lattice The lattice on which the assumptions are checked
             * @return Map with three assumptions, and the validation
             */
            std::map<std::shared_ptr<expressions::BinaryRelationExpression>, AssumptionStatus> createAndCheckAssumption(uint_fast64_t val1, uint_fast64_t val2, Lattice* lattice);

        private:
            AssumptionChecker<ValueType>* assumptionChecker;

            std::shared_ptr<expressions::ExpressionManager> expressionManager;

            uint_fast64_t numberOfStates;

            bool validate;
        };
    }
}
#endif //STORM_ASSUMPTIONMAKER_H

