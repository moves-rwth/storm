#pragma once

#include "storm-pomdp/analysis/FormulaInformation.h"

#include "storm/api/verification.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/Scheduler.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {
            template <typename PomdpType>
            class TrivialPomdpValueBoundsModelChecker {
            public:
                typedef typename PomdpType::ValueType ValueType;
                TrivialPomdpValueBoundsModelChecker(PomdpType const& pomdp) : pomdp(pomdp) {
                    // Intentionally left empty
                }
                
                struct ValueBounds {
                    std::vector<ValueType> lower;
                    std::vector<ValueType> upper;
                };
                ValueBounds getValueBounds(storm::logic::Formula const& formula) {
                    return getValueBounds(formula, storm::pomdp::analysis::getFormulaInformation(pomdp, formula));
                }
                
                ValueBounds getValueBounds(storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info) {
                    STORM_LOG_THROW(info.isNonNestedReachabilityProbability() || info.isNonNestedExpectedRewardFormula(), storm::exceptions::NotSupportedException, "The property type is not supported for this analysis.");
                    // Compute the values on the fully observable MDP
                    // We need an actual MDP here so that the apply scheduler method below will work.
                    // Also, the api call in the next line will require a copy anyway.
                    auto underlyingMdp = std::make_shared<storm::models::sparse::Mdp<ValueType>>(pomdp.getTransitionMatrix(), pomdp.getStateLabeling(), pomdp.getRewardModels());
                    auto resultPtr = storm::api::verifyWithSparseEngine<ValueType>(underlyingMdp, storm::api::createTask<ValueType>(formula.asSharedPointer(), false));
                    STORM_LOG_THROW(resultPtr, storm::exceptions::UnexpectedException, "No check result obtained.");
                    STORM_LOG_THROW(resultPtr->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected Check result Type");
                    std::vector<ValueType> fullyObservableResult = std::move(resultPtr->template asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                    
                    // Create some positional scheduler for the POMDP
                    storm::storage::Scheduler<ValueType> pomdpScheduler(pomdp.getNumberOfStates());
                    // For each state, we heuristically find a good distribution over output actions.
                    std::vector<ValueType> fullyObservableChoiceValues(pomdp.getNumberOfChoices());
                    if (info.isNonNestedExpectedRewardFormula()) {
                        std::vector<ValueType> actionBasedRewards = pomdp.getRewardModel(info.getRewardModelName()).getTotalRewardVector(pomdp.getTransitionMatrix());
                        pomdp.getTransitionMatrix().multiplyWithVector(fullyObservableResult, fullyObservableChoiceValues, &actionBasedRewards);
                    } else {
                        pomdp.getTransitionMatrix().multiplyWithVector(fullyObservableResult, fullyObservableChoiceValues);
                    }
                    auto const& choiceIndices = pomdp.getTransitionMatrix().getRowGroupIndices();
                    for (uint32_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
                        auto obsStates = pomdp.getStatesWithObservation(obs);
                        storm::storage::Distribution<ValueType, uint_fast64_t> choiceDistribution;
                        for (auto const &state : obsStates) {
                            ValueType const& stateValue = fullyObservableResult[state];
                            assert(stateValue >= storm::utility::zero<ValueType>());
                            for (auto choice = choiceIndices[state]; choice < choiceIndices[state + 1]; ++choice) {
                                ValueType const& choiceValue = fullyObservableChoiceValues[choice];
                                assert(choiceValue >= storm::utility::zero<ValueType>());
                                // Rate this choice by considering the relative difference between the choice value and the (optimal) state value
                                ValueType choiceRating;
                                if (stateValue < choiceValue) {
                                    choiceRating = choiceValue - stateValue;
                                    if (!storm::utility::isZero(choiceValue)) {
                                        choiceRating /= choiceValue;
                                    }
                                } else {
                                    choiceRating = stateValue - choiceValue;
                                    if (!storm::utility::isZero(stateValue)) {
                                        choiceRating /= stateValue;
                                    }
                                }
                                assert(choiceRating <= storm::utility::one<ValueType>());
                                assert(choiceRating >= storm::utility::zero<ValueType>());
                                // choiceRating = 0 is a very good choice, choiceRating = 1 is a very bad choice
                                if (choiceRating <= storm::utility::convertNumber<ValueType>(0.5)) {
                                    choiceDistribution.addProbability(choice - choiceIndices[state], storm::utility::one<ValueType>() - choiceRating);
                                }
                            }
                        }
                        choiceDistribution.normalize();
                        for (auto const& state : obsStates) {
                            pomdpScheduler.setChoice(choiceDistribution, state);
                        }
                    }
                    auto scheduledModel = underlyingMdp->applyScheduler(pomdpScheduler, false);
                    
                    auto resultPtr2 = storm::api::verifyWithSparseEngine<ValueType>(scheduledModel, storm::api::createTask<ValueType>(formula.asSharedPointer(), false));
                    STORM_LOG_THROW(resultPtr2, storm::exceptions::UnexpectedException, "No check result obtained.");
                    STORM_LOG_THROW(resultPtr2->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected Check result Type");
                    std::vector<ValueType> pomdpSchedulerResult = std::move(resultPtr2->template asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                    
                    // Finally prepare the result
                    ValueBounds result;
                    if (info.minimize()) {
                        result.lower = std::move(fullyObservableResult);
                        result.upper = std::move(pomdpSchedulerResult);
                    } else {
                        result.lower = std::move(pomdpSchedulerResult);
                        result.upper = std::move(fullyObservableResult);
                    }
                    return result;
                }
    
            private:
                PomdpType const& pomdp;
            };
        }
    }
}