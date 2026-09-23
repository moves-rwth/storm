#include "PolicyExtractor.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/sparse/ModelComponents.h"

#include <charconv>
#include <string_view>
#include <unordered_set>

namespace storm::pomdp::policy {

template<typename PomdpModelType, typename BeliefValueType, typename BeliefMdpValueType, typename PolicyValueType>
PolicyExtractor<PomdpModelType, BeliefValueType, BeliefMdpValueType, PolicyValueType>::PolicyExtractor(
    PomdpModelType const& pomdp, BeliefMdpType const& beliefMdp, std::unordered_map<uint64_t, uint32_t> const& beliefStateToObservationMap,
    storm::storage::Scheduler<BeliefMdpValueType> const& beliefMdpScheduler,
    std::optional<std::vector<storm::storage::Scheduler<typename PomdpModelType::ValueType>>> const& pomdpApproximationSchedulers,
    PomdpModelType const* preprocessedPomdp)
    : pomdp(pomdp),
      beliefMdp(beliefMdp),
      beliefMdpScheduler(beliefMdpScheduler),
      beliefStateToObservationMap(beliefStateToObservationMap),
      pomdpApproximationSchedulers(pomdpApproximationSchedulers),
      preprocessedPomdp(preprocessedPomdp) {
    // Intentionally left empty
}

template<typename PomdpModelType, typename BeliefValueType, typename BeliefMdpValueType, typename PolicyValueType>
ObservationBasedFiniteStateController<PolicyValueType>
PolicyExtractor<PomdpModelType, BeliefValueType, BeliefMdpValueType, PolicyValueType>::exportPolicyAsFiniteStateController() const {
    std::optional<std::unordered_map<uint64_t, ObservationSignature>> optionalIdToObservationSignature = std::nullopt;
    std::optional<std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::string>>> optionalIdToActionNameMap = std::nullopt;
    if (pomdp.hasObservationValuations()) {
        auto const& obsValuations = pomdp.getObservationValuations();
        std::unordered_map<uint64_t, ObservationSignature> idToObservationSignatureMap;
        std::vector<ObservationSignature> usedObservationSignatures;
        bool observationSignaturesAreUnique = true;
        for (uint64_t i = 0; i < pomdp.getNrObservations(); ++i) {
            auto observationSignature = ObservationSignature::fromValuations(obsValuations, i);
            if (observationSignature.values.empty() || std::ranges::find(usedObservationSignatures, observationSignature) != usedObservationSignatures.end()) {
                observationSignaturesAreUnique = false;
                break;
            }
            usedObservationSignatures.push_back(observationSignature);
            idToObservationSignatureMap[i] = std::move(observationSignature);
        }
        if (observationSignaturesAreUnique) {
            optionalIdToObservationSignature = std::move(idToObservationSignatureMap);
        }
    }
    if (pomdp.hasChoiceLabeling()) {
        auto const& choiceLabeling = pomdp.getChoiceLabeling();
        std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::string>> idToActionNameMap;
        for (uint64_t obs = 0; obs < pomdp.getNrObservations(); ++obs) {
            if (pomdp.getStatesWithObservation(obs).empty()) {
                continue;
            }
            uint64_t representativeState = *pomdp.getStatesWithObservation(obs).begin();
            auto const rowGroupIndex = pomdp.getTransitionMatrix().getRowGroupIndices()[representativeState];
            auto const rowGroupSize = pomdp.getTransitionMatrix().getRowGroupSize(representativeState);
            for (uint64_t localChoice = 0; localChoice < rowGroupSize; ++localChoice) {
                auto const globalChoiceIndex = rowGroupIndex + localChoice;
                STORM_LOG_ASSERT(choiceLabeling.getLabelsOfChoice(globalChoiceIndex).size() <= 1,
                                 "Expected choice labeling to have at most one label per choice, but choice ID "
                                     << globalChoiceIndex << " has " << choiceLabeling.getLabelsOfChoice(globalChoiceIndex).size() << " labels.");
                if (choiceLabeling.getLabelsOfChoice(globalChoiceIndex).empty()) {
                    idToActionNameMap[obs][localChoice] = "";
                } else {
                    idToActionNameMap[obs][localChoice] = *choiceLabeling.getLabelsOfChoice(globalChoiceIndex).begin();
                }
            }
        }
        optionalIdToActionNameMap = idToActionNameMap;
    }
    ObservationBasedFiniteStateController<PolicyValueType> fsc(0ul, optionalIdToObservationSignature, optionalIdToActionNameMap);

    std::unordered_map<uint64_t, uint64_t> beliefStateToFscNodeMap;
    std::unordered_map<uint64_t, uint64_t> cutoffPolicyToFscNodeMap;
    std::queue<uint64_t> statesToProcess;
    // Add initial transition
    auto const initialBeliefState = beliefMdp.getInitialStates().getNextSetIndex(0);
    auto const initialChoice = beliefMdpScheduler.getChoice(initialBeliefState).getDeterministicChoice();
    fsc.addDeterministicActionTransition(0ul, beliefStateToObservationMap.at(initialBeliefState), initialChoice, 1ul);
    beliefStateToFscNodeMap[initialBeliefState] = 1ul;
    statesToProcess.push(initialBeliefState);
    uint64_t nextId = 2ul;

    while (!statesToProcess.empty()) {
        uint64_t currentState = statesToProcess.front();
        statesToProcess.pop();
        auto const currentChoice = beliefMdpScheduler.getChoice(currentState).getDeterministicChoice();
        for (auto const& entry : beliefMdp.getTransitionMatrix().getRow(currentState, currentChoice)) {
            auto successorBeliefStateId = entry.getColumn();
            if (beliefMdp.getStateLabeling().getStateHasLabel("truncated", successorBeliefStateId)) {
                // State has been cut off. Add transition to self-looping cut-off policy node.
                auto const localCutoffChoice = beliefMdpScheduler.getChoice(successorBeliefStateId).getDeterministicChoice();
                auto const globalCutoffChoice = beliefMdp.getTransitionMatrix().getRowGroupIndices()[successorBeliefStateId] + localCutoffChoice;
                auto const& cutoffChoiceLabels = beliefMdp.getChoiceLabeling().getLabelsOfChoice(globalCutoffChoice);
                STORM_LOG_THROW(cutoffChoiceLabels.size() == 1, storm::exceptions::UnexpectedException,
                                "Expected the selected cutoff choice to have exactly one scheduler label.");
                std::string const& cutoffChoiceLabel = *cutoffChoiceLabels.begin();
                constexpr std::string_view schedulerLabelPrefix = "__sched_";
                STORM_LOG_THROW(cutoffChoiceLabel.starts_with(schedulerLabelPrefix), storm::exceptions::UnexpectedException,
                                "Expected cutoff scheduler label, but found '" << cutoffChoiceLabel << "'.");
                uint64_t chosenCutoffPolicy = 0;
                auto const schedulerIndex = std::string_view(cutoffChoiceLabel).substr(schedulerLabelPrefix.size());
                auto const [parseEnd, parseError] = std::from_chars(schedulerIndex.begin(), schedulerIndex.end(), chosenCutoffPolicy);
                STORM_LOG_THROW(parseError == std::errc() && parseEnd == schedulerIndex.end(), storm::exceptions::UnexpectedException,
                                "Could not parse cutoff scheduler label '" << cutoffChoiceLabel << "'.");
                STORM_LOG_THROW(pomdpApproximationSchedulers.has_value() && chosenCutoffPolicy < pomdpApproximationSchedulers->size(),
                                storm::exceptions::UnexpectedException,
                                "Cutoff scheduler label '" << cutoffChoiceLabel << "' does not refer to an available approximation scheduler.");
                if (!cutoffPolicyToFscNodeMap.contains(chosenCutoffPolicy)) {
                    // TODO extend for fm-policies
                    cutoffPolicyToFscNodeMap[chosenCutoffPolicy] = nextId;
                    ++nextId;
                }
                auto dist = pomdpSchedulerToObservationBasedMap(
                    pomdpApproximationSchedulers->at(chosenCutoffPolicy))[beliefStateToObservationMap.at(successorBeliefStateId)];
                if (dist.size() == 1) {
                    fsc.addDeterministicActionTransition(beliefStateToFscNodeMap[currentState], beliefStateToObservationMap.at(successorBeliefStateId),
                                                         dist.begin()->first, cutoffPolicyToFscNodeMap[chosenCutoffPolicy]);
                } else {
                    fsc.addRandomisedActionTransition(beliefStateToFscNodeMap[currentState], beliefStateToObservationMap.at(successorBeliefStateId), dist,
                                                      cutoffPolicyToFscNodeMap[chosenCutoffPolicy]);
                }
            } else {
                if (!beliefStateToFscNodeMap.contains(successorBeliefStateId)) {
                    beliefStateToFscNodeMap[successorBeliefStateId] = nextId;
                    ++nextId;
                    statesToProcess.push(successorBeliefStateId);
                }
                if (beliefStateToObservationMap.contains(successorBeliefStateId)) {
                    auto const successorChoice = beliefMdpScheduler.getChoice(successorBeliefStateId).getDeterministicChoice();
                    fsc.addDeterministicActionTransition(beliefStateToFscNodeMap[currentState], beliefStateToObservationMap.at(successorBeliefStateId),
                                                         successorChoice, beliefStateToFscNodeMap[successorBeliefStateId]);
                }  // else, the state does not actually correspond to a belief, and we don't care what to do afterwards
            }
        }
    }

    if (pomdpApproximationSchedulers.has_value()) {
        for (uint64_t i = 0; i < pomdpApproximationSchedulers->size(); ++i) {
            // TODO extend for fm-policies
            if (cutoffPolicyToFscNodeMap.contains(i)) {
                for (auto observationToActionMap = pomdpSchedulerToObservationBasedMap(pomdpApproximationSchedulers->at(i));
                     auto const& [observationId, actionDist] : observationToActionMap) {
                    if (actionDist.size() == 1) {
                        fsc.addDeterministicActionTransition(cutoffPolicyToFscNodeMap[i], observationId, actionDist.begin()->first,
                                                             cutoffPolicyToFscNodeMap[i]);
                    } else {
                        fsc.addRandomisedActionTransition(cutoffPolicyToFscNodeMap[i], observationId, actionDist, cutoffPolicyToFscNodeMap[i]);
                    }
                }
            }
        }
    }
    return fsc;
}

template<typename PomdpModelType, typename BeliefValueType, typename BeliefMdpValueType, typename PolicyValueType>
std::shared_ptr<storm::models::sparse::Model<BeliefMdpValueType>>
PolicyExtractor<PomdpModelType, BeliefValueType, BeliefMdpValueType, PolicyValueType>::exportPolicyAsInducedMarkovChain() const {
    storm::models::sparse::StateLabeling newLabeling(beliefMdp.getStateLabeling());
    if (pomdpApproximationSchedulers.has_value()) {
        for (uint64_t i = 0; i < pomdpApproximationSchedulers->size(); ++i) {
            newLabeling.addLabel("sched_" + std::to_string(i));
        }
    }
    for (uint64_t i = 0; i < pomdp.getNrObservations(); ++i) {
        newLabeling.addLabel("obs_" + std::to_string(i));
    }
    newLabeling.addLabel("cutoff");
    newLabeling.addLabel("clipping");
    newLabeling.addLabel("finite_mem");

    storm::storage::SparseMatrix<BeliefMdpValueType> transMatrix = beliefMdp.getTransitionMatrix();
    for (uint64_t i = 0; i < beliefMdp.getNumberOfStates(); ++i) {
        if (beliefStateToObservationMap.contains(i)) {
            newLabeling.addLabelToState("obs_" + std::to_string(beliefStateToObservationMap.at(i)), i);
        }
        if (newLabeling.getStateHasLabel("truncated", i)) {
            uint64_t localChosenActionIndex = beliefMdpScheduler.getChoice(i).getDeterministicChoice();
            auto rowIndex = beliefMdp.getTransitionMatrix().getRowGroupIndices()[i];
            if (beliefMdp.getChoiceLabeling().getLabelsOfChoice(rowIndex + localChosenActionIndex).size() > 0) {
                if (auto label = *(beliefMdp.getChoiceLabeling().getLabelsOfChoice(rowIndex + localChosenActionIndex).begin()); label.rfind("clip", 0) == 0) {
                    newLabeling.addLabelToState("clipping", i);
                    auto chosenRow = transMatrix.getRow(i, 0);
                    auto candidateIndex = (chosenRow.end() - 1)->getColumn();
                    transMatrix.makeRowDirac(transMatrix.getRowGroupIndices()[i], candidateIndex);
                } else if (label.rfind("__mem_node", 0) == 0) {
                    newLabeling.addLabelToState("finite_mem", i);
                    newLabeling.addLabelToState("cutoff", i);
                } else if (pomdpApproximationSchedulers.has_value()) {
                    // Label has form "__sched_X"
                    newLabeling.addLabelToState(label.substr(2), i);
                    newLabeling.addLabelToState("cutoff", i);
                }
            }
        }
    }
    newLabeling.removeLabel("truncated");

    transMatrix.dropZeroEntries();
    storm::storage::sparse::ModelComponents<BeliefMdpValueType> modelComponents(transMatrix, newLabeling);
    if (beliefMdp.hasChoiceLabeling()) {
        // Add empty string as label of all choices without labels
        storm::models::sparse::ChoiceLabeling extendedChoiceLabeling(beliefMdp.getChoiceLabeling());
        extendedChoiceLabeling.addLabel("");
        for (uint64_t i = 0; i < beliefMdp.getNumberOfChoices(); ++i) {
            if (beliefMdp.getChoiceLabeling().getLabelsOfChoice(i).empty()) {
                extendedChoiceLabeling.addLabelToChoice("", i);
            }
        }
        modelComponents.choiceLabeling = extendedChoiceLabeling;
    }
    storm::models::sparse::Mdp<BeliefMdpValueType> newMDP(modelComponents);
    auto inducedMC = newMDP.applyScheduler(beliefMdpScheduler, true);
    return std::static_pointer_cast<storm::models::sparse::Model<BeliefMdpValueType>>(inducedMC);
}

template<typename PomdpModelType, typename BeliefValueType, typename BeliefMdpValueType, typename PolicyValueType>
std::unordered_map<uint64_t, storm::storage::Distribution<PolicyValueType, uint64_t>>
PolicyExtractor<PomdpModelType, BeliefValueType, BeliefMdpValueType, PolicyValueType>::pomdpSchedulerToObservationBasedMap(
    storm::storage::Scheduler<typename PomdpModelType::ValueType> const& scheduler) const {
    std::unordered_map<uint64_t, storm::storage::Distribution<PolicyValueType, uint64_t>> result;

    // Iterate all POMDP states and group by observation id.
    const uint64_t numStates = pomdp.getNumberOfStates();
    for (uint64_t state = 0; state < numStates; ++state) {
        // Get the observation id for this state.
        uint64_t obs = pomdp.getObservation(state);

        // If we've already built the distribution for this observation, skip.
        if (result.contains(obs)) {
            continue;
        }

        // Build distribution from the scheduler choice for this state.
        auto choice = scheduler.getChoice(state).template toValueType<PolicyValueType>();
        storm::storage::Distribution<PolicyValueType, uint64_t> dist = choice.getChoiceAsDistribution();

        result.emplace(obs, std::move(dist));
    }

    return result;
}

template class PolicyExtractor<models::sparse::Pomdp<double>, double, double>;
template class PolicyExtractor<storm::models::sparse::Pomdp<double>, storm::RationalNumber, double>;
template class PolicyExtractor<storm::models::sparse::Pomdp<storm::RationalNumber>, storm::RationalNumber, storm::RationalNumber>;
template class PolicyExtractor<storm::models::sparse::Pomdp<storm::RationalNumber>, storm::RationalNumber, double>;
template class PolicyExtractor<storm::models::sparse::Pomdp<double>, double, storm::RationalNumber>;
template class PolicyExtractor<storm::models::sparse::Pomdp<double>, double, double, storm::RationalNumber>;

}  // namespace storm::pomdp::policy
