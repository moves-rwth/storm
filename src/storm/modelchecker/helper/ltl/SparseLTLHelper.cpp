#include "SparseLTLHelper.h"

#include "storm/transformer/DAProductBuilder.h"
#include "storm/automata/LTL2DeterministicAutomaton.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"

#include "storm/storage/StronglyConnectedComponentDecomposition.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {

            template <typename ValueType, typename Model, bool Nondeterministic>
            SparseLTLHelper<ValueType, Model, Nondeterministic>::SparseLTLHelper(Model const& model, storm::storage::SparseMatrix<ValueType> const& transitionMatrix) : _model(model), _transitionMatrix(transitionMatrix) {
                // Intentionally left empty.
            }

            template<typename ValueType, typename Model, bool Nondeterministic>
            std::vector<ValueType> SparseLTLHelper<ValueType, Model, Nondeterministic>::computeDAProductProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::automata::DeterministicAutomaton const& da, std::map<std::string, storm::storage::BitVector>& apSatSets, bool qualitative) {
                STORM_LOG_THROW((!Nondeterministic) || goal.hasDirection() && goal.direction() == OptimizationDirection::Maximize, storm::exceptions::InvalidPropertyException, "Can only compute maximizing probabilties for DA product with MDP");


                const storm::automata::APSet& apSet = da.getAPSet();

                std::vector<storm::storage::BitVector> apLabels;
                for (const std::string& ap : apSet.getAPs()) {
                    auto it = apSatSets.find(ap);
                    STORM_LOG_THROW(it != apSatSets.end(), storm::exceptions::InvalidOperationException, "Deterministic automaton has AP " << ap << ", does not appear in formula");

                    apLabels.push_back(std::move(it->second));
                }

                storm::storage::BitVector statesOfInterest;
                if (goal.hasRelevantValues()) {
                    statesOfInterest = goal.relevantValues();
                } else {
                    // product from all model states
                    statesOfInterest = storm::storage::BitVector(this->_model.getNumberOfStates(), true);
                }


                STORM_LOG_INFO("Building "+ (Nondeterministic ? std::string("MDP-DA") : std::string("DTMC-DA")) +"product with deterministic automaton, starting from " << statesOfInterest.getNumberOfSetBits() << " model states...");
                storm::transformer::DAProductBuilder productBuilder(da, apLabels);

                auto product = productBuilder.build(this->_model, statesOfInterest);

                STORM_LOG_INFO("Product "+ (Nondeterministic ? std::string("MDP-DA") : std::string("DTMC")) +" has " << product->getProductModel().getNumberOfStates() << " states and "
                                                   << product->getProductModel().getNumberOfTransitions() << " transitions.");

                if (storm::settings::getModule<storm::settings::modules::DebugSettings>().isTraceSet()) {
                    STORM_LOG_TRACE("Writing model to model.dot");
                    std::ofstream modelDot("model.dot");
                    this->_model.writeDotToStream(modelDot);
                    modelDot.close();

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
                storm::storage::BitVector accepting;
                if (Nondeterministic) {
                    STORM_LOG_INFO("Computing accepting end components...");
                    accepting = storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeSurelyAcceptingPmaxStates(*product->getAcceptance(), product->getProductModel().getTransitionMatrix(), product->getProductModel().getBackwardTransitions());
                    if (accepting.empty()) {
                        STORM_LOG_INFO("No accepting states, skipping probability computation.");
                        std::vector<ValueType> numericResult(this->_model.getNumberOfStates(), storm::utility::zero<ValueType>());
                        return numericResult;
                    }

                } else {
                    STORM_LOG_INFO("Computing BSCCs and checking for acceptance...");

                    storm::storage::StronglyConnectedComponentDecomposition<ValueType> bottomSccs(product->getProductModel().getTransitionMatrix(),
                                                                                                  storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs().dropNaiveSccs());
                    accepting = storm::storage::BitVector(product->getProductModel().getNumberOfStates());
                    std::size_t checkedBSCCs = 0, acceptingBSCCs = 0, acceptingBSCCStates = 0;
                    for (auto& scc : bottomSccs) {
                        checkedBSCCs++;
                        if (product->getAcceptance()->isAccepting(scc)) {
                            acceptingBSCCs++;
                            for (auto& state : scc) {
                                accepting.set(state);
                                acceptingBSCCStates++;
                            }
                        }
                    }

                    STORM_LOG_INFO("BSCC analysis: " << acceptingBSCCs << " of " << checkedBSCCs << " BSCCs were accepting (" << acceptingBSCCStates << " states in accepting BSCCs).");

                    if (acceptingBSCCs == 0) {
                        STORM_LOG_INFO("No accepting BSCCs, skipping probability computation.");
                        std::vector<ValueType> numericResult(this->_model.getNumberOfStates(), storm::utility::zero<ValueType>());
                        return numericResult;
                    }
                }

                STORM_LOG_INFO("Computing probabilities for reaching accepting BSCCs...");

                storm::storage::BitVector bvTrue(product->getProductModel().getNumberOfStates(), true);

                storm::solver::SolveGoal<ValueType> solveGoalProduct(goal);
                storm::storage::BitVector soiProduct(product->getStatesOfInterest());
                solveGoalProduct.setRelevantValues(std::move(soiProduct));

                std::vector<ValueType> prodNumericResult;

                if (Nondeterministic) {
                    prodNumericResult
                            = std::move(storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(env,
                                                                                                                                std::move(solveGoalProduct),
                                                                                                                                product->getProductModel().getTransitionMatrix(),
                                                                                                                                product->getProductModel().getBackwardTransitions(),
                                                                                                                                bvTrue,
                                                                                                                                accepting,
                                                                                                                                qualitative,
                                                                                                                                false  // no schedulers (at the moment)
                                                                                                                                ).values);

                } else {
                    prodNumericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeUntilProbabilities(env,
                                                                                                                   std::move(solveGoalProduct),
                                                                                                                   product->getProductModel().getTransitionMatrix(),
                                                                                                                   product->getProductModel().getBackwardTransitions(),
                                                                                                                   bvTrue,
                                                                                                                   accepting,
                                                                                                                   qualitative);
                    }

                std::vector<ValueType> numericResult = product->projectToOriginalModel(this->_model, prodNumericResult);
                return numericResult;
            }


            template<typename ValueType, typename RewardModelType, bool Nondeterministic>
            std::vector <ValueType> SparseLTLHelper<ValueType, RewardModelType, Nondeterministic>::computeLTLProbabilities(Environment const &env, storm::solver::SolveGoal<ValueType>&& goal, storm::logic::Formula const& ltlFormula, std::map<std::string, storm::storage::BitVector>& apSatSets) {
                STORM_LOG_INFO("Resulting LTL path formula: " << ltlFormula);
                STORM_LOG_INFO(" in prefix format: " << ltlFormula.toPrefixString());

                std::shared_ptr<storm::automata::DeterministicAutomaton> da = storm::automata::LTL2DeterministicAutomaton::ltl2da(ltlFormula);

                STORM_LOG_INFO("Deterministic automaton for LTL formula has "
                                       << da->getNumberOfStates() << " states, "
                                       << da->getAPSet().size() << " atomic propositions and "
                                       << *da->getAcceptance()->getAcceptanceExpression() << " as acceptance condition.");


                std::vector<ValueType> numericResult = computeDAProductProbabilities(env, std::move(goal), *da, apSatSets, this->isQualitativeSet());

                /*
                if(Nondeterministic && this->getOptimizationDirection()==OptimizationDirection::Minimize) {
                    // compute 1-Pmax[!ltl]
                    for (auto& value : numericResult) {
                        value = storm::utility::one<ValueType>() - value;
                    }
                }
                */

                return numericResult;
            }

            template class SparseLTLHelper<double, storm::models::sparse::Dtmc<double>, false>;
            template class SparseLTLHelper<double, storm::models::sparse::Mdp<double>, true>;

#ifdef STORM_HAVE_CARL
            template class SparseLTLHelper<storm::RationalNumber, storm::models::sparse::Dtmc<storm::RationalNumber>, false>;
            template class SparseLTLHelper<storm::RationalNumber, storm::models::sparse::Mdp<storm::RationalNumber>, true>;
            template class SparseLTLHelper<storm::RationalFunction,  storm::models::sparse::Dtmc<storm::RationalFunction>, false>;

#endif

        }
    }
}