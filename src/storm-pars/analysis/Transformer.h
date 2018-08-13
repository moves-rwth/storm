//
// Created by Jip Spel on 26.07.18.
//

#ifndef LATTICE_BUILDER_H
#define LATTICE_BUILDER_H

#include <storm/models/sparse/Model.h>
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "Lattice.h"
namespace storm {
    namespace analysis {
        class Transformer {
        public:
            struct State {
                uint_fast64_t stateNumber;
                uint_fast64_t successor1;
                uint_fast64_t successor2;
            };

            /*!
             * Returns the pointer to the Lattice constructed from the given transitionMatrix,
             * BitVector of initialStates, vector with the top states and vector with the bottom states.
             * Assumes that every state has at least one and at most two outgoing transitions.
             *
             * @param matrix The transition matrix.
             * @param initialStates The BitVector containing the initialStates.
             * @param topStates Vector containing the numbers of the top states.
             * @param bottomStates Vector containing the numbers of the bottom states.
             *
             * @return The lattice ordering of the states.
             */
            static Lattice *toLattice(storm::storage::SparseMatrix<storm::RationalFunction> matrix,
                                      storm::storage::BitVector const &initialStates,
                                      storm::storage::BitVector topStates,
                                      storm::storage::BitVector bottomStates, uint_fast64_t numberOfStates);

        private:
            static storm::RationalFunction getProbability(storm::storage::BitVector state, storm::storage::BitVector successor, storm::storage::SparseMatrix<storm::RationalFunction> matrix);

            static storm::RationalFunction getProbability(storm::storage::BitVector state, uint_fast64_t successor, storm::storage::SparseMatrix<storm::RationalFunction> matrix);

            static storm::RationalFunction getProbability(uint_fast64_t state, uint_fast64_t successor, storm::storage::SparseMatrix<storm::RationalFunction> matrix);

            };
    }
}
#endif //LATTICE_BUILDER_H
