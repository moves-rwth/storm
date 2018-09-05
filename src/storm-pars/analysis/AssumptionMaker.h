//
// Created by Jip Spel on 03.09.18.
//

#ifndef STORM_ASSUMPTIONMAKER_H
#define STORM_ASSUMPTIONMAKER_H

#include "Lattice.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "LatticeExtender.h"

namespace storm {
    namespace analysis {

        template<typename ValueType>
        class AssumptionMaker {
        public:
            AssumptionMaker(storm::analysis::LatticeExtender<ValueType>* latticeExtender, uint_fast64_t numberOfStates);

            std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> startMakingAssumptions(storm::analysis::Lattice* lattice, uint_fast64_t critical1, uint_fast64_t critical2);


        private:
            std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> runRecursive(storm::analysis::Lattice* lattice, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions);

            storm::analysis::LatticeExtender<ValueType>* latticeExtender;

            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;

            uint_fast64_t numberOfStates;
        };
    }
}
#endif //STORM_ASSUMPTIONMAKER_H
