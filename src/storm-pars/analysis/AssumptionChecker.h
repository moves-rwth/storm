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
            AssumptionChecker(std::shared_ptr<storm::logic::Formula const> formula, std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples);

            bool checkOnSamples(uint_fast64_t val1, uint_fast64_t val2);
            bool checkOnSamples(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption);
        private:
            std::shared_ptr<storm::logic::Formula const> formula;

//            std::vector<storm::models::sparse::Dtmc<double>> sampleModels;

            std::vector<std::vector<double>> results;

//            uint_fast64_t numberOfStates;
//
//            storm::storage::BitVector initialStates;
        };
    }
}


#endif //STORM_ASSUMPTIONCHECKER_H
