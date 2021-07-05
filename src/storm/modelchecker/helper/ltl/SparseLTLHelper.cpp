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
                STORM_LOG_ASSERT(this->_productChoices.is_initialized(), "Trying to extract the produced scheduler but none is available. Was there a computation call before?");
                STORM_LOG_ASSERT(this->_memoryTransitions.is_initialized(), "Trying to extract the DA transition structure but none is available. Was there a computation call before?");
                STORM_LOG_ASSERT(this->_memoryInitialStates.is_initialized(), "Trying to extract the initial states of the DA but there are none available. Was there a computation call before?");

                // Create a memory structure for the MDP scheduler with memory.
                typename storm::storage::MemoryStructureBuilder<ValueType>::MemoryStructureBuilder memoryBuilder(this->_memoryTransitions.get().size(), model);

                // Build the transitions between the memory states:  startState--- modelStates (transitionVector) --->goalState
                for (storm::storage::sparse::state_type startState = 0; startState < this->_memoryTransitions.get().size(); ++startState) {
                    for (storm::storage::sparse::state_type goalState = 0; goalState < this->_memoryTransitions.get().size(); ++goalState) {
                        // Bitvector that represents modelStates the model states that trigger this transition.
                        memoryBuilder.setTransition(startState, goalState, this->_memoryTransitions.get()[startState][goalState]);
                    }
                }

                /*
                memoryStateLabeling: Label the memory states to specify, e.g., accepting states // TODO where are these labels used?
                for (storm::storage::sparse::state_type q : set) {
                    memoryBuilder.setLabel(q,//todo);
                }
                */

                // initialMemoryStates: Assign an initial memory state to each initial state of the model.
                for (uint_fast64_t s0 : model.getInitialStates()) {
                    // Label the state as initial TODO unnecessary?
                    //memoryBuilder.setLabel(q, "Initial for model state " s0)
                    memoryBuilder.setInitialMemoryState(s0, this->_memoryInitialStates.get()[s0]);
                }


                // Finally, we can build the memoryStructure.
                storm::storage::MemoryStructure memoryStructure = memoryBuilder.build();

                // Create a scheduler (with memory of size DA) for the model from the scheduler of the MDP-DA-product model.
                storm::storage::Scheduler<ValueType> scheduler(this->_transitionMatrix.getRowGroupCount(), memoryStructure);

                // Use choices in the product model to create a choice based on model state and memory state
                for (const auto &choice : this->_productChoices.get()) {
                    uint_fast64_t modelState = choice.first.first;
                    uint_fast64_t memoryState = choice.first.second;
                    scheduler.setChoice(choice.second, modelState, memoryState);
                }

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
            storm::storage::BitVector SparseLTLHelper<ValueType, Nondeterministic>::computeAcceptingECs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
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

                            for (auto const& stateChoicePair : mec) {
                                // TODO extend scheduler by mec choices (which?)
                                //  need adversary which forces (to remain in mec and) to visit all states of the mec inf. often with prob. 1
                                acceptingStates.set(stateChoicePair.first);
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
                //storm::storage::BitVector acceptingStates = storm::storage::BitVector(product->getProductModel().getNumberOfStates());
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


                STORM_LOG_INFO("Building "+ (Nondeterministic ? std::string("MDP-DA") : std::string("DTMC-DA")) +"product with deterministic automaton, starting from " << statesOfInterest.getNumberOfSetBits() << " model states...");
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
                    acceptingStates = computeAcceptingECs(*product->getAcceptance(), product->getProductModel().getTransitionMatrix(), product->getProductModel().getBackwardTransitions());

                } else {
                    STORM_LOG_INFO("Computing BSCCs and checking for acceptance...");
                    acceptingStates = computeAcceptingBCCs(*product->getAcceptance(), product->getProductModel().getTransitionMatrix());

                }

                if (acceptingStates.empty()) {
                    STORM_LOG_INFO("No accepting states, skipping probability computation.");
                    std::vector<ValueType> numericResult(this->_numberOfStates, storm::utility::zero<ValueType>());
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

                    if (this->isProduceSchedulerSet()) {
                        // Extract the choices of the scheduler for the MDP-DA product: <s,q> -> choice
                        this->_productChoices.emplace();
                        for (storm::storage::sparse::state_type pState = 0;
                            pState < product->getProductModel().getNumberOfStates(); ++pState) {
                            this->_productChoices.get().insert({std::make_pair(product->getModelState(pState), product->getAutomatonState(pState)), prodCheckResult.scheduler->getChoice(pState)});
                        }


                        // Prepare the memory structure. For that, we need: transitions,  initialMemoryStates (and todo: memoryStateLabeling)

                        // The next move function of the memory, will be build based on the transitions of the DA.
                        this->_memoryTransitions.emplace(da.getNumberOfStates(), std::vector<storm::storage::BitVector>(da.getNumberOfStates()));

                        for (storm::storage::sparse::state_type startState = 0; startState < da.getNumberOfStates(); ++startState) {
                            for (storm::storage::sparse::state_type goalState = 0;
                                 goalState < da.getNumberOfStates(); ++goalState) {
                                // Bitvector that represents modelStates the model states that trigger this transition.
                                storm::storage::BitVector modelStates(this->_transitionMatrix.getRowGroupCount(), false);
                                for (storm::storage::sparse::state_type modelState = 0;
                                    modelState < this->_transitionMatrix.getRowGroupCount(); ++modelState) {
                                    if (goalState == productBuilder.getSuccessor(modelState, startState, modelState)) {
                                        modelStates.set(modelState);
                                    }
                                }
                                // Insert a transition: startState--{modelStates}-->goalState
                                this->_memoryTransitions.get()[startState][goalState] = std::move(modelStates);
                            }
                        }

                        this->_memoryInitialStates.emplace();
                        this->_memoryInitialStates->resize(this->_transitionMatrix.getRowGroupCount());
                        // Save for each automaton state for which model state it is initial.
                        for (storm::storage::sparse::state_type modelState = 0; modelState< this->_transitionMatrix.getRowGroupCount(); ++modelState) {
                            this->_memoryInitialStates.get().at(modelState) = productBuilder.getInitialState(modelState);
                        }

                        // TODO labels (e.g. the accepting set?) for the automaton states
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

                STORM_PRINT("Deterministic automaton for LTL formula has "
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