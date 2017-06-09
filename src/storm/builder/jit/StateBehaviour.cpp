#include "storm/builder/jit/StateBehaviour.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/constants.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            StateBehaviour<IndexType, ValueType>::StateBehaviour() : expanded(false) {
                // Intentionally left empty.
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::addChoice(Choice<IndexType, ValueType>&& choice) {
                choices.emplace_back(std::move(choice));
            }
            
            template <typename IndexType, typename ValueType>
            Choice<IndexType, ValueType>& StateBehaviour<IndexType, ValueType>::addChoice(bool markovian) {
                choices.emplace_back(markovian);
                return choices.back();
            }
            
            template <typename IndexType, typename ValueType>
            typename StateBehaviour<IndexType, ValueType>::ContainerType const& StateBehaviour<IndexType, ValueType>::getChoices() const {
                return choices;
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::addStateReward(ValueType const& stateReward) {
                stateRewards.push_back(stateReward);
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::addStateRewards(std::vector<ValueType>&& stateRewards) {
                this->stateRewards = std::move(stateRewards);
            }
            
            template <typename IndexType, typename ValueType>
            std::vector<ValueType> const& StateBehaviour<IndexType, ValueType>::getStateRewards() const {
                return stateRewards;
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::reduce(storm::jani::ModelType const& modelType) {
                if (choices.size() > 1) {
                    if (modelType == storm::jani::ModelType::DTMC || modelType == storm::jani::ModelType::CTMC) {
                        reduceDeterministic(modelType);
                    } else if (modelType == storm::jani::ModelType::MA) {
                        reduceMarkovAutomaton();
                    } else {
                        for (auto& choice : choices) {
                            choice.compress();
                        }
                    }
                } else if (choices.size() == 1) {
                    choices.front().compress();
                }
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::reduceDeterministic(storm::jani::ModelType const& modelType) {
                std::size_t totalCount = choices.size();
                
                ValueType totalExitRate = modelType == storm::jani::ModelType::DTMC ? static_cast<ValueType>(totalCount) : storm::utility::zero<ValueType>();
                
                if (choices.front().getNumberOfRewards() > 0) {
                    std::vector<ValueType> newRewards(choices.front().getNumberOfRewards());
                    
                    if (modelType == storm::jani::ModelType::CTMC) {
                        for (auto const& choice : choices) {
                            ValueType massOfChoice = storm::utility::zero<ValueType>();
                            for (auto const& entry : choice.getDistribution()) {
                                massOfChoice += entry.getValue();
                            }
                            totalExitRate += massOfChoice;

                            auto outIt = newRewards.begin();
                            for (auto const& reward : choice.getRewards()) {
                                *outIt += reward * massOfChoice;
                                ++outIt;
                            }
                        }
                    } else {
                        for (auto const& choice : choices) {
                            auto outIt = newRewards.begin();
                            for (auto const& reward : choice.getRewards()) {
                                *outIt += reward;
                                ++outIt;
                            }
                        }
                    }
                    
                    for (auto& entry : newRewards) {
                        entry /= totalExitRate;
                    }

                    choices.front().setRewards(std::move(newRewards));
                }
                
                for (auto it = ++choices.begin(), ite = choices.end(); it != ite; ++it) {
                    choices.front().add(std::move(*it));
                }
                
                choices.resize(1);
                choices.front().compress();
                
                if (modelType == storm::jani::ModelType::DTMC) {
                    choices.front().divideDistribution(static_cast<ValueType>(totalCount));
                }
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::reduceMarkovAutomaton() {
                // If the model we build is a Markov Automaton, we reduce the choices by summing all Markovian choices
                // and making the Markovian choice the very first one (if there is any).
                bool foundPreviousMarkovianChoice = false;
                uint64_t numberOfChoicesToDelete = 0;
                
                for (uint_fast64_t index = 0; index + numberOfChoicesToDelete < this->size();) {
                    Choice<IndexType, ValueType>& choice = choices[index];
                    
                    if (choice.isMarkovian()) {
                        if (foundPreviousMarkovianChoice) {
                            // If there was a previous Markovian choice, we need to sum them. Note that we can assume
                            // that the previous Markovian choice is the very first one in the choices vector.
                            choices.front().add(std::move(choice));
                            
                            // Swap the choice to the end to indicate it can be removed (if it's not already there).
                            if (index != this->size() - 1) {
                                choice = std::move(choices[choices.size() - 1 - numberOfChoicesToDelete]);
                            }
                            ++numberOfChoicesToDelete;
                        } else {
                            // If there is no previous Markovian choice, just move the Markovian choice to the front.
                            if (index != 0) {
                                std::swap(choices.front(), choice);
                            }
                            foundPreviousMarkovianChoice = true;
                            ++index;
                        }
                    } else {
                        ++index;
                    }
                }
                
                // Finally remove the choices that were added to other Markovian choices.
                if (numberOfChoicesToDelete > 0) {
                    choices.resize(choices.size() - numberOfChoicesToDelete);
                }
            }
            
            template <typename IndexType, typename ValueType>
            bool StateBehaviour<IndexType, ValueType>::isHybrid() const {
                return choices.size() > 1 && choices.front().isMarkovian() && !choices.back().isMarkovian();
            }
            
            template <typename IndexType, typename ValueType>
            bool StateBehaviour<IndexType, ValueType>::isMarkovian() const {
                return choices.size() == 1 && choices.front().isMarkovian();
            }
            
            template <typename IndexType, typename ValueType>
            bool StateBehaviour<IndexType, ValueType>::isMarkovianOrHybrid() const {
                return choices.front().isMarkovian();
            }
            
            template <typename IndexType, typename ValueType>
            bool StateBehaviour<IndexType, ValueType>::isExpanded() const {
                return expanded;
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::setExpanded() {
                expanded = true;
            }
            
            template <typename IndexType, typename ValueType>
            bool StateBehaviour<IndexType, ValueType>::empty() const {
                return choices.empty();
            }
            
            template <typename IndexType, typename ValueType>
            std::size_t StateBehaviour<IndexType, ValueType>::size() const {
                return choices.size();
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::clear() {
                choices.clear();
                stateRewards.clear();
                expanded = false;
            }
            
            template class StateBehaviour<uint32_t, double>;
            template class StateBehaviour<uint32_t, storm::RationalNumber>;
            template class StateBehaviour<uint32_t, storm::RationalFunction>;
            
        }
    }
}
