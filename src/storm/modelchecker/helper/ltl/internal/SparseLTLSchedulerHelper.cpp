#include "SparseLTLSchedulerHelper.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/storage/memorystructure/MemoryStructureBuilder.h"
#include "storm/transformer/DAProductBuilder.h"

#include "storm/utility/graph.h"


namespace storm {
    namespace modelchecker {
        namespace helper {
            namespace internal {

                template<typename ValueType, bool Nondeterministic>
                SparseLTLSchedulerHelper<ValueType, Nondeterministic>::SparseLTLSchedulerHelper(uint_fast64_t numProductStates) : _randomScheduler(false), _producedChoices(), _infSets(), _accInfSets(numProductStates, boost::none) {
                    // Intentionally left empty.
                }


                template<typename ValueType, bool Nondeterministic>
                uint_fast64_t SparseLTLSchedulerHelper<ValueType, Nondeterministic>::SparseLTLSchedulerHelper::getMemoryState(uint_fast64_t daState, uint_fast64_t infSet) {
                    return (daState * (_infSets.size()+1))+ infSet;
                }

                template<typename ValueType, bool Nondeterministic>
                void SparseLTLSchedulerHelper<ValueType, Nondeterministic>::SparseLTLSchedulerHelper::setRandom() {
                    this->_randomScheduler = true;
                }

                template<typename ValueType, bool Nondeterministic>
                void SparseLTLSchedulerHelper<ValueType, Nondeterministic>::saveProductEcChoices(automata::AcceptanceCondition const& acceptance, storm::storage::MaximalEndComponent const& mec, std::vector<automata::AcceptanceCondition::acceptance_expr::ptr> const& conjunction, typename transformer::DAProduct<productModelType>::ptr product) {
                    // Save all states contained in this MEC
                    storm::storage::BitVector mecStates(product->getProductModel().getNumberOfStates(), false);
                    for (auto const &stateChoicePair : mec) {
                        mecStates.set(stateChoicePair.first);
                    }

                    // We know the MEC satisfied the conjunction: Save InfSets.
                    std::set<uint_fast64_t> infSetIds;
                    for (auto const& literal : conjunction) {
                        storm::storage::BitVector infSet;
                        if (literal->isTRUE()) {
                            // All states
                            infSet = storm::storage::BitVector(product->getProductModel().getNumberOfStates(), true);

                        } else if (literal->isAtom()) {
                            const cpphoafparser::AtomAcceptance &atom = literal->getAtom();
                            if (atom.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_INF) {
                                if (atom.isNegated()) {
                                    infSet =  ~acceptance.getAcceptanceSet(atom.getAcceptanceSet());

                                } else {
                                    infSet = acceptance.getAcceptanceSet(atom.getAcceptanceSet());
                                }
                            }
                            else if (atom.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_FIN) {
                                //  If there are FinSets in the conjunction we use the InfSet containing all states in this MEC
                                infSet = mecStates;
                            }
                        }

                        // Save new InfSets
                        if (infSet.size() > 0) {
                            auto it = std::find(_infSets.begin(), _infSets.end(), infSet);
                            if (it == _infSets.end()) {
                                infSetIds.insert(_infSets.size());
                                _infSets.emplace_back(infSet);
                            } else {
                                // save ID for accCond of the MEC states
                                infSetIds.insert(distance(_infSets.begin(), it));
                            }
                        }
                    }

                    // Save the InfSets into the _accInfSets for states in this MEC, but only if there weren't assigned to any other MEC yet.
                    storm::storage::BitVector newMecStates(product->getProductModel().getNumberOfStates(), false);
                    for (auto const &stateChoicePair : mec) {

                        if (_accInfSets[stateChoicePair.first] == boost::none) {
                            // state wasn't  assigned to any other MEC yet.
                            _accInfSets[stateChoicePair.first].emplace(infSetIds);
                            newMecStates.set(stateChoicePair.first);
                        }
                    }


                    // Define scheduler choices for the states in this MEC (that are not in any other MEC)
                    for (uint_fast64_t id : infSetIds) {
                        // Scheduler that satisfies the MEC acceptance condition (visit each InfSet inf often, or switch to scheduler of another MEC)
                        storm::storage::Scheduler<ValueType> mecScheduler(product->getProductModel().getNumberOfStates());

                        // States not in InfSet: Compute a scheduler that, with prob=1, reaches the infSet via mecStates starting from states that are not yet in other MEC
                        storm::utility::graph::computeSchedulerProb1E<ValueType>(newMecStates, product->getProductModel().getTransitionMatrix(), product->getProductModel().getBackwardTransitions(), mecStates, _infSets[id] & mecStates, mecScheduler);

                        // States that already reached the InfSet
                        for (auto pState : (newMecStates & _infSets[id])) {
                            // Prob1E sets an arbitrary choice for the psi states, but we want to stay in this accepting MEC.
                            mecScheduler.setChoice(*mec.getChoicesForState(pState).begin() - product->getProductModel().getTransitionMatrix().getRowGroupIndices()[pState], pState);
                        }

                        // Extract scheduler choices (only for states that are already assigned a scheduler, i.e are in another MEC)
                        for (auto pState : newMecStates) {
                            // We want to reach the InfSet, save choice:  <s, q, InfSetID> --->  choice
                            this->_producedChoices.insert({std::make_tuple(product->getModelState(pState), product->getAutomatonState(pState), id), mecScheduler.getChoice(pState)});
                        }
                    }
                }


