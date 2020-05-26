#include "storm-pomdp/transformer/GlobalPOMDPSelfLoopEliminator.h"
#include "storm/storage/BitVector.h"
#include <vector>
#include <storm/transformer/ChoiceSelector.h>

namespace storm {
    namespace transformer {


        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> GlobalPOMDPSelfLoopEliminator<ValueType>::transform() const
        {
            uint64_t nrStates = pomdp.getNumberOfStates();

            std::vector<storm::storage::BitVector> observationSelfLoopMasks;
            for (uint64_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
                observationSelfLoopMasks.push_back(storm::storage::BitVector(1, false));
                assert(observationSelfLoopMasks.back().size() == 1);
            }
            assert(pomdp.getNrObservations() >= 1);
            assert(observationSelfLoopMasks.size() == pomdp.getNrObservations());


            for (uint64_t state = 0; state < nrStates; ++state) {
                uint32_t observation = pomdp.getObservation(state);
                assert(pomdp.getNumberOfChoices(state) != 0);
                if (pomdp.getNumberOfChoices(state) == 1) {
                    continue;
                }
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

                STORM_LOG_ASSERT(observation < observationSelfLoopMasks.size(), "Observation index (" << observation << ") should be less than number of observations (" << observationSelfLoopMasks.size() << "). ");
                if (observationSelfLoopMasks[observation].size() == 1) {
                    observationSelfLoopMasks[observation] = actionVector;
                } else {
                    STORM_LOG_ASSERT(observationSelfLoopMasks[observation].size() == pomdp.getNumberOfChoices(state), "State " + std::to_string(state) + " has " + std::to_string(pomdp.getNumberOfChoices(state)) + " actions, different from other with same observation (" + std::to_string(observationSelfLoopMasks[observation].size()) + ")." );
                    observationSelfLoopMasks[observation] &= actionVector;
                }
            }

            storm::storage::BitVector filter(pomdp.getNumberOfChoices(), false);
            uint64_t offset = 0;
            for (uint64_t state = 0; state < nrStates; ++state) {
                uint32_t observation = pomdp.getObservation(state);
                storm::storage::BitVector& vec = observationSelfLoopMasks[observation];
                if (vec.full()) {
                    vec.set(0, false);
                }
                assert(!vec.full());
               // std::cout << "state " << state << " vec " << vec << std::endl;
                for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                    if (vec.get(action)) {
                        filter.set(offset + action);
                    }
                }
                offset += pomdp.getNumberOfChoices(state);
            }
           // std::cout << "filter: " << filter << std::endl;
            assert(filter.size() == pomdp.getNumberOfChoices());
            // TODO rewards with state-action rewards
            filter.complement();

         //   std::cout << "selection: " << filter << std::endl;

            ChoiceSelector<ValueType> cs(pomdp);
            auto res = cs.transform(filter)->template as<storm::models::sparse::Pomdp<ValueType>>();
            res->setIsCanonic();
            return res;
        }

        template class GlobalPOMDPSelfLoopEliminator<storm::RationalNumber>;

        template
        class GlobalPOMDPSelfLoopEliminator<double>;
    }
}
