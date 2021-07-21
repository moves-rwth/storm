#include "SparseLTLHelper.h"

#include "storm/automata/LTL2DeterministicAutomaton.h"

#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"

#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/storage/memorystructure/MemoryStructureBuilder.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/exceptions/InvalidPropertyException.h"

#include "storm/environment/modelchecker/ModelCheckerEnvironment.h"

#include "storm/utility/graph.h"

namespace storm {
    namespace modelchecker {
        namespace helper {

            template <typename ValueType, bool Nondeterministic>
            SparseLTLHelper<ValueType, Nondeterministic>::SparseLTLHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::size_t numberOfStates) : _transitionMatrix(transitionMatrix), _numberOfStates(numberOfStates){
                // Intentionally left empty.
            }


            template <typename ValueType, bool Nondeterministic>
            storm::storage::Scheduler<ValueType> SparseLTLHelper<ValueType, Nondeterministic>::SparseLTLHelper::extractScheduler(storm::models::sparse::Model<ValueType> const& model) {
                STORM_LOG_ASSERT(this->isProduceSchedulerSet(), "Trying to get the produced optimal choices although no scheduler was requested.");
                // If Pmax(phi) = 0 or Pmin(phi) = 1, we return a memoryless scheduler with arbitrary choices
                if (_randomScheduler) {
                    storm::storage::Scheduler<ValueType> scheduler(this->_transitionMatrix.getRowGroupCount());
                    for (storm::storage::sparse::state_type state = 0; state < this->_transitionMatrix.getRowGroupCount(); ++state) {
                        scheduler.setChoice(0, state);

                    }
                    return scheduler;
                }

                // Otherwise, we compute a scheduler with memory.
                STORM_LOG_ASSERT(this->_productChoices.is_initialized(), "Trying to extract the produced scheduler but none is available. Was there a computation call before?");
                STORM_LOG_ASSERT(this->_memoryTransitions.is_initialized(), "Trying to extract the DA transition structure but none is available. Was there a computation call before?");
                STORM_LOG_ASSERT(this->_memoryInitialStates.is_initialized(), "Trying to extract the initial states of the DA but there are none available. Was there a computation call before?");


                // Create a memory structure for the MDP scheduler with memory. If hasRelevantStates is set, we only consider initial model states relevant.
                typename storm::storage::MemoryStructureBuilder<ValueType>::MemoryStructureBuilder memoryBuilder(this->_memoryTransitions.get().size(), model, this->hasRelevantStates());

                // Build the transitions between the memory states:  startState--- modelStates (transitionVector) --->goalState
                for (storm::storage::sparse::state_type startState = 0; startState < this->_memoryTransitions.get().size(); ++startState) {
                    for (storm::storage::sparse::state_type goalState = 0; goalState < this->_memoryTransitions.get().size(); ++goalState) {
                        // Bitvector that represents modelStates the model states that trigger this transition.
                        memoryBuilder.setTransition(startState, goalState, this->_memoryTransitions.get()[startState][goalState]);
                    }
                }

                // initialMemoryStates: Assign an initial memory state model states
                if (this->hasRelevantStates()) {
                    // Only consider initial model states
                    for (uint_fast64_t modelState : model.getInitialStates()) {
                        memoryBuilder.setInitialMemoryState(modelState, this->_memoryInitialStates.get()[modelState]);
                    }
                } else {
                    // All model states are relevant
                    for (uint_fast64_t modelState = 0; modelState < model.getNumberOfStates(); ++modelState) {
                        memoryBuilder.setInitialMemoryState(modelState, this->_memoryInitialStates.get()[modelState]);
                    }
                }


                // Now, we can build the memoryStructure.
                storm::storage::MemoryStructure memoryStructure = memoryBuilder.build();

                // Create a scheduler (with memory) for the model from the REACH and MEC scheduler of the MDP-DA-product model.
                storm::storage::Scheduler<ValueType> scheduler(this->_transitionMatrix.getRowGroupCount(), memoryStructure);

                // Use choices in the product model to create a choice based on model state and memory state
                for (const auto &choice : this->_productChoices.get()) {
                    // <s, q, InfSet> -> choice
                    storm::storage::sparse::state_type modelState = std::get<0>(choice.first);
                    storm::storage::sparse::state_type automatonState = std::get<1>(choice.first);
                    uint_fast64_t infSet = std::get<2>(choice.first);

                    scheduler.setChoice(choice.second, modelState, (automatonState*(_infSets.get().size()+1))+ infSet);

                    // TODO: shouldn't happen?
                    //STORM_LOG_ASSERT(!this->_unreachableStates.get()[(automatonState*(_infSets.get().size()+1))+ infSet].get(modelState), "Tried to set choice for unreachable state.");
                }

                /*
                for (uint_fast64_t memoryState = 0; this->_unreachableStates.get().size(); ++memoryState) {
                    for (auto state : this->_unreachableStates.get()[memoryState]) {
                        // todo fails, memory state = 9
                        scheduler.setStateUnreachable(state, memoryState);
                    }
                }
                 */



                // Sanity check for created scheduler.
                STORM_LOG_ASSERT(scheduler.isDeterministicScheduler(), "Expected a deterministic scheduler");
                STORM_LOG_ASSERT(!scheduler.isPartialScheduler(), "Expected a fully defined scheduler");
                return scheduler;

            }

