#include <storm/exceptions/NotSupportedException.h>
#include "storm-pomdp/transformer/PomdpMemoryUnfolder.h"
#include "storm/storage/sparse/ModelComponents.h"

#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace transformer {

            
            template<typename ValueType>
            PomdpMemoryUnfolder<ValueType>::PomdpMemoryUnfolder(storm::models::sparse::Pomdp<ValueType> const& pomdp, uint64_t numMemoryStates) : pomdp(pomdp), numMemoryStates(numMemoryStates) {
                // intentionally left empty
            }

            
            template<typename ValueType>
            std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> PomdpMemoryUnfolder<ValueType>::transform() const {
                storm::storage::sparse::ModelComponents<ValueType> components;
                components.transitionMatrix = transformTransitions();
                components.stateLabeling = transformStateLabeling();
                components.observabilityClasses = transformObservabilityClasses();
                for (auto const& rewModel : pomdp.getRewardModels()) {
                    components.rewardModels.emplace(rewModel.first, transformRewardModel(rewModel.second));
                }
                
                return std::make_shared<storm::models::sparse::Pomdp<ValueType>>(std::move(components));
            }
        
        
            template<typename ValueType>
            storm::storage::SparseMatrix<ValueType> PomdpMemoryUnfolder<ValueType>::transformTransitions() const {
                storm::storage::SparseMatrix<ValueType> const& origTransitions = pomdp.getTransitionMatrix();
                storm::storage::SparseMatrixBuilder<ValueType> builder(pomdp.getNumberOfStates() * numMemoryStates * numMemoryStates,
                                                                        pomdp.getNumberOfStates() * numMemoryStates,
                                                                        origTransitions.getEntryCount() * numMemoryStates * numMemoryStates,
                                                                        true,
                                                                        false,
                                                                        pomdp.getNumberOfStates() * numMemoryStates);
                
                uint64_t row = 0;
                for (uint64_t modelState = 0; modelState < pomdp.getNumberOfStates(); ++modelState) {
                    for (uint32_t memState = 0; memState < numMemoryStates; ++memState) {
                        builder.newRowGroup(row);
                        for (uint64_t origRow = origTransitions.getRowGroupIndices()[modelState]; origRow < origTransitions.getRowGroupIndices()[modelState + 1]; ++origRow) {
                            for (uint32_t memStatePrime = 0; memStatePrime < numMemoryStates; ++memStatePrime) {
                                for (auto const& entry : origTransitions.getRow(origRow)) {
                                    builder.addNextValue(row, getUnfoldingState(entry.getColumn(), memStatePrime), entry.getValue());
                                }
                                ++row;
                            }
                        }
                    }
                }
                return builder.build();
            }
        
            template<typename ValueType>
            storm::models::sparse::StateLabeling PomdpMemoryUnfolder<ValueType>::transformStateLabeling() const {
                storm::models::sparse::StateLabeling labeling(pomdp.getNumberOfStates() * numMemoryStates);
                for (auto const& labelName : pomdp.getStateLabeling().getLabels()) {
                    storm::storage::BitVector newStates(pomdp.getNumberOfStates() * numMemoryStates, false);
                    for (auto const& modelState : pomdp.getStateLabeling().getStates(labelName)) {
                        for (uint32_t memState = 0; memState < numMemoryStates; ++memState) {
                            newStates.set(getUnfoldingState(modelState, memState));
                        }
                    }
                    labeling.addLabel(labelName, std::move(newStates));
                }
                return labeling;
            }
        
            template<typename ValueType>
            std::vector<uint32_t> PomdpMemoryUnfolder<ValueType>::transformObservabilityClasses() const {
                std::vector<uint32_t> observations;
                observations.reserve(pomdp.getNumberOfStates() * numMemoryStates);
                for (uint64_t modelState = 0; modelState < pomdp.getNumberOfStates(); ++modelState) {
                    for (uint32_t memState = 0; memState < numMemoryStates; ++memState) {
                        observations.push_back(getUnfoldingObersvation(pomdp.getObservation(modelState), memState));
                    }
                }
                return observations;
            }
        
            template<typename ValueType>
            storm::models::sparse::StandardRewardModel<ValueType> PomdpMemoryUnfolder<ValueType>::transformRewardModel(storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel) const {
                boost::optional<std::vector<ValueType>> stateRewards, actionRewards;
                if (rewardModel.hasStateRewards()) {
                    stateRewards = std::vector<ValueType>();
                    stateRewards->reserve(pomdp.getNumberOfStates() * numMemoryStates);
                    for (auto const& stateReward : rewardModel.getStateRewardVector()) {
                        for (uint32_t memState = 0; memState < numMemoryStates; ++memState) {
                            stateRewards->push_back(stateReward);
                        }
                    }
                }
                if (rewardModel.hasStateActionRewards()) {
                    actionRewards = std::vector<ValueType>();
                    stateRewards->reserve(pomdp.getNumberOfStates() * numMemoryStates * numMemoryStates);
                    for (uint64_t modelState = 0; modelState < pomdp.getNumberOfStates(); ++modelState) {
                        for (uint32_t memState = 0; memState < numMemoryStates; ++memState) {
                            for (uint64_t origRow = pomdp.getTransitionMatrix().getRowGroupIndices()[modelState]; origRow < pomdp.getTransitionMatrix().getRowGroupIndices()[modelState + 1]; ++origRow) {
                                ValueType const& actionReward = rewardModel.getStateActionReward(origRow);
                                for (uint32_t memStatePrime = 0; memStatePrime < numMemoryStates; ++memStatePrime) {
                                    actionRewards->push_back(actionReward);
                                }
                            }
                        }
                    }
                }
                STORM_LOG_THROW(rewardModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are currently not supported.");
                return storm::models::sparse::StandardRewardModel<ValueType>(std::move(stateRewards), std::move(actionRewards));
            }

            template<typename ValueType>
            uint64_t PomdpMemoryUnfolder<ValueType>::getUnfoldingState(uint64_t modelState, uint32_t memoryState) const {
                return modelState * numMemoryStates + memoryState;
            }
            
            template<typename ValueType>
            uint64_t PomdpMemoryUnfolder<ValueType>::getModelState(uint64_t unfoldingState) const {
                return unfoldingState / numMemoryStates;
            }
            
            template<typename ValueType>
            uint32_t PomdpMemoryUnfolder<ValueType>::getMemoryState(uint64_t unfoldingState) const {
                return unfoldingState % numMemoryStates;
            }
            
            template<typename ValueType>
            uint32_t PomdpMemoryUnfolder<ValueType>::getUnfoldingObersvation(uint32_t modelObservation, uint32_t memoryState) const {
                return modelObservation * numMemoryStates + memoryState;
            }
            
            template<typename ValueType>
            uint32_t PomdpMemoryUnfolder<ValueType>::getModelObersvation(uint32_t unfoldingObservation) const {
                return unfoldingObservation / numMemoryStates;
            }
            
            template<typename ValueType>
            uint32_t PomdpMemoryUnfolder<ValueType>::getMemoryStateFromObservation(uint32_t unfoldingObservation) const {
                return unfoldingObservation % numMemoryStates;
            }

            template class PomdpMemoryUnfolder<storm::RationalNumber>;
    }
}