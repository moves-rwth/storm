#include "storm-pars/analysis/OrderExtenderMdp.h"

namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        OrderExtenderMdp<ValueType, ConstantType>::OrderExtenderMdp(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, storage::ParameterRegion<ValueType> region) : OrderExtender<ValueType, ConstantType>(model, formula, region) {
            initMdpStateMap();
        }

        template<typename ValueType, typename ConstantType>
        OrderExtenderMdp<ValueType, ConstantType>::OrderExtenderMdp(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            initMdpStateMap();
        }

        template<typename ValueType, typename ConstantType>
        void OrderExtenderMdp<ValueType, ConstantType>::initMdpStateMap() {
            for (uint64_t stateNr = 0; stateNr < this->numberOfStates; stateNr++) {
                for (auto & row : this->matrix.getRowGroup(stateNr)) {
                    auto i = 0;
                    for (auto & rowEntry : row) {
                        mdpStateMap[stateNr][i].push_back(rowEntry.getColumn());
                    }
                    i++;
                }
            }
        }

        template<typename ValueType, typename ConstantType>
        storm::storage::BitVector OrderExtenderMdp<ValueType, ConstantType>::gatherPotentialSuccs(uint64_t state) {
            auto succs = mdpStateMap[state];
            auto res = storm::storage::BitVector(this->numberOfStates);
            for (auto & act : succs) {
                for (auto & succ : act) {
                    res.set(succ, true);
                }

            }
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtenderMdp<ValueType, ConstantType>::extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, bool prMax) {
            // Finding the best action for the current state
            // TODO implement =)/=( case (or is that not necessary?)
            if(mdpStateMap[currentState].size() == 1){
                // if we only have one possible action, we already know which one we take.
                order->addToMdpScheduler(currentState, 0);
            } else {
                // note that succs in this function mean potential succs
                auto orderedSuccs = order->sortStates(gatherPotentialSuccs(currentState));
                auto nrOfSuccs = orderedSuccs.size();
                uint64_t  bestAct = 0;
                if (prMax) {
                    if (nrOfSuccs == 2) {
                        uint64_t bestSucc = orderedSuccs[0];
                        // TODO is this the right way to create a constant function?
                        storm::RationalFunction bestFunc = storm::RationalFunction(0);
                        for (auto & action : this->matrix.getRowGroup(currentState)) {
                            auto itr = action.begin();
                            while (itr != action.end() && itr->getColumn != bestSucc) {
                                itr++;
                            }
                            if (itr != action.end() && itr->getEntry() > bestFunc) {
                                bestFunc = itr->getEntry();
                                // TODO How to get the index of an action?
                                // bestAct = action index;
                            }
                        }
                    } else {
                        // more than 2 succs
                        std::map<uint64_t, uint64_t> weightMap;
                        for (uint64_t i = 0; i < nrOfSuccs; i++) {
                            weightMap.insert(orderedSuccs[i], nrOfSuccs - i);
                        }
                        storm::RationalFunction bestCoeff = storm::RationalFunction(0);
                        for (auto & action : this->matrix.getRowGroup(currentState)) {
                            storm::RationalFunction currentCoeff = storm::RationalFunction(0);
                            for (auto & entry : action) {
                                // TODO does this work?
                                currentCoeff += entry.getEntry() * weightMap[entry.getColumn()];
                            }
                            // TODO how to compare rational functions (shouldn't there be regions involved?)
                            if (bestCoeff < currentCoeff) {
                                bestCoeff = currentCoeff;
                                // TODO How to get the index of an action?
                                // bestAct = action index;
                            }
                        }

                    }
                } else {
                    // We are interested in PrMin
                    // TODO make sure I am not having a big miscalculation in my thoughts here
                    if (nrOfSuccs == 2) {
                        uint64_t bestSucc = orderedSuccs[1];
                        // TODO is this the right way to create a constant function?
                        storm::RationalFunction bestFunc = storm::RationalFunction(2);
                        for (auto & action : this->matrix.getRowGroup(currentState)) {
                            auto itr = action.begin();
                            while (itr != action.end() && itr->getColumn != bestSucc) {
                                itr++;
                            }
                            if (itr != action.end() && itr->getEntry() < bestFunc) {
                                bestFunc = itr->getEntry();
                                // TODO How to get the index of an action?
                                // bestAct = action index;
                            }
                        }
                    } else {
                        // more than 2 succs
                        std::map<uint64_t, uint64_t> weightMap;
                        for (uint64_t i = 0; i < nrOfSuccs; i++) {
                            weightMap.insert(orderedSuccs[i], nrOfSuccs - i);
                        }
                        storm::RationalFunction bestCoeff = storm::RationalFunction(nrOfSuccs + 1);
                        for (auto & action : this->matrix.getRowGroup(currentState)) {
                            storm::RationalFunction currentCoeff = storm::RationalFunction(0);
                            for (auto & entry : action) {
                                // TODO does this work?
                                currentCoeff += entry.getEntry() * weightMap[entry.getColumn()];
                            }
                            // TODO how to compare rational functions (shouldn't there be regions involved?)
                            if (currentCoeff < bestCoeff) {
                                bestCoeff = currentCoeff;
                                // TODO How to get the index of an action?
                                // bestAct = action index;
                            }
                        }

                    }
                }

                order->addToMdpScheduler(currentState, bestAct);
            }

            // TODO actual extending of the order here
            std::vector<uint64_t> successors; // = actual successors resulting from the chosen action (need to be ordered, right? so maybe order them.);
            this->extendByBackwardReasoning(order, currentState, successors); // Base Class function

        }


    }
}