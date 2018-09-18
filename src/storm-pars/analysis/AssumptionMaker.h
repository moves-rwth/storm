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
        public:
            /*!
             * Constructs AssumptionMaker based on the lattice extender, the assumption checker and number of states of the model.
             *
             * @param latticeExtender The LatticeExtender which needs the assumptions made by the AssumptionMaker.
             * @param checker The AssumptionChecker which checks the assumptions at sample points.
             * @param numberOfStates The number of states of the model.
             */
            AssumptionMaker(storm::analysis::LatticeExtender<ValueType>* latticeExtender, storm::analysis::AssumptionChecker<ValueType>* checker, uint_fast64_t numberOfStates);

            /*!
             * Make the assumptions given a lattice and two states which could not be added to the lattice. Returns when no more assumptions can be made.
             *
             * @param lattice The lattice on which assumptions are needed to allow further extension.
             * @param critical1 State number
             * @param critical2 State number
             * @return A mapping from pointers to different lattices and assumptions made to create the specific lattice.
             */
            std::map<storm::analysis::Lattice*, std::vector<std::pair<std::shared_ptr<storm::expressions::BinaryRelationExpression>, bool>>> makeAssumptions(
                    storm::analysis::Lattice *lattice, uint_fast64_t critical1, uint_fast64_t critical2);

        private:
            std::map<storm::analysis::Lattice*, std::vector<std::pair<std::shared_ptr<storm::expressions::BinaryRelationExpression>, bool>>> runRecursive(storm::analysis::Lattice* lattice, std::vector<std::pair<std::shared_ptr<storm::expressions::BinaryRelationExpression>, bool>> assumptions);

            std::map<storm::analysis::Lattice*, std::vector<std::pair<std::shared_ptr<storm::expressions::BinaryRelationExpression>, bool>>> createAssumptions(storm::expressions::Variable var1, storm::expressions::Variable var2, storm::analysis::Lattice* lattice,std::vector<std::pair<std::shared_ptr<storm::expressions::BinaryRelationExpression>, bool>> assumptions);

            storm::analysis::LatticeExtender<ValueType>* latticeExtender;

            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;

            uint_fast64_t numberOfStates;

            storm::analysis::AssumptionChecker<ValueType>* assumptionChecker;
        };
    }
}
#endif //STORM_ASSUMPTIONMAKER_H
