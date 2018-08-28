//
// Created by Jip Spel on 28.08.18.
//

#ifndef STORM_LATTICEEXTENDER_H
#define STORM_LATTICEEXTENDER_H

#include <storm/logic/Formula.h>
#include "storm/models/sparse/Dtmc.h"
#include "storm-pars/analysis/Lattice.h"



namespace storm {
    namespace analysis {

        template <typename ValueType>
        class LatticeExtender {

        public:
            LatticeExtender();

//            /*!
//             * Creates a Lattice based on the transition matrix, topStates of the Lattice and bottomStates of the Lattice
//             * @tparam ValueType Type of the probabilities
//             * @param model The pointer to the model
//             * @param formulas Vector with pointer to formula
//             * @return pointer to the created Lattice.
//             */
//            storm::analysis::Lattice* toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas);
//
//            storm::analysis::Lattice* extendLattice(storm::analysis::Lattice* lattice, std::set<storm::expressions::Expression> assumptions);

        private:
            std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model;

            std::map<uint_fast64_t, storm::storage::BitVector> stateMap;
        };
    }
}

#endif //STORM_LATTICEEXTENDER_H
