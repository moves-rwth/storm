//
// Created by Jip Spel on 03.09.18.
//

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
            typedef std::shared_ptr<storm::expressions::BinaryRelationExpression> AssumptionType;
        public:
            /*!
             * Constructs AssumptionMaker based on the lattice extender, the assumption checker and number of states of the model.
             * TODO
             * @param latticeExtender The LatticeExtender which needs the assumptions made by the AssumptionMaker.
             * @param checker The AssumptionChecker which checks the assumptions at sample points.
             * @param numberOfStates The number of states of the model.
             */
            AssumptionMaker( storm::analysis::AssumptionChecker<ValueType>* checker, uint_fast64_t numberOfStates, bool validate);

            std::map<std::shared_ptr<storm::expressions::BinaryRelationExpression>, AssumptionStatus> createAndCheckAssumption(uint_fast64_t val1, uint_fast64_t val2, storm::analysis::Lattice* lattice);

        private:
            storm::analysis::AssumptionChecker<ValueType>* assumptionChecker;

            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;

            uint_fast64_t numberOfStates;

            bool validate;
        };
    }
}
#endif //STORM_ASSUMPTIONMAKER_H