                template<typename ValueType, bool Nondeterministic>
                void SparseLTLSchedulerHelper<ValueType, Nondeterministic>::prepareScheduler(uint_fast64_t numDaStates, storm::storage::BitVector const& acceptingProductStates, std::unique_ptr<storm::storage::Scheduler<ValueType>> reachScheduler, transformer::DAProductBuilder const& productBuilder, typename transformer::DAProduct<productModelType>::ptr product, storm::storage::BitVector const& modelStatesOfInterest, storm::storage::SparseMatrix<ValueType> const& transitionMatrix) {
                    // Compute size of the resulting memory structure: A state <q, infSet> is encoded as (q* (|infSets|+1))+ |infSet|
                    uint64 numMemoryStates = (numDaStates) * (_infSets.size()+1); //+1 for states outside accECs
                    _dontCareStates = std::vector<storm::storage::BitVector>(numMemoryStates, storm::storage::BitVector(transitionMatrix.getRowGroupCount(), false));

                    // Set choices for states or consider them "dontCare"
                    for (storm::storage::sparse::state_type automatonState= 0; automatonState < numDaStates; ++automatonState) {
                        for (storm::storage::sparse::state_type modelState = 0; modelState < transitionMatrix.getRowGroupCount(); ++modelState) {

                            if (!product->isValidProductState(modelState, automatonState)) {
                                // If the state <s,q> does not occur in the product model, all the infSet combinations are irrelevant for the scheduler.
                                for (uint_fast64_t infSet = 0; infSet < _infSets.size()+1; ++infSet) {
                                    _dontCareStates[getMemoryState(automatonState, infSet)].set(modelState, true);
                                }
                            } else {
                                auto pState = product->getProductStateIndex(modelState, automatonState);
                                if (acceptingProductStates.get(pState)) {
                                    // For states in accepting ECs set the missing MEC-scheduler combinations are "dontCare", they are not reachable using the scheduler choices.
                                    for (uint_fast64_t infSet = 0; infSet < _infSets.size()+1; ++infSet) {
                                        if (_producedChoices.find(std::make_tuple(product->getModelState(pState), product->getAutomatonState(pState), infSet)) == _producedChoices.end() ) {
                                            _dontCareStates[getMemoryState(product->getAutomatonState(pState), infSet)].set(product->getModelState(pState), true);
                                        }
                                    }

                                } else {
                                    // Extract the choices of the REACH-scheduler (choices to reach an acc. MEC) for the MDP-DA product: <s,q> -> choice. The memory structure corresponds to the "last" copy of the DA (_infSets.get().size()).
                                    this->_accInfSets[pState] = std::set<uint_fast64_t>({_infSets.size()});
                                    if (reachScheduler->isDontCare(pState)) {
                                        // Mark the maybe States of the untilProbability scheduler as "dontCare"
                                        _dontCareStates[getMemoryState(product->getAutomatonState(pState), _infSets.size())].set(product->getModelState(pState), true);
                                    } else {
                                        // Set choice For non-accepting states that are not in any accepting EC
                                        this->_producedChoices.insert({std::make_tuple(product->getModelState(pState),product->getAutomatonState(pState),_infSets.size()),reachScheduler->getChoice(pState)});
                                    };
                                    // All other InfSet combinations are unreachable (dontCare)
                                    for (uint_fast64_t infSet = 0; infSet < _infSets.size(); ++infSet) {
                                        _dontCareStates[getMemoryState(product->getAutomatonState(pState), infSet)].set(product->getModelState(pState), true);

                                    }
                                }
                            }
                        }
                    }


                    // Prepare the memory structure. For that, we need: transitions,  initialMemoryStates (and memoryStateLabeling)

                    // The next move function of the memory, will be build based on the transitions of the DA and jumps between InfSets.
                    _memoryTransitions = std::vector<std::vector<storm::storage::BitVector>>(numMemoryStates, std::vector<storm::storage::BitVector>(numMemoryStates, storm::storage::BitVector(transitionMatrix.getRowGroupCount(), false)));
                    for (storm::storage::sparse::state_type automatonFrom = 0; automatonFrom < numDaStates; ++automatonFrom) {
                        for (storm::storage::sparse::state_type modelState = 0; modelState < transitionMatrix.getRowGroupCount(); ++modelState) {
                            uint_fast64_t automatonTo = productBuilder.getSuccessor(automatonFrom, modelState);

                            if (product->isValidProductState(modelState, automatonTo)) {
                                // Add the modelState to one outgoing transition of all states of the form <automatonFrom, InfSet> (Inf=lenInfSet equals not in MEC)
                                // For non-accepting states that are not in any accepting EC we use the 'last' copy of the DA
                                // and for the accepting states we jump through copies of the DA wrt. the infinity sets.
                                for (uint_fast64_t infSet = 0; infSet < _infSets.size()+1; ++infSet) {
                                    // Check if we need to switch the acceptance condition
                                    STORM_LOG_ASSERT(_accInfSets[product->getProductStateIndex(modelState, automatonTo)] != boost::none, "The list of InfSets for the product state <" <<modelState<< ", " << automatonTo<<"> is undefined.");

                                    if (_accInfSets[product->getProductStateIndex(modelState, automatonTo)].get().count(infSet) == 0) {
                                        // the state is is in a different accepting MEC with a different accepting conjunction of InfSets.
                                        auto newInfSet = _accInfSets[product->getProductStateIndex(modelState, automatonTo)].get().begin();
                                        _memoryTransitions[getMemoryState(automatonFrom, infSet)][getMemoryState(automatonTo, *newInfSet)].set(modelState);

                                    } else {
                                        // Continue looking for any accepting EC (if we haven't reached one yet) or stay in the corresponding accepting EC, test whether we have reached the next infSet.
                                        if (infSet == _infSets.size() || !(_infSets[infSet].get(product->getProductStateIndex(modelState, automatonTo)))) {
                                            // <modelState, automatonTo> is not in any accepting EC or does not satisfy the InfSet, we stay there.
                                            // Add modelState to the transition from <automatonFrom, InfSet> to <automatonTo, InfSet>
                                            _memoryTransitions[getMemoryState(automatonFrom, infSet)][getMemoryState(automatonTo, infSet)].set(modelState);

                                        } else {
                                            STORM_LOG_ASSERT(_accInfSets[product->getProductStateIndex(modelState, automatonTo)] != boost::none, "The list of InfSets for the product state <" <<modelState<< ", " << automatonTo<<"> is undefined.");
                                            //  <modelState, automatonTo> satisfies the InfSet, find the next one.
                                            auto nextInfSet = std::find(_accInfSets[product->getProductStateIndex(modelState, automatonTo)].get().begin(), _accInfSets[product->getProductStateIndex(modelState, automatonTo)].get().end(), infSet);
                                            STORM_LOG_ASSERT(nextInfSet != _accInfSets[product->getProductStateIndex(modelState, automatonTo)].get().end(), "The list of InfSets for the product state <" <<modelState<< ", " << automatonTo<<"> does not contain the infSet " << infSet);
                                            nextInfSet++;
                                            if (nextInfSet == _accInfSets[product->getProductStateIndex(modelState, automatonTo)].get().end()) {
                                                // Start again.
                                                nextInfSet = _accInfSets[product->getProductStateIndex(modelState, automatonTo)].get().begin();
                                            }
                                            // Add modelState to the transition from <automatonFrom <mec, InfSet>> to  <automatonTo, <mec, NextInfSet>>.
                                            _memoryTransitions[getMemoryState(automatonFrom, infSet)][getMemoryState(automatonTo, *nextInfSet)].set(modelState);
                                        }
                                    }
                                }
                            }

                        }
                    }
                    // Finished creation of transitions.

                    // Find initial memory states
                    this->_memoryInitialStates = std::vector<uint_fast64_t>(transitionMatrix.getRowGroupCount());
                    // Save for each relevant model state its initial memory state (get the s-successor q of q0)
                    for (storm::storage::sparse::state_type modelState : modelStatesOfInterest) {
                        storm::storage::sparse::state_type automatonState = productBuilder.getInitialState(modelState);
                        STORM_LOG_ASSERT(product->isValidProductState(modelState, automatonState), "The memory successor state for the model state "<< modelState << "does not exist in the DA-Model Product.");
                        if (acceptingProductStates[product->getProductStateIndex(modelState, automatonState)]) {
                            STORM_LOG_ASSERT(_accInfSets[product->getProductStateIndex(modelState, automatonState)] != boost::none, "The list of InfSets for the product state <" <<modelState<< ", " << automatonState<<"> is undefined.");
                            // If <s, q> is an accepting state start in the first InfSet of <s, q>.
                            auto infSet = _accInfSets[product->getProductStateIndex(modelState, automatonState)].get().begin();
                            _memoryInitialStates[modelState] = getMemoryState(automatonState, *infSet);

                        } else {
                            _memoryInitialStates[modelState] = getMemoryState(automatonState, _infSets.size());
                        }

                    }

                }


