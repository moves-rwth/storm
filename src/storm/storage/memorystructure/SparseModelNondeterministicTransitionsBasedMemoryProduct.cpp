//
// Created by Florent Delgrange on 2019-05-28.
//

#include "SparseModelNondeterministicTransitionsBasedMemoryProduct.h"

namespace storm {
    namespace storage {

        template<typename SparseModelType>
        SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::SparseModelNondeterministicTransitionsBasedMemoryProduct(SparseModelType const& model, storm::storage::NondeterministicMemoryStructure const& memory, bool forceLabeling)
        : model(model), memory(memory), productStates(model.getNumberOfStates()), forceLabeling(forceLabeling)
        {}

        template<typename SparseModelType>
        std::shared_ptr<SparseModelType> SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::build() {
            // For simplicity we first build the 'full' product of model and memory (with model.numStates * memory.numStates states).
            storm::storage::sparse::ModelComponents<ValueType> components;
            components.transitionMatrix = buildTransitions();
            components.stateLabeling = buildStateLabeling();
            components.choiceLabeling = buildChoiceLabeling(components.transitionMatrix);
            // Now delete unreachable states.
            storm::storage::BitVector allStates(components.transitionMatrix.getRowGroupCount(), true);
            reachableStates = storm::utility::graph::getReachableStates(components.transitionMatrix, components.stateLabeling.getStates("init"), allStates, ~allStates);
            storm::storage::BitVector enabledActions(components.transitionMatrix.getRowCount());
            for (uint64_t state : reachableStates) {
                for (uint64_t row = components.transitionMatrix.getRowGroupIndices()[state]; row < components.transitionMatrix.getRowGroupIndices()[state + 1]; ++ row) {
                    enabledActions.set(row);
                }
            }
            components.transitionMatrix = components.transitionMatrix.getSubmatrix(true, reachableStates, reachableStates);
            components.stateLabeling = components.stateLabeling.getSubLabeling(reachableStates);
            components.choiceLabeling = components.choiceLabeling->getSubLabeling(enabledActions);

            // build the remaining components
            for (auto const& rewModel : model.getRewardModels()) {
                components.rewardModels.emplace(rewModel.first, buildRewardModel(rewModel.second, reachableStates, components.transitionMatrix));
            }

            // build the offset vector, that allows to maintain getter indices
            fullProductStatesOffset = std::move(generateOffsetVector(reachableStates));

            return std::make_shared<SparseModelType>(std::move(components));
        }

        template<typename SparseModelType>
        storm::storage::SparseMatrix<typename SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::ValueType> SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::buildTransitions() {
            storm::storage::SparseMatrix<ValueType> const& origTransitions = model.getTransitionMatrix();
            uint64_t numRows = 0;
            uint64_t numEntries = 0;
            uint64_t numberOfStates = model.getNumberOfStates() * memory.getNumberOfStates() * (1 + model.getNumberOfTransitions());
            productStates[0] = 0;
            for (uint64_t modelState = 0; modelState < model.getNumberOfStates(); ++ modelState) {
                if (modelState < model.getNumberOfStates() - 1) {
                    productStates[modelState + 1] = productStates[modelState] + memory.getNumberOfStates() * (1 + origTransitions.getRowGroupEntryCount(modelState));
                }
                for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++ memState) {
                    for (uint64_t row = origTransitions.getRowGroupIndices()[modelState];
                         row < origTransitions.getRowGroupIndices()[modelState + 1];
                         ++ row) {
                        numRows += 1 + origTransitions.getRow(row).getNumberOfEntries() * memory.getNumberOfOutgoingTransitions(memState);
                        numEntries += origTransitions.getRow(row).getNumberOfEntries() * (1 + memory.getNumberOfOutgoingTransitions(memState));
                    }
                }
            }
            storm::storage::SparseMatrixBuilder<ValueType> builder(numRows,
                                                                   numberOfStates,
                                                                   numEntries,
                                                                   true,
                                                                   true,
                                                                   numberOfStates);