            template<typename ValueType, bool Nondeterministic>
            std::map<std::string, storm::storage::BitVector> SparseLTLHelper<ValueType, Nondeterministic>::computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, std::function<std::unique_ptr<CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula)> formulaChecker){
                std::map<std::string, storm::storage::BitVector> apSets;
                for (auto& p: extracted) {
                    STORM_LOG_INFO(" Computing satisfaction set for atomic proposition \"" << p.first << "\" <=> " << *p.second << "...");

                    std::unique_ptr<CheckResult> subResultPointer = formulaChecker(p.second);

                    ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
                    auto sat = subResult.getTruthValuesVector();

                    apSets[p.first] = std::move(sat);
                    STORM_LOG_INFO(" Atomic proposition \"" << p.first << "\" is satisfied by " << apSets[p.first].getNumberOfSetBits() << " states.");
                }
                return apSets;
            }


            template <typename ValueType, bool Nondeterministic>
            storm::storage::BitVector SparseLTLHelper<ValueType, Nondeterministic>::computeAcceptingECs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions,  typename transformer::DAProduct<productModelType>::ptr product) {
                STORM_LOG_INFO("Computing accepting states for acceptance condition " << *acceptance.getAcceptanceExpression());
                if (acceptance.getAcceptanceExpression()->isTRUE()) {
                    STORM_LOG_INFO(" TRUE -> all states accepting (assumes no deadlock in the model)");
                    return storm::storage::BitVector(transitionMatrix.getRowGroupCount(), true);
                } else if (acceptance.getAcceptanceExpression()->isFALSE()) {
                    STORM_LOG_INFO(" FALSE -> all states rejecting");
                    return storm::storage::BitVector(transitionMatrix.getRowGroupCount(), false);
                }

                std::vector<std::vector<automata::AcceptanceCondition::acceptance_expr::ptr>> dnf = acceptance.extractFromDNF();

                storm::storage::BitVector acceptingStates(transitionMatrix.getRowGroupCount(), false);

                std::size_t accMECs = 0;
                std::size_t allMECs = 0;
                std::size_t i = 0;

                if (this->isProduceSchedulerSet()) {
                    _infSets.emplace();
                    _accInfSets.emplace(product->getProductModel().getNumberOfStates(), boost::none);
                    _productChoices.emplace();
                }

                for (auto const& conjunction : dnf) {
                    // determine the set of states of the subMDP that can satisfy the condition
                    //  => remove all states that would violate Fins in the conjunction
                    storm::storage::BitVector allowed(transitionMatrix.getRowGroupCount(), true);

                    STORM_LOG_INFO("Handle conjunction " << i);

                    for (auto const& literal : conjunction) {
                        STORM_LOG_INFO(" " << *literal);
                        if (literal->isTRUE()) {
                            // skip
                        } else if (literal->isFALSE()) {
                            allowed.clear();
                            break;
                        } else if (literal->isAtom()) {
                            const cpphoafparser::AtomAcceptance& atom = literal->getAtom();
                            if (atom.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_FIN) {
                                // only deal with FIN, ignore INF here
                                const storm::storage::BitVector& accSet = acceptance.getAcceptanceSet(atom.getAcceptanceSet());
                                if (atom.isNegated()) {
                                    // allowed = allowed \ ~accSet = allowed & accSet
                                    allowed &= accSet;
                                } else {
                                    // allowed = allowed \ accSet = allowed & ~accSet
                                    allowed &= ~accSet;
                                }
                            }
                        }
                    }

                    if (allowed.empty()) {
                        // skip
                        continue;
                    }

                    STORM_LOG_DEBUG(" Allowed states: " << allowed);

                    // compute MECs in the allowed fragment
                    storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(transitionMatrix, backwardTransitions, allowed);
                    allMECs += mecs.size();
                    for (const auto& mec : mecs) {
                        STORM_LOG_DEBUG("Inspect MEC: " << mec);

                        bool accepting = true;
                        for (auto const& literal : conjunction) {
                            if (literal->isTRUE()) {
                                // skip

                            } else if (literal->isFALSE()) {
                                accepting = false;
                                break;
                            } else if (literal->isAtom()) {
                                const cpphoafparser::AtomAcceptance& atom = literal->getAtom();
                                const storm::storage::BitVector& accSet = acceptance.getAcceptanceSet(atom.getAcceptanceSet());
                                if (atom.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_INF) {
                                    if (atom.isNegated()) {
                                        STORM_LOG_DEBUG("Checking against " << ~accSet);
                                        if (!mec.containsAnyState(~accSet)) {
                                            STORM_LOG_DEBUG("  -> not satisfied");
                                            accepting = false;
                                            break;
                                        }

                                    } else {
                                        STORM_LOG_DEBUG("Checking against " << accSet);
                                        if (!mec.containsAnyState(accSet)) {
                                            STORM_LOG_DEBUG("  -> not satisfied");
                                            accepting = false;
                                            break;
                                        }

                                    }

                                } else if (atom.getType() == cpphoafparser::AtomAcceptance::TEMPORAL_FIN) {
                                    // do only sanity checks here
                                    STORM_LOG_ASSERT(atom.isNegated() ? !mec.containsAnyState(~accSet) : !mec.containsAnyState(accSet), "MEC contains Fin-states, which should have been removed");
                                }
                            }
                        }

                        if (accepting) {
                            accMECs++;
                            STORM_LOG_DEBUG("MEC is accepting");

                            for (auto const &stateChoicePair : mec) {
                                acceptingStates.set(stateChoicePair.first);
                            }

                            if (this->isProduceSchedulerSet()) {

                                // Save all states contained in this MEC
                                storm::storage::BitVector mecStates(product->getProductModel().getNumberOfStates(), false);
                                for (auto const &stateChoicePair : mec) {
                                    mecStates.set(stateChoicePair.first);
                                }

                                // We know the MEC satisfied the conjunction: Save InfSets
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
                                        auto it = std::find(_infSets.get().begin(), _infSets.get().end(), infSet);
                                        if (it == _infSets.get().end()) {
                                            infSetIds.insert(_infSets.get().size());
                                            _infSets.get().emplace_back(infSet);
                                        } else {
                                            // save ID for accCond of the MEC states
                                            infSetIds.insert(distance(_infSets.get().begin(), it));
                                        }
                                    }
                                }

                                // Save the InfSets into the _accInfSets for states in this MEC, but only if there weren't assigned to any other MEC yet.
                                storm::storage::BitVector newMecStates(product->getProductModel().getNumberOfStates(), false);
                                for (auto const &stateChoicePair : mec) {
                                    if (_accInfSets.get()[stateChoicePair.first] == boost::none) {
                                        // state wasn't  assigned to any other MEC yet.
                                        _accInfSets.get()[stateChoicePair.first] = infSetIds;
                                        newMecStates.set(stateChoicePair.first);

                                    }
                                }


                                // Define scheduler choices for the states in this MEC (that are not in any other MEC)
                                for (uint_fast64_t id : infSetIds) {
                                    // Scheduler that satisfies the MEC acceptance condition (visit each InfSet inf often, or switch to scheduler of another MEC)
                                    storm::storage::Scheduler<ValueType> mecScheduler(transitionMatrix.getRowGroupCount());

                                    // States not in InfSet: Compute a scheduler that, with prob=1, reaches the infSet via mecStates starting from states that are not yet in other MEC
                                    storm::utility::graph::computeSchedulerProb1E<ValueType>(newMecStates, transitionMatrix, backwardTransitions, mecStates, _infSets.get()[id] & mecStates, mecScheduler); //todo &mecstates? nec.?

                                    // States that already reached the InfSet: Prob1E sets an arbitrary choice for the psi states, but we want to stay in this accepting mec.

                                    for (auto pState : (newMecStates & _infSets.get()[id])) {
                                        // If we are in the InfSet, we move to some state in this MEC.
                                        mecScheduler.setChoice(*mec.getChoicesForState(pState).begin() - transitionMatrix.getRowGroupIndices()[pState], pState);
                                    }

                                    // Extract scheduler choices (only for states that are already assigned a scheduler, i.e are in another MEC)
                                    for (auto pState : newMecStates) {
                                        // We want to reach the InfSet, save choice:  <s, q, InfSetID> --->  choice
                                        this->_productChoices.get().insert({std::make_tuple(product->getModelState(pState), product->getAutomatonState(pState), id),  mecScheduler.getChoice(pState)});
                                    }
                                }
                            }
                        }

                    }
                }

                STORM_LOG_DEBUG("Accepting states: " << acceptingStates);
                STORM_LOG_INFO("Found " << acceptingStates.getNumberOfSetBits() << " states in " << accMECs << " accepting MECs (considered " << allMECs << " MECs).");

                return acceptingStates;
            }

            template <typename ValueType, bool Nondeterministic>
            storm::storage::BitVector SparseLTLHelper<ValueType, Nondeterministic>::computeAcceptingBCCs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix) {
                storm::storage::StronglyConnectedComponentDecomposition<ValueType> bottomSccs(transitionMatrix, storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs().dropNaiveSccs());
                storm::storage::BitVector acceptingStates(transitionMatrix.getRowGroupCount(), false);

                std::size_t checkedBSCCs = 0, acceptingBSCCs = 0, acceptingBSCCStates = 0;
                for (auto& scc : bottomSccs) {
                    checkedBSCCs++;
                    if (acceptance.isAccepting(scc)) {
                        acceptingBSCCs++;
                        for (auto& state : scc) {
                            acceptingStates.set(state);
                            acceptingBSCCStates++;
                        }
                    }
                }
                STORM_LOG_INFO("BSCC analysis: " << acceptingBSCCs << " of " << checkedBSCCs << " BSCCs were acceptingStates (" << acceptingBSCCStates << " states in acceptingStates BSCCs).");
                return  acceptingStates;
            }

            template<typename ValueType, bool Nondeterministic>
            std::vector<ValueType> SparseLTLHelper<ValueType, Nondeterministic>::computeDAProductProbabilities(Environment const& env, storm::automata::DeterministicAutomaton const& da, std::map<std::string, storm::storage::BitVector>& apSatSets) {
                const storm::automata::APSet& apSet = da.getAPSet();


                std::vector<storm::storage::BitVector> statesForAP;
                for (const std::string& ap : apSet.getAPs()) {
                    auto it = apSatSets.find(ap);
                    STORM_LOG_THROW(it != apSatSets.end(), storm::exceptions::InvalidOperationException, "Deterministic automaton has AP " << ap << ", does not appear in formula");

                    statesForAP.push_back(std::move(it->second));
                }

                storm::storage::BitVector statesOfInterest;

                if (this->hasRelevantStates()) {
                    statesOfInterest = this->getRelevantStates();
                } else {
                    // product from all model states
                    statesOfInterest = storm::storage::BitVector(this->_numberOfStates, true);
                }


                STORM_LOG_INFO("Building "+ (Nondeterministic ? std::string("MDP-DA") : std::string("DTMC-DA")) +" product with deterministic automaton, starting from " << statesOfInterest.getNumberOfSetBits() << " model states...");
                transformer::DAProductBuilder productBuilder(da, statesForAP);

                typename transformer::DAProduct<productModelType>::ptr product = productBuilder.build<productModelType>(this->_transitionMatrix, statesOfInterest);


                STORM_LOG_INFO("Product "+ (Nondeterministic ? std::string("MDP-DA") : std::string("DTMC-DA")) +" has " << product->getProductModel().getNumberOfStates() << " states and "
                                                   << product->getProductModel().getNumberOfTransitions() << " transitions.");

                if (storm::settings::getModule<storm::settings::modules::DebugSettings>().isTraceSet()) {
                    STORM_LOG_TRACE("Writing product model to product.dot");
                    std::ofstream productDot("product.dot");
                    product->getProductModel().writeDotToStream(productDot);
                    productDot.close();

                    STORM_LOG_TRACE("Product model mapping:");
                    std::stringstream str;
                    product->printMapping(str);
                    STORM_LOG_TRACE(str.str());
                }

                // Compute accepting states
                storm::storage::BitVector acceptingStates;
                if (Nondeterministic) {
                    STORM_LOG_INFO("Computing MECs and checking for acceptance...");
                    acceptingStates = computeAcceptingECs(*product->getAcceptance(), product->getProductModel().getTransitionMatrix(), product->getProductModel().getBackwardTransitions(), product); //TODO product is only needed for ->getModelState(pState) and DAStates for scheduler creation (maybe lambda instead)

                    } else {
                    STORM_LOG_INFO("Computing BSCCs and checking for acceptance...");
                    acceptingStates = computeAcceptingBCCs(*product->getAcceptance(), product->getProductModel().getTransitionMatrix());

                }

                if (acceptingStates.empty()) {
                    STORM_LOG_INFO("No accepting states, skipping probability computation.");
                    std::vector<ValueType> numericResult(this->_numberOfStates, storm::utility::zero<ValueType>());
                    this->_randomScheduler = true;
                    return numericResult;
                }

                STORM_LOG_INFO("Computing probabilities for reaching accepting components...");

                storm::storage::BitVector bvTrue(product->getProductModel().getNumberOfStates(), true);
                storm::storage::BitVector soiProduct(product->getStatesOfInterest());

                // Create goal for computeUntilProbabilities, always compute maximizing probabilities
                storm::solver::SolveGoal<ValueType> solveGoalProduct;
                if (this->isValueThresholdSet()) {
                    solveGoalProduct = storm::solver::SolveGoal<ValueType>(OptimizationDirection::Maximize, this->getValueThresholdComparisonType(), this->getValueThresholdValue(), std::move(soiProduct));
                } else {
                    solveGoalProduct = storm::solver::SolveGoal<ValueType>(OptimizationDirection::Maximize);
                    solveGoalProduct.setRelevantValues(std::move(soiProduct));
                }

                std::vector<ValueType> prodNumericResult;


                if (Nondeterministic) {
                    MDPSparseModelCheckingHelperReturnType<ValueType> prodCheckResult = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(env,
                                                                                                                              std::move(solveGoalProduct),
                                                                                                                              product->getProductModel().getTransitionMatrix(),
                                                                                                                              product->getProductModel().getBackwardTransitions(),
                                                                                                                              bvTrue,
                                                                                                                              acceptingStates,
                                                                                                                              this->isQualitativeSet(),
                                                                                                                              this->isProduceSchedulerSet() // Whether to create memoryless scheduler for the Model-DA Product.
                                                                                                                              );
                    prodNumericResult = std::move(prodCheckResult.values);

                    if (this->isProduceSchedulerSet()) {  // TODO create fct for this

                        // TODO asserts _mecStatesInfSets initialized etc.

                        // Compute size of the resulting memory structure: a state corresponds to <q, infSet>> encoded as (q* (|infSets|+1))+infSet
                        uint64 numMemoryStates = (da.getNumberOfStates() * (_infSets.get().size()+1)) + _infSets.get().size()+1; //+1 for states outside accECs

                        _unreachableStates.emplace(numMemoryStates, storm::storage::BitVector(this->_transitionMatrix.getRowGroupCount(), false));


                        // Extract the choices of the REACH-scheduler (choices to reach an acc. MEC) for the MDP-DA product: <s,q> -> choice
                        for (storm::storage::sparse::state_type pState : ~acceptingStates) {

                            // For non-accepting states that are not in any accepting EC we use the 'last' copy of the DA
                            this->_accInfSets.get()[pState] = {_infSets.get().size()};

                            // For state <s,q> not in any accEC <s,q, REACH> --->  choice. Do not overwrite choices of states in an accepting MEC.
                            this->_productChoices.get().insert({std::make_tuple(product->getModelState(pState), product->getAutomatonState(pState), _infSets.get().size()), prodCheckResult.scheduler->getChoice(pState)});
                            if (!prodCheckResult.scheduler->isStateReachable(pState)) {
                                //TODO
                                this->_unreachableStates.get()[(product->getAutomatonState(pState)) * (_infSets.get().size()+1) + _infSets.get().size()].set(product->getModelState(pState));
                            }
                        }

                        // Prepare the memory structure. For that, we need: transitions,  initialMemoryStates (and memoryStateLabeling)



                        // The next move function of the memory, will be build based on the transitions of the DA and jumps between InfSets.
                        this->_memoryTransitions.emplace(numMemoryStates, std::vector<storm::storage::BitVector>(numMemoryStates, storm::storage::BitVector(this->_transitionMatrix.getRowGroupCount(), false)));




                        for (storm::storage::sparse::state_type automatonFrom = 0; automatonFrom < da.getNumberOfStates(); ++automatonFrom) {
                            for (storm::storage::sparse::state_type automatonTo = 0; automatonTo < da.getNumberOfStates(); ++automatonTo) {
                                // Find the modelStates that trigger this transition.
                                for (storm::storage::sparse::state_type modelState = 0; modelState < this->_transitionMatrix.getRowGroupCount(); ++modelState) {

                                    if(!product->isValidProductState(modelState, automatonTo)) {
                                        // <modelState, automatonTo> is not defined in the Product. Thus, considered not reachable for the scheduler.
                                        for (uint_fast64_t infSet = 0; infSet < _infSets.get().size(); ++infSet) {
                                            this->_unreachableStates.get()[ (automatonTo* (_infSets.get().size()+1)) + infSet].set(modelState);
                                        }

                                    } else if (automatonTo == productBuilder.getSuccessor(modelState, automatonFrom, modelState)) { //TODO remove first parameter of getSuccessor

                                        // Add the modelState to one outgoing transition of all states of the form <automatonFrom, InfSet> (Inf=lenInfSet equals not in MEC)
                                        // For non-accepting states that are not in any accepting EC we use the 'last' copy of the DA
                                        // and for the accepting states we jump through copies of the DA wrt. the infinity sets.
                                        for (uint_fast64_t infSet = 0; infSet < _infSets.get().size()+1; ++infSet) {
                                            // Check if we need to switch the acceptance condition
                                            if (_accInfSets.get()[product->getProductStateIndex(modelState, automatonTo)].get().count(infSet) == 0) {
                                                // the state is is in a different accepting MEC with a different accepting conjunction of InfSets.
                                                auto newInfSet = _accInfSets.get()[product->getProductStateIndex(modelState, automatonTo)].get().begin();
                                                _memoryTransitions.get()[(automatonFrom *  (_infSets.get().size()+1)) + infSet][(automatonTo *  (_infSets.get().size()+1)) + *newInfSet].set(modelState);

                                                // TODO problem other inf set combis may not be reachable?
                                                //  this->_reachableStates.get()[(automatonTo *  (_infSets.get().size()+1)) + *newInfSet].set(modelState, true);

                                            } else {
                                                // The state is either not in an accepting EC or in an accepting EC that needs to satisfy the infSet.
                                                if (infSet == _infSets.get().size() || !(_infSets.get()[infSet].get(product->getProductStateIndex(modelState, automatonTo)))) {
                                                    // <modelState, automatonTo> is not in any accepting EC or does not satisfy the InfSet, we stay there.
                                                    // Add modelState to the transition from <automatonFrom, InfSet> to <automatonTo, InfSet>
                                                    _memoryTransitions.get()[(automatonFrom * (_infSets.get().size()+1)) + infSet][(automatonTo * (_infSets.get().size()+1)) + infSet].set(modelState);

                                                    // TODO problem other inf set combis may not be reachable?
                                                    //this->_reachableStates.get()[(automatonTo *  (_infSets.get().size()+1)) + infSet].set(modelState, true);

                                                } else {
                                                    STORM_LOG_ASSERT(_accInfSets.get()[product->getProductStateIndex(modelState, automatonTo)] != boost::none, "The list of InfSets for the product state <" <<modelState<< ", " << automatonTo<<"> is undefined.");
                                                    //  <modelState, automatonTo> satisfies the InfSet, find the next one
                                                    auto nextInfSet = std::find(_accInfSets.get()[product->getProductStateIndex(modelState, automatonTo)].get().begin(), _accInfSets.get()[product->getProductStateIndex(modelState, automatonTo)].get().end(), infSet);
                                                    STORM_LOG_ASSERT(nextInfSet != _accInfSets.get()[product->getProductStateIndex(modelState, automatonTo)].get().end(), "The list of InfSets for the product state <" <<modelState<< ", " << automatonTo<<"> does not contain the infSet " << infSet);
                                                    nextInfSet++;
                                                    if (nextInfSet == _accInfSets.get()[product->getProductStateIndex(modelState, automatonTo)].get().end()) {
                                                        // Start again.
                                                        nextInfSet = _accInfSets.get()[product->getProductStateIndex(modelState, automatonTo)].get().begin();
                                                    }
                                                    // Add modelState to the transition from <automatonFrom <mec, InfSet>> to  <automatonTo, <mec, NextInfSet>>.
                                                    _memoryTransitions.get()[(automatonFrom *  (_infSets.get().size()+1)) + infSet][(automatonTo *  (_infSets.get().size()+1)) + *nextInfSet].set(modelState);

                                                    // TODO problem other inf set combis may not be reachable?
                                                    // this->_reachableStates.get()[(automatonTo *  (_infSets.get().size()+1)) + *nextInfSet].set(modelState, true);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // Finished creation of transitions.

                        // Find initial memory states
                        this->_memoryInitialStates.emplace();
                        this->_memoryInitialStates->resize(this->_transitionMatrix.getRowGroupCount());
                        // Save for each relevant model state its initial memory state (get the s-successor q of q0)
                        for (storm::storage::sparse::state_type modelState : statesOfInterest) {
                            storm::storage::sparse::state_type automatonState = productBuilder.getInitialState(modelState);
                            STORM_LOG_ASSERT(product->isValidProductState(modelState, automatonState), "The memory successor state for the model state "<< modelState << "does not exist in the DA-Model Product.");
                            if (acceptingStates[product->getProductStateIndex(modelState, automatonState)]) {
                                STORM_LOG_ASSERT(_accInfSets.get()[product->getProductStateIndex(modelState, automatonState)] != boost::none, "The list of InfSets for the product state <" <<modelState<< ", " << automatonState<<"> is undefined.");
                                // If <s, q> is an accepting state start in the first InfSet of <s, q>.
                                auto infSet = _accInfSets.get()[product->getProductStateIndex(modelState, automatonState)].get().begin();
                                _memoryInitialStates.get()[modelState] = (automatonState * (_infSets.get().size()+1)) + *infSet;

                            } else {
                                _memoryInitialStates.get()[modelState] = (automatonState * (_infSets.get().size()+1)) + _infSets.get().size();
                            }

                        }
                        // Finished creation of initial states.
                    }

                } else {
                    prodNumericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeUntilProbabilities(env,
                                                                                                                                 std::move(solveGoalProduct),
                                                                                                                                 product->getProductModel().getTransitionMatrix(),
                                                                                                                                 product->getProductModel().getBackwardTransitions(),
                                                                                                                                 bvTrue,
                                                                                                                                 acceptingStates,
                                                                                                                                 this->isQualitativeSet());
                    }

                std::vector<ValueType> numericResult = product->projectToOriginalModel(this->_numberOfStates, prodNumericResult);

                return numericResult;
            }


            template<typename ValueType, bool Nondeterministic>
            std::vector <ValueType> SparseLTLHelper<ValueType, Nondeterministic>::computeLTLProbabilities(Environment const& env, storm::logic::Formula const& formula, std::map<std::string, storm::storage::BitVector>& apSatSets) {
                std::shared_ptr<storm::logic::Formula const> ltlFormula;
                STORM_LOG_THROW((!Nondeterministic) || this->isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
                if (Nondeterministic && this->getOptimizationDirection() == OptimizationDirection::Minimize) {
                    // negate formula in order to compute 1-Pmax[!formula]
                    ltlFormula = std::make_shared<storm::logic::UnaryBooleanPathFormula>(storm::logic::UnaryBooleanOperatorType::Not, formula.asSharedPointer());
                    STORM_LOG_INFO("Computing Pmin, proceeding with negated LTL formula.");
                } else {
                    ltlFormula = formula.asSharedPointer();
                }

                STORM_LOG_INFO("Resulting LTL path formula: " << ltlFormula->toString());
                STORM_LOG_INFO(" in prefix format: " << ltlFormula->toPrefixString());

                // Convert LTL formula to a deterministic automaton
                std::shared_ptr<storm::automata::DeterministicAutomaton> da;
                if (env.modelchecker().isLtl2daSet()) {
                    // Use the external tool given via ltl2da
                    std::string ltl2da = env.modelchecker().getLtl2da().get();
                    da = storm::automata::LTL2DeterministicAutomaton::ltl2daExternalTool(*ltlFormula, ltl2da);
                }
                else {
                    // Use the internal tool (Spot)
                    // For nondeterministic models the acceptance condition is transformed into DNF
                    da = storm::automata::LTL2DeterministicAutomaton::ltl2daSpot(*ltlFormula, Nondeterministic);
                }

                STORM_LOG_DEBUG("Deterministic automaton for LTL formula has "
                                       << da->getNumberOfStates() << " states, "
                                       << da->getAPSet().size() << " atomic propositions and "
                                       << *da->getAcceptance()->getAcceptanceExpression() << " as acceptance condition." << std::endl);


                std::vector<ValueType> numericResult = computeDAProductProbabilities(env, *da, apSatSets);

                if(Nondeterministic && this->getOptimizationDirection()==OptimizationDirection::Minimize) {
                    // compute 1-Pmax[!fomula]
                    for (auto& value : numericResult) {
                        value = storm::utility::one<ValueType>() - value;
                    }
                }

                return numericResult;
            }


            template class SparseLTLHelper<double, false>;
            template class SparseLTLHelper<double, true>;

#ifdef STORM_HAVE_CARL
            template class SparseLTLHelper<storm::RationalNumber, false>;
            template class SparseLTLHelper<storm::RationalNumber, true>;
            template class SparseLTLHelper<storm::RationalFunction, false>;

#endif

        }
    }
}