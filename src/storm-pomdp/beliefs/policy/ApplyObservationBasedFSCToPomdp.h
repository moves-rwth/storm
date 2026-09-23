#pragma once

#include "storm-pomdp/beliefs/policy/ObservationBasedFiniteStateController.h"
#include "storm/api/export.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Pomdp.h"

namespace storm {
namespace pomdp::policy {
template<typename PomdpValueType>
ObservationSignature getObservationSignature(storm::models::sparse::Pomdp<PomdpValueType> const& pomdp, uint64_t const observationId) {
    return ObservationSignature::fromValuations(pomdp.getObservationValuations(), observationId);
}

/**
 * Applies an observation-based finite-state controller to a POMDP and constructs the induced DTMC.
 *
 * States of the resulting DTMC represent reachable pairs of a POMDP state and an FSC memory node. If the FSC provides observation signatures or action
 * names, they are used to match its outputs to the POMDP; otherwise, the corresponding numeric IDs are assumed to agree. Observation signatures match
 * observable names and typed values, so the observable-storage order need not agree. State and state-action rewards are preserved, while transition
 * rewards are ignored.
 *
 * @tparam PomdpValueType The type used for probabilities and rewards in the POMDP and resulting DTMC.
 * @tparam FscValueType The type used for probabilities in randomised FSC action outputs.
 * @param pomdp The POMDP to which the controller is applied.
 * @param fsc The finite-state controller to apply.
 * @param treatUnspecifiedChoiceAsDontCare If true, an unspecified FSC output selects local action zero and keeps the current FSC memory node. If false,
 * an unspecified output causes an exception.
 * @return The DTMC induced by the POMDP and the FSC.
 * @throws storm::exceptions::UnexpectedException If required observation or action names cannot be matched, or if the FSC has an unspecified output and
 * @p treatUnspecifiedChoiceAsDontCare is false.
 */
template<typename PomdpValueType, typename FscValueType>
storm::models::sparse::Dtmc<PomdpValueType> applyObservationBasedFSCToPomdp(
    storm::models::sparse::Pomdp<PomdpValueType> const& pomdp, storm::pomdp::policy::ObservationBasedFiniteStateController<FscValueType> const& fsc,
    bool treatUnspecifiedChoiceAsDontCare = false) {
    std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> idToPomdpStateAndFscNode;
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> pomdpStateAndFscNodeToNewStateId;
    // TODO change for vector?
    std::map<uint64_t, std::unordered_map<uint64_t, PomdpValueType>> transitionProbabilities;
    std::unordered_map<std::string, std::vector<PomdpValueType>> stateRewards;

    uint64_t initialStateId = 0ul;
    uint64_t nextStateId = 1ul;
    uint64_t nrEntries = 0ul;
    uint64_t initialPomdpState = pomdp.getInitialStates().getNextSetIndex(0ul);
    idToPomdpStateAndFscNode[initialStateId] = {initialPomdpState, fsc.getInitialNodeId()};
    pomdpStateAndFscNodeToNewStateId[initialPomdpState][fsc.getInitialNodeId()] = initialStateId;
    STORM_LOG_DEBUG("Add initial state " << initialStateId << " (P: " << initialPomdpState << ", F: " << fsc.getInitialNodeId() << ")");

    std::optional<std::unordered_map<uint64_t, uint64_t>> pomdpObservationIdToFscObservationId = std::nullopt;
    std::optional<std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>> fscActionIdToPomdpActionIdMap = std::nullopt;
    // If the POMDP and FSC have matching metadata, use it to create mappings between actions and observations.
    // Otherwise, we assume that IDs correspond directly.
    if (fsc.hasIdToObservationSignatureMap()) {
        STORM_LOG_THROW(pomdp.hasObservationValuations(), storm::exceptions::UnexpectedException,
                        "The FSC uses observation signatures, but the POMDP has no observation valuations. Cannot apply the FSC robustly.");
        std::unordered_map<uint64_t, uint64_t> observationMap;
        for (uint64_t pomdpObservationId = 0; pomdpObservationId < pomdp.getNrObservations(); ++pomdpObservationId) {
            auto const observationSignature = getObservationSignature(pomdp, pomdpObservationId);
            auto fscObservationId = fsc.getObservationIdBySignature(observationSignature);
            STORM_LOG_THROW(fscObservationId.has_value(), storm::exceptions::UnexpectedException,
                            "Could not map POMDP observation " << pomdpObservationId << " (" << observationSignatureToString(observationSignature)
                                                               << ") to an FSC observation.");
            observationMap[pomdpObservationId] = *fscObservationId;
        }
        pomdpObservationIdToFscObservationId = std::move(observationMap);
    }

    if (fsc.hasIdToActionNameMap()) {
        STORM_LOG_THROW(pomdp.hasChoiceLabeling(), storm::exceptions::UnexpectedException,
                        "The FSC uses action names, but the POMDP has no choice labeling. Cannot apply the FSC robustly.");
        std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> actionMap;
        for (uint64_t pomdpObservationId = 0; pomdpObservationId < pomdp.getNrObservations(); ++pomdpObservationId) {
            if (pomdp.getStatesWithObservation(pomdpObservationId).empty()) {
                continue;
            }
            uint64_t representativeState = *pomdp.getStatesWithObservation(pomdpObservationId).begin();
            uint64_t const pomdpRowGroupIndex = pomdp.getTransitionMatrix().getRowGroupIndices()[representativeState];
            uint64_t const pomdpRowGroupSize = pomdp.getTransitionMatrix().getRowGroupSize(representativeState);
            uint64_t const fscObservationId = pomdpObservationIdToFscObservationId && pomdpObservationIdToFscObservationId->contains(pomdpObservationId)
                                                  ? pomdpObservationIdToFscObservationId->at(pomdpObservationId)
                                                  : pomdpObservationId;
            for (uint64_t localActionIndex = 0; localActionIndex < pomdpRowGroupSize; ++localActionIndex) {
                auto const& labels = pomdp.getChoiceLabeling().getLabelsOfChoice(pomdpRowGroupIndex + localActionIndex);
                if (labels.empty()) {
                    actionMap[fscObservationId][localActionIndex] = localActionIndex;
                    continue;
                }
                std::string const& actionName = *labels.begin();
                if (auto fscActionId = fsc.getActionIdByName(fscObservationId, actionName)) {
                    actionMap[fscObservationId][*fscActionId] = localActionIndex;
                }
            }
        }
        fscActionIdToPomdpActionIdMap = std::move(actionMap);
    }

    if (!pomdpObservationIdToFscObservationId && !fscActionIdToPomdpActionIdMap) {
        STORM_LOG_WARN(
            "The POMDP or the FSC is missing naming information for actions or observations. Assuming that IDs correspond directly (this might lead to "
            "unexpected results if they don't).");
    }

    std::queue<uint64_t> statesToExplore;
    statesToExplore.push(initialStateId);
    while (!statesToExplore.empty()) {
        uint64_t currentStateId = statesToExplore.front();
        auto [currentPomdpState, currentFscNode] = idToPomdpStateAndFscNode[currentStateId];
        statesToExplore.pop();
        STORM_LOG_DEBUG("Explore state " << currentStateId << " (P: " << currentPomdpState << ", F: " << currentFscNode << ")");
        storm::storage::Distribution<FscValueType, uint64_t> actionDistribution;
        uint64_t nextFSCNode;
        uint64_t const pomdpObservationId = pomdp.getObservation(currentPomdpState);
        uint64_t const fscObservationId = pomdpObservationIdToFscObservationId && pomdpObservationIdToFscObservationId->contains(pomdpObservationId)
                                              ? pomdpObservationIdToFscObservationId->at(pomdpObservationId)
                                              : pomdpObservationId;
        if (fsc.hasOutputForObservationInNode(currentFscNode, fscObservationId)) {
            std::tie(actionDistribution, nextFSCNode) = fsc.getActionDistributionAndSuccessorForObservationInNode(currentFscNode, fscObservationId);
        } else {
            STORM_LOG_THROW(treatUnspecifiedChoiceAsDontCare, storm::exceptions::UnexpectedException,
                            "Expected FSC to have an output for observation " << fscObservationId << " in FSC node " << currentFscNode << ", but it does not.");
            // If requested, treat unspecified choice as don't care: pick the first enabled action deterministically and stay in the same FSC node
            nextFSCNode = currentFscNode;
            actionDistribution = storm::storage::Distribution<FscValueType, uint64_t>();
            actionDistribution.addProbability(0ul, storm::utility::one<FscValueType>());
        }
        for (auto const& [rewardModelName, rewardModel] : pomdp.getRewardModels()) {
            auto& rewards = stateRewards[rewardModelName];
            rewards.resize(currentStateId + 1, storm::utility::zero<PomdpValueType>());
            if (rewardModel.hasStateRewards()) {
                rewards[currentStateId] += rewardModel.getStateRewardVector()[currentPomdpState];
            }
            if (rewardModel.hasTransitionRewards()) {
                STORM_LOG_WARN("Transition rewards are not supported when applying an FSC to a POMDP. They will be ignored.");
            }
        }
        for (auto const& actionEntry : actionDistribution) {
            auto const actionProbability = storm::utility::convertNumber<PomdpValueType>(actionEntry.second);
            uint64_t pomdpActionId = actionEntry.first;
            if (fscActionIdToPomdpActionIdMap) {
                STORM_LOG_THROW(fscActionIdToPomdpActionIdMap->contains(fscObservationId) &&
                                    fscActionIdToPomdpActionIdMap->at(fscObservationId).contains(actionEntry.first),
                                storm::exceptions::UnexpectedException,
                                "Could not map FSC action " << actionEntry.first << " for FSC observation " << fscObservationId << " to a POMDP action.");
                pomdpActionId = fscActionIdToPomdpActionIdMap->at(fscObservationId).at(actionEntry.first);
            }
            for (auto const& [rewardModelName, rewardModel] : pomdp.getRewardModels()) {
                // We take the expected reward over the action distribution, so not all properties are preserved.
                if (rewardModel.hasStateActionRewards()) {
                    uint64_t globalActionIndex = pomdp.getTransitionMatrix().getRowGroupIndices()[currentPomdpState] + pomdpActionId;
                    stateRewards[rewardModelName][currentStateId] += rewardModel.getStateActionRewardVector()[globalActionIndex] * actionProbability;
                }
            }
            for (auto const& transitionEntry : pomdp.getTransitionMatrix().getRow(currentPomdpState, pomdpActionId)) {
                uint64_t successorPomdpState = transitionEntry.getColumn();
                uint64_t successorNewStateId;
                if (pomdpStateAndFscNodeToNewStateId.contains(successorPomdpState) &&
                    pomdpStateAndFscNodeToNewStateId[successorPomdpState].contains(nextFSCNode)) {
                    successorNewStateId = pomdpStateAndFscNodeToNewStateId[successorPomdpState][nextFSCNode];
                } else {
                    successorNewStateId = nextStateId;
                    idToPomdpStateAndFscNode[successorNewStateId] = {
                        successorPomdpState,
                        nextFSCNode,
                    };
                    pomdpStateAndFscNodeToNewStateId[successorPomdpState][nextFSCNode] = successorNewStateId;
                    STORM_LOG_DEBUG("Add state " << successorNewStateId << " (P: " << successorPomdpState << ", F: " << nextFSCNode << ")");
                    ++nextStateId;
                    statesToExplore.push(successorNewStateId);
                }
                if (transitionProbabilities.contains(currentStateId) && transitionProbabilities[currentStateId].contains(successorNewStateId)) {
                    // Add transition from currentStateId to successorNewStateId with probability from POMDP
                    transitionProbabilities[currentStateId][successorNewStateId] += transitionEntry.getValue() * actionProbability;
                } else {
                    transitionProbabilities[currentStateId][successorNewStateId] = transitionEntry.getValue() * actionProbability;
                    ++nrEntries;
                }
            }
        }
    }

    // Build transition matrix from collected transitions
    storm::storage::SparseMatrixBuilder<PomdpValueType> matrixBuilder(nextStateId, nextStateId, nrEntries);
    for (auto const& fromEntry : transitionProbabilities) {
        uint64_t fromStateId = fromEntry.first;
        for (auto const& toEntry : fromEntry.second) {
            uint64_t toStateId = toEntry.first;
            PomdpValueType prob = toEntry.second;
            matrixBuilder.addNextValue(fromStateId, toStateId, prob);
        }
    }
    auto transitionMatrix = matrixBuilder.build();
    storm::storage::sparse::ModelComponents<PomdpValueType> modelComponents;
    modelComponents.transitionMatrix = transitionMatrix;

    // Add state labeling
    storm::models::sparse::StateLabeling stateLabeling(nextStateId);
    stateLabeling.addLabel("init");
    stateLabeling.addLabelToState("init", initialStateId);
    for (uint64_t pomdpState = 0; pomdpState < pomdp.getNumberOfStates(); ++pomdpState) {
        if (pomdpStateAndFscNodeToNewStateId.contains(pomdpState)) {
            for (const auto& newStateId : pomdpStateAndFscNodeToNewStateId[pomdpState] | std::views::values) {
                for (auto const& label : pomdp.getStateLabeling().getLabelsOfState(pomdpState)) {
                    if (label != "init") {
                        if (!stateLabeling.containsLabel(label)) {
                            stateLabeling.addLabel(label);
                        }
                        stateLabeling.addLabelToState(label, newStateId);
                    }
                }
            }
        }
    }
    modelComponents.stateLabeling = stateLabeling;

    // build reward models
    for (auto const& [rewardModelName, rewardValues] : stateRewards) {
        storm::models::sparse::StandardRewardModel<PomdpValueType> rewardModel(rewardValues);
        modelComponents.rewardModels[rewardModelName] = rewardModel;
    }

    return storm::models::sparse::Dtmc<PomdpValueType>(modelComponents);
}
}  // namespace pomdp::policy
}  // namespace storm
