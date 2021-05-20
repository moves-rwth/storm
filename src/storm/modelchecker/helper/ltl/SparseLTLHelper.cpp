#include "SparseLTLHelper.h"

#include "storm/transformer/DAProductBuilder.h"
#include "storm/automata/LTL2DeterministicAutomaton.h"

#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"

#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {

            template <typename ValueType, bool Nondeterministic>
            SparseLTLHelper<ValueType, Nondeterministic>::SparseLTLHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::size_t numberOfStates) : _transitionMatrix(transitionMatrix), _numberOfStates(numberOfStates) {
                // Intentionally left empty.
            }


            // todo only for MDP and change name!
            template <typename ValueType, bool Nondeterministic>
            storm::storage::BitVector SparseLTLHelper<ValueType, Nondeterministic>::computeSurelyAcceptingPmaxStates(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
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
                                acceptingStates.set(stateChoicePair.first);
                            }
                        }

                    }
                }

                STORM_LOG_DEBUG("Accepting states: " << acceptingStates);
                STORM_LOG_INFO("Found " << acceptingStates.getNumberOfSetBits() << " states in " << accMECs << " accepting MECs (considered " << allMECs << " MECs).");

                return acceptingStates;
            }

            // todo only for dtmc and change name!
            template <typename ValueType, bool Nondeterministic>
            storm::storage::BitVector SparseLTLHelper<ValueType, Nondeterministic>::computeAcceptingComponentStates(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix) {
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
            std::vector<ValueType> SparseLTLHelper<ValueType, Nondeterministic>::computeDAProductProbabilities(Environment const& env, storm::automata::DeterministicAutomaton const& da, std::map<std::string, storm::storage::BitVector>& apSatSets, bool qualitative) {
                const storm::automata::APSet& apSet = da.getAPSet();

                std::vector<storm::storage::BitVector> apLabels;
                for (const std::string& ap : apSet.getAPs()) {
                    auto it = apSatSets.find(ap);
                    STORM_LOG_THROW(it != apSatSets.end(), storm::exceptions::InvalidOperationException, "Deterministic automaton has AP " << ap << ", does not appear in formula");

                    apLabels.push_back(std::move(it->second));
                }

                storm::storage::BitVector statesOfInterest;

                if (this->hasRelevantStates()) {
                    statesOfInterest = this->getRelevantStates();
                } else {
                    // product from all model states
                    statesOfInterest = storm::storage::BitVector(this->_numberOfStates, true);
                }


                // todo change text
                STORM_LOG_INFO("Building "+ (Nondeterministic ? std::string("MDP-DA") : std::string("DTMC-DA")) +"product with deterministic automaton, starting from " << statesOfInterest.getNumberOfSetBits() << " model states...");
                storm::transformer::DAProductBuilder productBuilder(da, apLabels);

                auto product = productBuilder.build<productModelType>(this->_transitionMatrix, statesOfInterest);

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

                // DTMC: BCC
                // MDP: computeSurelyAcceptingPmaxStates
                storm::storage::BitVector acceptingStates;
                if (Nondeterministic) {
                    STORM_LOG_INFO("Computing accepting end components...");
                    // todo compute accepting states, same as below
                    acceptingStates = computeSurelyAcceptingPmaxStates(*product->getAcceptance(), product->getProductModel().getTransitionMatrix(), product->getProductModel().getBackwardTransitions());


                } else {
                    STORM_LOG_INFO("Computing BSCCs and checking for acceptance...");
                    // todo compute accepting states, (no btm)
                    acceptingStates = computeAcceptingComponentStates(*product->getAcceptance(), product->getProductModel().getTransitionMatrix());

                }

                if (acceptingStates.empty()) {
                    STORM_LOG_INFO("No acceptingStates states, skipping probability computation.");
                    std::vector<ValueType> numericResult(this->_numberOfStates, storm::utility::zero<ValueType>());
                    return numericResult;
                }

                STORM_LOG_INFO("Computing probabilities for reaching acceptingStates components...");

                storm::storage::BitVector bvTrue(product->getProductModel().getNumberOfStates(), true);
                storm::storage::BitVector soiProduct(product->getStatesOfInterest());

                // Create goal for computeUntilProbabilities, compute maximizing probabilties for DA product with MDP
                storm::solver::SolveGoal<ValueType> solveGoalProduct;
                if (this->isValueThresholdSet()) {
                    solveGoalProduct = storm::solver::SolveGoal<ValueType>(OptimizationDirection::Maximize, this->getValueThresholdComparisonType(), this->getValueThresholdValue(), std::move(soiProduct));
                } else {
                    solveGoalProduct = storm::solver::SolveGoal<ValueType>(OptimizationDirection::Maximize);
                    solveGoalProduct.setRelevantValues(std::move(soiProduct));
                }

                std::vector<ValueType> prodNumericResult;

                if (Nondeterministic) {
                    prodNumericResult = std::move(storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(env,
                                                                                                                                          std::move(solveGoalProduct),
                                                                                                                                          product->getProductModel().getTransitionMatrix(),
                                                                                                                                          product->getProductModel().getBackwardTransitions(),
                                                                                                                                          bvTrue,
                                                                                                                                          acceptingStates,
                                                                                                                                          qualitative,
                                                                                                                                          false  // no schedulers (at the moment)
                                                                                                                                ).values);

                } else {
                    prodNumericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeUntilProbabilities(env,
                                                                                                                                 std::move(solveGoalProduct),
                                                                                                                                 product->getProductModel().getTransitionMatrix(),
                                                                                                                                 product->getProductModel().getBackwardTransitions(),
                                                                                                                                 bvTrue,
                                                                                                                                 acceptingStates,
                                                                                                                                 qualitative);
                    }

                std::vector<ValueType> numericResult = product->projectToOriginalModel(this->_numberOfStates, prodNumericResult);
                return numericResult;
            }


            //todo remove goal?
            template<typename ValueType, bool Nondeterministic>
            std::vector <ValueType> SparseLTLHelper<ValueType, Nondeterministic>::computeLTLProbabilities(Environment const &env, storm::logic::Formula const& ltlFormula, std::map<std::string, storm::storage::BitVector>& apSatSets) {
                // TODO optDir: helper.getOptimizationDirection for MDP
                // negate formula etc ~ap?

                STORM_LOG_INFO("Resulting LTL path formula: " << ltlFormula);
                STORM_LOG_INFO(" in prefix format: " << ltlFormula.toPrefixString());

                std::shared_ptr<storm::automata::DeterministicAutomaton> da = storm::automata::LTL2DeterministicAutomaton::ltl2da(ltlFormula);

                STORM_LOG_INFO("Deterministic automaton for LTL formula has "
                                       << da->getNumberOfStates() << " states, "
                                       << da->getAPSet().size() << " atomic propositions and "
                                       << *da->getAcceptance()->getAcceptanceExpression() << " as acceptance condition.");


                //todo remove goal here? send dir instead?
                std::vector<ValueType> numericResult = computeDAProductProbabilities(env, *da, apSatSets, this->isQualitativeSet());

                // TODO optDir: helper.getOptimizationDirection for MDP
                /* //for any path formula ψ: pmin(s, ψ) = 1- pmax(s, ¬ψ)
                if(Nondeterministic && this->getOptimizationDirection()==OptimizationDirection::Minimize) {
                    // compute 1-Pmax[!ltl]
                    for (auto& value : numericResult) {
                        value = storm::utility::one<ValueType>() - value;
                    }
                }
                */

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