            uint64_t row = 0;
            for (uint64_t modelState = 0; modelState < model.getNumberOfStates(); ++ modelState) {
                for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++ memState) {
                    builder.newRowGroup(row);
                    uint64_t entryCount = 0;
                    for (uint64_t origRow = origTransitions.getRowGroupIndices()[modelState]; origRow < origTransitions.getRowGroupIndices()[modelState + 1]; ++origRow) {
                        for (auto const& entry : origTransitions.getRow(origRow)) {
                            uint64_t productState = getProductState(modelState, memState);
                            builder.addNextValue(row, productState + 1 + entryCount, entry.getValue());
                            ++ entryCount;
                        }
                        ++row;
                    }
                    // transition states
                    for (uint64_t origRow = origTransitions.getRowGroupIndices()[modelState]; origRow < origTransitions.getRowGroupIndices()[modelState + 1]; ++origRow) {
                        for (auto const& entry : origTransitions.getRow(origRow)) {
                            builder.newRowGroup(row);
                            for (auto const& memStatePrime : memory.getTransitions(memState)) {
                                builder.addNextValue(row, getProductState(entry.getColumn(), memStatePrime), storm::utility::one<ValueType>());
                                ++row;
                            }
                        }
                    }
                }
            }
            return builder.build();
        }

        template<typename SparseModelType>
        storm::models::sparse::StateLabeling SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::buildStateLabeling() const {
            storm::storage::SparseMatrix<ValueType> const& origTransitions = model.getTransitionMatrix();
            // storm::storage::SparseMatrix<ValueType> backwardTransitions = origTransitions.transpose(true);
            uint64_t numberOfStates = model.getNumberOfStates() * memory.getNumberOfStates() * (1 + model.getNumberOfTransitions());
            storm::models::sparse::StateLabeling labeling(numberOfStates);
            for (auto const& labelName : model.getStateLabeling().getLabels()) {
                storm::storage::BitVector newStates(numberOfStates, false);

                // The init label is only assigned to Product states with the initial memory state
                if (labelName == "init") {
                    for (auto const& modelState : model.getStateLabeling().getStates(labelName)) {
                        uint64_t nextProductState = getProductState(modelState, memory.getInitialState() + 1); //memory.getInitialState() < memory.getNumberOfStates() - 1 ? getProductState(modelState, memory.getInitialState() + 1) : getProductState(modelState + 1, 0);
                        // by assuming that all states between (s, m) and (s, m + 1) are labeled with s
                        for (uint64_t productState = getProductState(modelState, memory.getInitialState()); productState < nextProductState; ++ productState) {
                            newStates.set(productState);
                        }
                    }
                } else {
                    for (auto const& modelState : model.getStateLabeling().getStates(labelName)) {
                        // by assuming that all states between (s, 0) and (s + 1, 0) are labeled with labels of s
                        for (uint64_t productState = getProductState(modelState, 0); productState < getProductState(modelState, memory.getNumberOfStates()); ++ productState) {
                            newStates.set(productState);
                        }
                    }
                }
                labeling.addLabel(labelName, std::move(newStates));
            }

            auto addLabel = [&] (std::string const& labelName, uint64_t state) -> void {
                if (not labeling.containsLabel(labelName)) {
                    labeling.addLabel(labelName);
                }
                labeling.addLabelToState(labelName, state);
            };

            for (uint64_t modelState = 0; modelState < model.getNumberOfStates(); ++ modelState) {
                if (forceLabeling) {
                    for (uint64_t memoryState = 0; memoryState < memory.getNumberOfStates(); ++ memoryState) {
                        if (labeling.getLabelsOfState(getProductState(modelState, memoryState)).empty()) {
                            std::ostringstream stream;
                            stream << "s" << modelState;
                            std::string labelName = stream.str();
                            addLabel(labelName, getProductState(modelState, memoryState));
                        }
                        {
                            std::ostringstream stream;
                            stream << "m" << memoryState;
                            std::string labelName = stream.str();
                            addLabel(labelName, getProductState(modelState, memoryState));
                        }
                    }
                }
                uint64_t entryCount = 0;
                for (uint64_t row = origTransitions.getRowGroupIndices()[modelState]; row < origTransitions.getRowGroupIndices()[modelState + 1]; ++ row) {
                    for (auto const& entry : origTransitions.getRow(row)) {
                        uint64_t const& successor = entry.getColumn();
                        for (uint64_t memoryState = 0; memoryState < memory.getNumberOfStates(); ++ memoryState) {
                            uint64_t productState = getProductState(modelState, memoryState) + 1 + entryCount;
                            // origin state
                            if ( model.getStateLabeling().getLabelsOfState(modelState).empty() or
                                (model.getStateLabeling().getLabelsOfState(modelState).size() == 1 and model.getStateLabeling().getStateHasLabel("init", modelState)
                                and not labeling.getStateHasLabel("init", productState)) ){
                                if (forceLabeling) {
                                    std::ostringstream stream;
                                    stream << "s" << modelState;
                                    std::string labelName = stream.str();
                                    addLabel(labelName, productState);
                                }
                            } else  {
                                for (auto const& labelName : model.getStateLabeling().getLabelsOfState(modelState)) {
                                    if (labelName != "init") {
                                        addLabel(labelName, productState);
                                    }
                                }
                            }
                            // memory labeling
                            if (forceLabeling) {
                                std::ostringstream stream;
                                stream << "m" << memoryState;
                                std::string labelName = stream.str();
                                addLabel(labelName, productState);
                            }
                            // action labeling
                            if (forceLabeling and (not model.getOptionalChoiceLabeling()
                                or model.getChoiceLabeling().getLabelsOfChoice(row).empty())) {
                                std::ostringstream stream;
                                stream << "a" << row;
                                std::string labelName = stream.str();
                                addLabel(labelName, productState);
                            } else if (model.getOptionalChoiceLabeling()) {
                                for (auto const &labelName : model.getChoiceLabeling().getLabelsOfChoice(row)) {
                                    addLabel(labelName, productState);
                                }
                            }
                            // arrival state
                            if ( model.getStateLabeling().getLabelsOfState(successor).empty() or
                                (model.getStateLabeling().getLabelsOfState(successor).size() == 1 and model.getStateLabeling().getStateHasLabel("init", successor)
                                 and not labeling.getStateHasLabel("init", productState)) ){
                                if (forceLabeling) {
                                    std::ostringstream stream;
                                    stream << "s" << successor;
                                    std::string labelName = stream.str();
                                    addLabel(labelName, productState);
                                }
                            } else  {
                                for (auto const& labelName : model.getStateLabeling().getLabelsOfState(successor)) {
                                    if (labelName != "init") {
                                        addLabel(labelName, productState);
                                    }
                                }
                            }
                        }
                        ++ entryCount;
                    }
                }
            }
            return labeling;
        }

        template<typename SparseModelType>
        storm::models::sparse::ChoiceLabeling SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::buildChoiceLabeling(storm::storage::SparseMatrix<ValueType> const& transitions) const {
            storm::storage::SparseMatrix<ValueType> const& origTransitions = model.getTransitionMatrix();
            storm::models::sparse::ChoiceLabeling labeling(transitions.getRowCount());

            auto addLabel = [&] (std::string const& labelName, uint64_t row) -> void {
                if (not labeling.containsLabel(labelName)) {
                    labeling.addLabel(labelName);
                }
                labeling.addLabelToChoice(labelName, row);
            };

            uint64_t row = 0;
            for (uint64_t modelState = 0; modelState < model.getNumberOfStates(); ++ modelState) {
                for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++ memState) {
                    for (uint64_t origRow = origTransitions.getRowGroupIndices()[modelState]; origRow < origTransitions.getRowGroupIndices()[modelState + 1]; ++origRow) {
                        if (forceLabeling and (not model.getOptionalChoiceLabeling()
                                               or model.getChoiceLabeling().getLabelsOfChoice(origRow).empty())) {
                            std::ostringstream stream;
                            stream << "a" << origRow;
                            std::string labelName = stream.str();
                            addLabel(labelName, row);
                        } else if (model.getOptionalChoiceLabeling()) {
                            for (auto const &labelName : model.getChoiceLabeling().getLabelsOfChoice(origRow)) {
                                addLabel(labelName, row);
                            }
                        }
                        ++row;
                    }
                    // transition states
                    for (uint64_t origRow = origTransitions.getRowGroupIndices()[modelState]; origRow < origTransitions.getRowGroupIndices()[modelState + 1]; ++origRow) {
                        for (auto const& entry : origTransitions.getRow(origRow)) {
                            for (auto const& memStatePrime : memory.getTransitions(memState)) {
                                if (forceLabeling) {
                                    std::ostringstream stream;
                                    stream << "m" << memStatePrime;
                                    std::string labelName = stream.str();
                                    addLabel(labelName, row);
                                }
                                ++row;
                            }
                        }
                    }
                }
            }
            return labeling;
        }

        template<typename SparseModelType>
        storm::models::sparse::StandardRewardModel<typename SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::ValueType> SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::buildRewardModel(storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel, storm::storage::BitVector const& reachableStates, storm::storage::SparseMatrix<ValueType> const& resultTransitionMatrix) const {
            boost::optional<std::vector<ValueType>> stateRewards, actionRewards;
            if (rewardModel.hasStateRewards()) {
                stateRewards = std::vector<ValueType>();
                stateRewards->reserve(resultTransitionMatrix.getRowGroupCount());
                for (uint64_t modelState = 0; modelState < model.getNumberOfStates(); ++ modelState) {
                    for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++ memState) {
                        if (reachableStates.get(getProductState(modelState, memState))) {
                            stateRewards->push_back(rewardModel.getStateReward(modelState));
                        }
                    }
                }
            }
            if (rewardModel.hasStateActionRewards()) {
                actionRewards = std::vector<ValueType>();
                actionRewards->reserve(resultTransitionMatrix.getRowCount());
                for (uint64_t modelState = 0; modelState < model.getNumberOfStates(); ++ modelState) {
                    uint64_t numberOfModelOutgoingTransitions = model.getTransitionMatrix().getRowGroupEntryCount(modelState);
                    for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++ memState) {
                        uint64_t numberOfMemoryOutgoingTransitions = memory.getNumberOfOutgoingTransitions(memState);
                        if (reachableStates.get(getProductState(modelState, memState))) {
                            for (uint64_t origRow = model.getTransitionMatrix().getRowGroupIndices()[modelState]; origRow < model.getTransitionMatrix().getRowGroupIndices()[modelState + 1]; ++ origRow) {
                                ValueType const& actionReward = rewardModel.getStateActionReward(origRow);
                                // push the reward of choosing a in (s, m)
                                actionRewards->push_back(actionReward);
                            }
                            // note that all outgoing model states are reachable by definition
                            actionRewards->insert(actionRewards->end(), numberOfModelOutgoingTransitions * numberOfMemoryOutgoingTransitions, storm::utility::zero<ValueType>());
                        }
                    }
                }
            }
            STORM_LOG_THROW(!rewardModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are currently not supported.");
            return storm::models::sparse::StandardRewardModel<ValueType>(std::move(stateRewards), std::move(actionRewards));
        }

        template<typename SparseModelType>
        std::vector<uint64_t> SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::generateOffsetVector(storm::storage::BitVector const& reachableStates) {
            uint64_t numberOfStates = model.getNumberOfStates() * memory.getNumberOfStates() * (1 + model.getNumberOfTransitions());
            STORM_LOG_ASSERT(reachableStates.size() == numberOfStates, "vector reachableStates has wrong size");
            uint64_t state = 0;
            uint64_t offset = 0;
            std::vector<uint64_t> offsetVector(numberOfStates);
            while (state < numberOfStates) {
                if (reachableStates[state]) {
                    offsetVector[state] = offset;
                    ++ state;
                }
                else {
                    uint64_t nextState = reachableStates.getNextSetIndex(state);
                    offset += nextState - state;
                    for (; state < nextState; ++ state) {
                        offsetVector[state] = offset;
                    }
                }
            }

            return std::move(offsetVector);
        }

        template<typename SparseModelType>
        uint64_t SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::getProductState(uint64_t modelState, uint64_t memoryState) const {
            uint64_t index = productStates[modelState] + memoryState * (1 + model.getTransitionMatrix().getRowGroupEntryCount(modelState));
            return index - (fullProductStatesOffset.empty() ? 0 : fullProductStatesOffset[index]);
        }

        template<typename SparseModelType>
        bool SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::isProductStateReachable(uint64_t modelState, uint64_t memoryState) const {
            STORM_LOG_ASSERT(not fullProductStatesOffset.empty(), "The product is not yet built");
            uint64_t index = productStates[modelState] + memoryState * (1 + model.getTransitionMatrix().getRowGroupEntryCount(modelState));
            return reachableStates[index];
        }

        template<typename SparseModelType>
        uint64_t SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::getModelState(uint64_t productState) const {
            uint64_t productStateWithOffset = productState + (fullProductStatesOffset.empty() ? 0 : fullProductStatesOffset[productState]);
            // binary search in the vector containing the product states indices
            auto search = std::upper_bound(productStates.begin(), productStates.end(), productStateWithOffset);
            return search - productStates.begin() - 1;
        }

        template<typename SparseModelType>
        uint64_t SparseModelNondeterministicTransitionsBasedMemoryProduct<SparseModelType>::getMemoryState(uint64_t productState) const {
            uint64_t productStateWithOffset = productState + (fullProductStatesOffset.empty() ? 0 : fullProductStatesOffset[productState]);
            // binary search in the vector containing the product states indices
            uint64_t modelState = std::upper_bound(productStates.begin(), productStates.end(), productStateWithOffset) - productStates.begin() - 1;
            uint64_t offset = productStateWithOffset - productStates[modelState];
            return offset / (1 + model.getTransitionMatrix().getRowGroupEntryCount(modelState));
        }

        template class SparseModelNondeterministicTransitionsBasedMemoryProduct<storm::models::sparse::Mdp<double>>;
        template class SparseModelNondeterministicTransitionsBasedMemoryProduct<storm::models::sparse::Mdp<storm::RationalNumber>>;
    }
}
