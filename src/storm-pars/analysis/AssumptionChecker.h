//
// Created by Jip Spel on 12.09.18.
//

#ifndef STORM_ASSUMPTIONCHECKER_H
#define STORM_ASSUMPTIONCHECKER_H

#include "storm/logic/Formula.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/environment/Environment.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"

namespace storm {
    namespace analysis {
        template<typename ValueType>
        class AssumptionChecker {
        public:
            /*!
             * Constructs an AssumptionChecker based on the number of samples, for the given formula and model.
             *
             * @param formula The formula to check.
             * @param model The model to check the formula on.
             * @param numberOfSamples Number of sample points.
             */
            AssumptionChecker(std::shared_ptr<storm::logic::Formula const> formula, std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples);

            /*!
             * Checks if the assumption holds at the sample points of the AssumptionChecker.
             *
             * @param assumption The assumption to check.
             * @return true if the assumption holds at the sample points
             */
            bool checkOnSamples(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption);

        private:
            std::shared_ptr<storm::logic::Formula const> formula;

            std::vector<std::vector<double>> results;

        };
    }
}

#endif //STORM_ASSUMPTIONCHECKER_H
