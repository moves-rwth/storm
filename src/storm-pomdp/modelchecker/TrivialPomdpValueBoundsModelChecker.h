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
    
            template<typename ValueType>
            struct TrivialPomdpValueBounds {
                std::vector<std::vector<ValueType>> lower;
                std::vector<std::vector<ValueType>> upper;
                ValueType getHighestLowerBound(uint64_t const& state) {
                    STORM_LOG_ASSERT(!lower.empty(), "requested a lower bound but none were available");
                    auto it = lower.begin();
                    ValueType result = (*it)[state];
                    for (++it; it != lower.end(); ++it) {
                        result = std::max(result, (*it)[state]);
                    }
                    return result;
                }
                ValueType getSmallestUpperBound(uint64_t const& state) {
                    STORM_LOG_ASSERT(!upper.empty(), "requested an upper bound but none were available");
                    auto it = upper.begin();
                    ValueType result = (*it)[state];
                    for (++it; it != upper.end(); ++it) {
                        result = std::min(result, (*it)[state]);
                    }
                    return result;
                }
            };
            
            template <typename PomdpType>
            class TrivialPomdpValueBoundsModelChecker {
            public:
                typedef typename PomdpType::ValueType ValueType;
                typedef TrivialPomdpValueBounds<ValueType> ValueBounds;
                TrivialPomdpValueBoundsModelChecker(PomdpType const& pomdp) : pomdp(pomdp) {
                    // Intentionally left empty
                }
                
                ValueBounds getValueBounds(storm::logic::Formula const& formula) {
                    return getValueBounds(formula, storm::pomdp::analysis::getFormulaInformation(pomdp, formula));
                }
                
                std::vector<ValueType> getChoiceValues(std::vector<ValueType> const& stateValues, std::vector<ValueType>* actionBasedRewards) {
                    std::vector<ValueType> choiceValues((pomdp.getNumberOfChoices()));
                    pomdp.getTransitionMatrix().multiplyWithVector(stateValues, choiceValues, actionBasedRewards);
                    return choiceValues;
                }
                
                std::vector<ValueType> computeValuesForGuessedScheduler(std::vector<ValueType> const& stateValues, std::vector<ValueType>* actionBasedRewards, storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info, std::shared_ptr<storm::models::sparse::Mdp<ValueType>> underlyingMdp, ValueType const& scoreThreshold, bool relativeScore) {
                    // Create some positional scheduler for the POMDP
                    storm::storage::Scheduler<ValueType> pomdpScheduler(pomdp.getNumberOfStates());
                    // For each state, we heuristically find a good distribution over output actions.
                    auto choiceValues = getChoiceValues(stateValues, actionBasedRewards);
                    auto const& choiceIndices = pomdp.getTransitionMatrix().getRowGroupIndices();
                    std::vector<storm::storage::Distribution<ValueType, uint_fast64_t>> choiceDistributions(pomdp.getNrObservations());
                    for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                        auto& choiceDistribution = choiceDistributions[pomdp.getObservation(state)];
                        ValueType const& stateValue = stateValues[state];
                        assert(stateValue >= storm::utility::zero<ValueType>());
                        for (auto choice = choiceIndices[state]; choice < choiceIndices[state + 1]; ++choice) {
                            ValueType const& choiceValue = choiceValues[choice];
                            assert(choiceValue >= storm::utility::zero<ValueType>());
                            // Rate this choice by considering the relative difference between the choice value and the (optimal) state value
                            // A high score shall mean that the choice is "good"
                            if (storm::utility::isInfinity(stateValue)) {
                                // For infinity states, we simply distribute uniformly.
                                // FIXME: This case could be handled a bit more sensible
                                choiceDistribution.addProbability(choice - choiceIndices[state], scoreThreshold);
                            } else {
                                ValueType choiceScore = info.minimize() ? (choiceValue - stateValue) : (stateValue - choiceValue);
                                if (relativeScore) {
                                    ValueType avg = (stateValue + choiceValue) / storm::utility::convertNumber<ValueType, uint64_t>(2);
                                    if (!storm::utility::isZero(avg)) {
                                            choiceScore /= avg;
                                    }
                                }
                                choiceScore = storm::utility::one<ValueType>() - choiceScore;
                                if (choiceScore >= scoreThreshold) {
                                    choiceDistribution.addProbability(choice - choiceIndices[state], choiceScore);
                                }
                            }

                        }
                        STORM_LOG_ASSERT(choiceDistribution.size() > 0, "Empty choice distribution.");
                    }
                    // Normalize all distributions
                    for (auto& choiceDistribution : choiceDistributions) {
                        choiceDistribution.normalize();
                    }
                    // Set the scheduler for all states
                    for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                        pomdpScheduler.setChoice(choiceDistributions[pomdp.getObservation(state)], state);
                    }
                    STORM_LOG_ASSERT(!pomdpScheduler.isPartialScheduler(), "Expected a fully defined scheduler.");
                    auto scheduledModel = underlyingMdp->applyScheduler(pomdpScheduler, false);
                    
                    auto resultPtr = storm::api::verifyWithSparseEngine<ValueType>(scheduledModel, storm::api::createTask<ValueType>(formula.asSharedPointer(), false));
                    STORM_LOG_THROW(resultPtr, storm::exceptions::UnexpectedException, "No check result obtained.");
                    STORM_LOG_THROW(resultPtr->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected Check result Type");
                    std::vector<ValueType> pomdpSchedulerResult = std::move(resultPtr->template asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                    return pomdpSchedulerResult;
                }
                
                ValueBounds getValueBounds(storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info) {
                    STORM_LOG_THROW(info.isNonNestedReachabilityProbability() || info.isNonNestedExpectedRewardFormula(), storm::exceptions::NotSupportedException, "The property type is not supported for this analysis.");
                    
                    // Compute the values on the fully observable MDP
                    // We need an actual MDP so that we can apply schedulers below.
                    // Also, the api call in the next line will require a copy anyway.
                    auto underlyingMdp = std::make_shared<storm::models::sparse::Mdp<ValueType>>(pomdp.getTransitionMatrix(), pomdp.getStateLabeling(), pomdp.getRewardModels());
                    auto resultPtr = storm::api::verifyWithSparseEngine<ValueType>(underlyingMdp, storm::api::createTask<ValueType>(formula.asSharedPointer(), false));
                    STORM_LOG_THROW(resultPtr, storm::exceptions::UnexpectedException, "No check result obtained.");
                    STORM_LOG_THROW(resultPtr->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected Check result Type");
                    std::vector<ValueType> fullyObservableResult = std::move(resultPtr->template asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                    
                    std::vector<ValueType> actionBasedRewards;
                    std::vector<ValueType>* actionBasedRewardsPtr = nullptr;
                    if (info.isNonNestedExpectedRewardFormula()) {
                        actionBasedRewards = pomdp.getRewardModel(info.getRewardModelName()).getTotalRewardVector(pomdp.getTransitionMatrix());
                        actionBasedRewardsPtr = &actionBasedRewards;
                    }
                    std::vector<std::vector<ValueType>> guessedSchedulerValues;
                    
                    std::vector<std::pair<double, bool>> guessParameters({{0.875,false},{0.875,true},{0.75,false},{0.75,true}});
                    for (auto const& pars : guessParameters) {
                        guessedSchedulerValues.push_back(computeValuesForGuessedScheduler(fullyObservableResult, actionBasedRewardsPtr, formula, info, underlyingMdp, storm::utility::convertNumber<ValueType>(pars.first), pars.second));
                    }
                    
                    // compute the 'best' guess and do a few iterations on it
                    uint64_t bestGuess = 0;
                    ValueType bestGuessSum = std::accumulate(guessedSchedulerValues.front().begin(), guessedSchedulerValues.front().end(), storm::utility::zero<ValueType>());
                    for (uint64_t guess = 1; guess < guessedSchedulerValues.size(); ++guess) {
                        ValueType guessSum = std::accumulate(guessedSchedulerValues[guess].begin(), guessedSchedulerValues[guess].end(), storm::utility::zero<ValueType>());
                        if ((info.minimize() && guessSum < bestGuessSum) || (info.maximize() && guessSum > bestGuessSum)) {
                            bestGuess = guess;
                            bestGuessSum = guessSum;
                        }
                    }
                    guessedSchedulerValues.push_back(computeValuesForGuessedScheduler(guessedSchedulerValues[bestGuess], actionBasedRewardsPtr, formula, info, underlyingMdp, storm::utility::convertNumber<ValueType>(guessParameters[bestGuess].first), guessParameters[bestGuess].second));
                    guessedSchedulerValues.push_back(computeValuesForGuessedScheduler(guessedSchedulerValues.back(), actionBasedRewardsPtr, formula, info, underlyingMdp, storm::utility::convertNumber<ValueType>(guessParameters[bestGuess].first), guessParameters[bestGuess].second));
                    guessedSchedulerValues.push_back(computeValuesForGuessedScheduler(guessedSchedulerValues.back(), actionBasedRewardsPtr, formula, info, underlyingMdp, storm::utility::convertNumber<ValueType>(guessParameters[bestGuess].first), guessParameters[bestGuess].second));

                    // Check if one of the guesses is worse than one of the others (and potentially delete it)
                    // Avoid deleting entries during the loop to ensure that indices remain valid
                    storm::storage::BitVector keptGuesses(guessedSchedulerValues.size(), true);
                    for (uint64_t i = 0; i < guessedSchedulerValues.size() - 1; ++i) {
                        if (!keptGuesses.get(i)) {
                            continue;
                        }
                        for (uint64_t j = i + 1; j < guessedSchedulerValues.size(); ++j) {
                            if (!keptGuesses.get(j)) {
                                continue;
                            }
                            if (storm::utility::vector::compareElementWise(guessedSchedulerValues[i], guessedSchedulerValues[j], std::less_equal<ValueType>())) {
                                if (info.minimize()) {
                                    // In this case we are guessing upper bounds (and smaller upper bounds are better)
                                    keptGuesses.set(j, false);
                                } else {
                                    // In this case we are guessing lower bounds (and larger lower bounds are better)
                                    keptGuesses.set(i, false);
                                    break;
                                }
                            } else if (storm::utility::vector::compareElementWise(guessedSchedulerValues[j], guessedSchedulerValues[i], std::less_equal<ValueType>())) {
                                if (info.minimize()) {
                                    keptGuesses.set(i, false);
                                    break;
                                } else {
                                    keptGuesses.set(j, false);
                                }
                            }
                        }
                    }
                    std::cout << "Keeping scheduler guesses " << keptGuesses << std::endl;
                    storm::utility::vector::filterVectorInPlace(guessedSchedulerValues, keptGuesses);
                    
                    // Finally prepare the result
                    ValueBounds result;
                    if (info.minimize()) {
                        result.lower.push_back(std::move(fullyObservableResult));
                        result.upper = std::move(guessedSchedulerValues);
                    } else {
                        result.lower = std::move(guessedSchedulerValues);
                        result.upper.push_back(std::move(fullyObservableResult));
                    }
                    STORM_LOG_WARN_COND_DEBUG(storm::utility::vector::compareElementWise(result.lower.front(), result.upper.front(), std::less_equal<ValueType>()), "Lower bound is larger than upper bound");
                    return result;
                }
    
            private:
                PomdpType const& pomdp;
            };
        }
    }
}