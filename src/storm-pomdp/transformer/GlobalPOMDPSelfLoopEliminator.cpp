#include "storm-pomdp/transformer/GlobalPOMDPSelfLoopEliminator.h"
#include "storm/storage/BitVector.h"
#include <vector>

namespace storm {
    namespace transformer {


        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> GlobalPOMDPSelfLoopEliminator<ValueType>::transform() const
        {
            uint64_t nrStates = pomdp.getNumberOfStates();
            bool nondeterminism = false;

            std::vector<storm::storage::BitVector> observationSelfLoopMasks;
            for (uint64_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
                observationSelfLoopMasks.push_back(storm::storage::BitVector());
            }


            for (uint64_t state = 0; state < nrStates; ++state) {
                uint32_t observation = pomdp.getObservation(state);
                storm::storage::BitVector actionVector(pomdp.getNumberOfChoices(state), false);
                for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                    // We just look at the first entry.
                    for (auto const& entry: pomdp.getTransitionMatrix().getRow(state, action)) {
                        if (storm::utility::isOne(entry.getValue()) && entry.getColumn() == state) {
                            actionVector.set(action);
                        }
                        break;
                    }
                }
                if (observationSelfLoopMasks[observation].size() == 0) {
                    observationSelfLoopMasks[observation] = actionVector;
                } else {
                    observationSelfLoopMasks[observation] &= actionVector;
                }
            }

            storm::storage::BitVector filter(pomdp.getNumberOfChoices(), true);
            uint64_t offset = 0;
            for (uint64_t state = 0; state < nrStates; ++state) {
                uint32_t observation = pomdp.getObservation(state);
                storm::storage::BitVector& vec = observationSelfLoopMasks[observation];
                if (vec.full()) {
                    vec.set(0, false);
                }
                for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                    if (vec.get(action)) {
                        filter.set(offset + action);
                    }
                }
                offset += pomdp.getNumberOfChoices(state);
            }
            // TODO rewards.

            //storm::storage::sparse::ModelComponents<storm::RationalFunction> modelComponents(smb.build(),pomdp.getStateLabeling());
            //return std::make_shared<storm::models::sparse::Dtmc<storm::RationalFunction>>(modelComponents);

        }

        template class GlobalPOMDPSelfLoopEliminator<storm::RationalNumber>;
    }
}