                template<typename ValueType, bool Nondeterministic>
                storm::storage::Scheduler<ValueType> SparseLTLSchedulerHelper<ValueType, Nondeterministic>::SparseLTLSchedulerHelper::extractScheduler(storm::models::sparse::Model<ValueType> const& model, bool onlyInitialStatesRelevant) {

                    if (_randomScheduler) {
                        storm::storage::Scheduler<ValueType> scheduler(model.getNumberOfStates());
                        for (storm::storage::sparse::state_type state = 0; state < model.getNumberOfStates(); ++state) {
                            scheduler.setChoice(0, state);

                        }
                        return scheduler;
                    }

                    // Otherwise, we compute a scheduler with memory.

                    // Create a memory structure for the MDP scheduler with memory. If hasRelevantStates is set, we only consider initial model states relevant.
                    auto memoryBuilder = storm::storage::MemoryStructureBuilder<ValueType>(this->_memoryTransitions.size(), model, onlyInitialStatesRelevant);

                    // Build the transitions between the memory states: startState to goalState using modelStates (transitionVector).
                    for (storm::storage::sparse::state_type startState = 0; startState < this->_memoryTransitions.size(); ++startState) {
                        for (storm::storage::sparse::state_type goalState = 0; goalState < this->_memoryTransitions.size(); ++goalState) {
                            // Bitvector that represents modelStates the model states that trigger this transition.
                            memoryBuilder.setTransition(startState, goalState, this->_memoryTransitions[startState][goalState]);
                        }
                    }

                    // InitialMemoryStates: Assign an initial memory state model states
                    if (onlyInitialStatesRelevant) {
                        // Only consider initial model states
                        for (uint_fast64_t modelState : model.getInitialStates()) {
                            memoryBuilder.setInitialMemoryState(modelState, this->_memoryInitialStates[modelState]);
                        }
                    } else {
                        // All model states are relevant
                        for (uint_fast64_t modelState = 0; modelState < model.getNumberOfStates(); ++modelState) {
                            memoryBuilder.setInitialMemoryState(modelState, this->_memoryInitialStates[modelState]);
                        }
                    }

                    // Build the memoryStructure.
                    storm::storage::MemoryStructure memoryStructure = memoryBuilder.build();

                    // Create a scheduler (with memory) for the model from the REACH and MEC scheduler of the MDP-DA-product model.
                    storm::storage::Scheduler<ValueType> scheduler(model.getNumberOfStates(), memoryStructure);

                    // Use choices in the product model to create a choice based on model state and memory state
                    for (const auto &choice : this->_producedChoices) {
                        // <s, q, InfSet> -> choice
                        storm::storage::sparse::state_type modelState = std::get<0>(choice.first);
                        storm::storage::sparse::state_type daState = std::get<1>(choice.first);
                        uint_fast64_t infSet = std::get<2>(choice.first);
                        STORM_LOG_ASSERT(!this->_dontCareStates[getMemoryState(daState, infSet)].get(modelState), "Tried to set choice for dontCare state.");
                        scheduler.setChoice(choice.second, modelState, getMemoryState(daState, infSet));
                    }

                    // Set "dontCare" states
                    for (uint_fast64_t memoryState = 0; memoryState < this->_dontCareStates.size(); ++memoryState) {
                        for (auto state : this->_dontCareStates[memoryState]) {
                            scheduler.setDontCare(state, memoryState);
                        }
                    }

                    // Sanity check for created scheduler.
                    STORM_LOG_ASSERT(scheduler.isDeterministicScheduler(), "Expected a deterministic scheduler");
                    STORM_LOG_ASSERT(!scheduler.isPartialScheduler(), "Expected a fully defined scheduler");

                    return scheduler;

                }

                template class SparseLTLSchedulerHelper<double, false>;
                template class SparseLTLSchedulerHelper<double, true>;

#ifdef STORM_HAVE_CARL
                template class SparseLTLSchedulerHelper<storm::RationalNumber, false>;
                template class SparseLTLSchedulerHelper<storm::RationalNumber, true>;
                template class SparseLTLSchedulerHelper<storm::RationalFunction, false>;

#endif

            }
        }
    }
}