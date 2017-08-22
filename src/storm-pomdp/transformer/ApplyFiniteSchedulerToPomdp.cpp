#include "storm-pomdp/transformer/ApplyFiniteSchedulerToPomdp.h"

namespace storm {
    namespace transformer {

        struct RationalFunctionConstructor {
            RationalFunctionConstructor() : cache(std::make_shared<RawPolynomialCache>()) {

            }

            storm::RationalFunction translate(storm::RationalFunctionVariable const& var) {
                storm::Polynomial pol(storm::RawPolynomial(var), cache);
                return storm::RationalFunction(pol);
            }

            std::shared_ptr<RawPolynomialCache> cache;
        };

        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> ApplyFiniteSchedulerToPomdp<ValueType>::transform() const {
            uint64_t nrStates = pomdp.getNumberOfStates();
            std::unordered_map<uint32_t, std::vector<storm::RationalFunction>> parameters;
            bool nondeterminism = false;
            storm::storage::SparseMatrixBuilder<storm::RationalFunction> smb(nrStates, nrStates, 0, !nondeterminism, false, nrStates);

            RationalFunctionConstructor ratFuncConstructor;

            for (uint64_t state = 0; state < nrStates; ++state) {
                if (nondeterminism) {
                    smb.newRowGroup(state);
                }
                auto observation = pomdp.getObservation(state);
                auto it = parameters.find(observation);
                std::vector<storm::RationalFunction> localWeights;
                if (it == parameters.end()) {
                    storm::RationalFunction lastWeight(1);
                    for (uint64_t a = 0; a < pomdp.getNumberOfChoices(state) - 1; ++a) {
                        std::string varName = "p" + std::to_string(observation) + "_" + std::to_string(a);
                        localWeights.push_back(ratFuncConstructor.translate(carl::freshRealVariable(varName)));
                        lastWeight -= localWeights.back();
                    }
                    localWeights.push_back(lastWeight);
                    parameters.emplace(observation, localWeights);
                } else {
                    STORM_LOG_ASSERT(it->second.size() == pomdp.getNumberOfChoices(state), "Number of choices must be equal for every state with same number of actions");
                    localWeights = it->second;
                }
                std::map<uint64_t, storm::RationalFunction> weightedTransitions;
                for (uint64_t action = 0; action < pomdp.getNumberOfChoices(state); ++action) {
                    for (auto const& entry: pomdp.getTransitionMatrix().getRow(state, action)) {
                        auto it = weightedTransitions.find(entry.getColumn());
                        if (it == weightedTransitions.end()) {
                            weightedTransitions[entry.getColumn()] = storm::utility::convertNumber<storm::RationalFunctionCoefficient>(entry.getValue()) * localWeights[action]; //carl::rationalize<storm::RationalFunctionCoefficient>(entry.getValue()) * localWeights[action];
                        } else {
                            it->second += storm::utility::convertNumber<storm::RationalFunctionCoefficient>(entry.getValue()) * localWeights[action];
                        }
                    }
                }

                for (auto const& entry : weightedTransitions) {
                    smb.addNextValue(state, entry.first, entry.second);
                }
            }

            // TODO rewards.

            storm::storage::sparse::ModelComponents<storm::RationalFunction> modelComponents(smb.build(),pomdp.getStateLabeling());
            return std::make_shared<storm::models::sparse::Dtmc<storm::RationalFunction>>(modelComponents);

        }

        template class ApplyFiniteSchedulerToPomdp<storm::RationalNumber>;
    }
}