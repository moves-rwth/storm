#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <map>
#include <boost/optional.hpp>

#include "storm/api/properties.h"
#include "storm/api/verification.h"

#include "storm/storage/BitVector.h"
#include "storm/utility/macros.h"
#include "storm-pomdp/storage/BeliefManager.h"
#include "storm/utility/SignalHandler.h"

namespace storm {
    namespace builder {
        template<typename PomdpType>
        class BeliefMdpExplorer {
        public:
            typedef typename PomdpType::ValueType ValueType;
            typedef storm::storage::BeliefManager<PomdpType> BeliefManagerType;
            typedef typename BeliefManagerType::BeliefId BeliefId;
            typedef uint64_t MdpStateType;
            
            BeliefMdpExplorer(std::shared_ptr<BeliefManagerType> beliefManager, std::vector<ValueType> const& pomdpLowerValueBounds, std::vector<ValueType> const& pomdpUpperValueBounds) : beliefManager(beliefManager), pomdpLowerValueBounds(pomdpLowerValueBounds), pomdpUpperValueBounds(pomdpUpperValueBounds) {
                // Intentionally left empty
            }

            void startNewExploration(boost::optional<ValueType> extraTargetStateValue = boost::none, boost::optional<ValueType> extraBottomStateValue = boost::none) {
                // Reset data from potential previous explorations
                mdpStateToBeliefIdMap.clear();
                beliefIdToMdpStateMap.clear();
                beliefIdsWithMdpState.clear();
                beliefIdsWithMdpState.grow(beliefManager->getNumberOfBeliefIds(), false);
                lowerValueBounds.clear();
                upperValueBounds.clear();
                values.clear();
                mdpTransitionsBuilder = storm::storage::SparseMatrixBuilder<ValueType>(0, 0, 0, true, true);
                currentRowCount = 0;
                startOfCurrentRowGroup = 0;
                mdpActionRewards.clear();
                exploredMdp = nullptr;
                
                // Add some states with special treatment (if requested)
                if (extraBottomStateValue) {
                    extraBottomState = getCurrentNumberOfMdpStates();
                    mdpStateToBeliefIdMap.push_back(beliefManager->noId());
                    insertValueHints(extraBottomStateValue.get(), extraBottomStateValue.get());

                    startOfCurrentRowGroup = currentRowCount;
                    mdpTransitionsBuilder.newRowGroup(startOfCurrentRowGroup);
                    mdpTransitionsBuilder.addNextValue(currentRowCount, extraBottomState.get(), storm::utility::one<ValueType>());
                    ++currentRowCount;
                } else {
                    extraBottomState = boost::none;
                }
                if (extraTargetStateValue) {
                    extraTargetState = getCurrentNumberOfMdpStates();
                    mdpStateToBeliefIdMap.push_back(beliefManager->noId());
                    insertValueHints(extraTargetStateValue.get(), extraTargetStateValue.get());
                    
                    startOfCurrentRowGroup = currentRowCount;
                    mdpTransitionsBuilder.newRowGroup(startOfCurrentRowGroup);
                    mdpTransitionsBuilder.addNextValue(currentRowCount, extraTargetState.get(), storm::utility::one<ValueType>());
                    ++currentRowCount;
                    
                    targetStates.grow(getCurrentNumberOfMdpStates(), false);
                    targetStates.set(extraTargetState.get(), true);
                } else {
                    extraTargetState = boost::none;
                }
                
                // Set up the initial state.
                initialMdpState = getOrAddMdpState(beliefManager->getInitialBelief());
            }
    
            bool hasUnexploredState() const {
                return !beliefIdsToExplore.empty();
            }
    
            BeliefId exploreNextState() {
                // Set up the matrix builder
                finishCurrentRow();
                startOfCurrentRowGroup = currentRowCount;
                mdpTransitionsBuilder.newRowGroup(startOfCurrentRowGroup);
                ++currentRowCount;
                
                // Pop from the queue.
                auto result = beliefIdsToExplore.front();
                beliefIdsToExplore.pop_front();
                return result;
            }
            
            void addTransitionsToExtraStates(uint64_t const& localActionIndex, ValueType const& targetStateValue = storm::utility::zero<ValueType>(), ValueType const& bottomStateValue = storm::utility::zero<ValueType>()) {
                // We first insert the entries of the current row in a separate map.
                // This is to ensure that entries are sorted in the right way (as required for the transition matrix builder)
                
                uint64_t row = startOfCurrentRowGroup + localActionIndex;
                if (!storm::utility::isZero(bottomStateValue)) {
                    STORM_LOG_ASSERT(extraBottomState.is_initialized(), "Requested a transition to the extra bottom state but there is none.");
                    internalAddTransition(row, extraBottomState.get(), bottomStateValue);
                }
                if (!storm::utility::isZero(targetStateValue)) {
                    STORM_LOG_ASSERT(extraTargetState.is_initialized(), "Requested a transition to the extra target state but there is none.");
                    internalAddTransition(row, extraTargetState.get(), targetStateValue);
                }
            }
            
            void addSelfloopTransition(uint64_t const& localActionIndex = 0, ValueType const& value = storm::utility::one<ValueType>()) {
                uint64_t row = startOfCurrentRowGroup + localActionIndex;
                internalAddTransition(row, getCurrentMdpState(), value);
            }
            
            /*!
             * Adds the next transition to the given successor belief
             * @param localActionIndex
             * @param transitionTarget
             * @param value
             * @param ignoreNewBeliefs If true, beliefs that were not found before are not inserted, i.e. we might not insert the transition.
             * @return true iff a transition was actually inserted. False can only happen if ignoreNewBeliefs is true.
             */
            bool addTransitionToBelief(uint64_t const& localActionIndex, BeliefId const& transitionTarget, ValueType const& value, bool ignoreNewBeliefs) {
                // We first insert the entries of the current row in a separate map.
                // This is to ensure that entries are sorted in the right way (as required for the transition matrix builder)
                MdpStateType column;
                if (ignoreNewBeliefs) {
                    column = getMdpState(transitionTarget);
                    if (column == noState()) {
                        return false;
                    }
                } else {
                    column = getOrAddMdpState(transitionTarget);
                }
                uint64_t row = startOfCurrentRowGroup + localActionIndex;
                internalAddTransition(row, column, value);
                return true;
            }
            
            void computeRewardAtCurrentState(uint64 const& localActionIndex, ValueType extraReward = storm::utility::zero<ValueType>()) {
                if (currentRowCount >= mdpActionRewards.size()) {
                    mdpActionRewards.resize(currentRowCount, storm::utility::zero<ValueType>());
                }
                uint64_t row = startOfCurrentRowGroup + localActionIndex;
                mdpActionRewards[row] = beliefManager->getBeliefActionReward(getCurrentBeliefId(), localActionIndex) + extraReward;
            }
            
            void setCurrentStateIsTarget() {
                targetStates.grow(getCurrentNumberOfMdpStates(), false);
                targetStates.set(getCurrentMdpState(), true);
            }
            
            void setCurrentStateIsTruncated() {
                truncatedStates.grow(getCurrentNumberOfMdpStates(), false);
                truncatedStates.set(getCurrentMdpState(), true);
            }
            
            void finishExploration() {
                // Create the tranistion matrix
                finishCurrentRow();
                auto mdpTransitionMatrix = mdpTransitionsBuilder.build(getCurrentNumberOfMdpChoices(), getCurrentNumberOfMdpStates(), getCurrentNumberOfMdpStates());
                
                // Create a standard labeling
                storm::models::sparse::StateLabeling mdpLabeling(getCurrentNumberOfMdpStates());
                mdpLabeling.addLabel("init");
                mdpLabeling.addLabelToState("init", initialMdpState);
                targetStates.resize(getCurrentNumberOfMdpStates(), false);
                mdpLabeling.addLabel("target", std::move(targetStates));
                truncatedStates.resize(getCurrentNumberOfMdpStates(), false);
                mdpLabeling.addLabel("truncated", std::move(truncatedStates));
                
                // Create a standard reward model (if rewards are available)
                std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>> mdpRewardModels;
                if (!mdpActionRewards.empty()) {
                    mdpActionRewards.resize(getCurrentNumberOfMdpChoices(), storm::utility::zero<ValueType>());
                    mdpRewardModels.emplace("default", storm::models::sparse::StandardRewardModel<ValueType>(boost::optional<std::vector<ValueType>>(), std::move(mdpActionRewards)));
                }
                
                storm::storage::sparse::ModelComponents<ValueType> modelComponents(std::move(mdpTransitionMatrix), std::move(mdpLabeling), std::move(mdpRewardModels));
                exploredMdp = std::make_shared<storm::models::sparse::Mdp<ValueType>>(std::move(modelComponents));
            }
            
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> getExploredMdp() const {
                STORM_LOG_ASSERT(exploredMdp, "Tried to get the explored MDP but exploration was not finished yet.");
                return exploredMdp;
            }
            
            MdpStateType getCurrentNumberOfMdpStates() const {
                return mdpStateToBeliefIdMap.size();
            }
    
            MdpStateType getCurrentNumberOfMdpChoices() const {
                return currentRowCount;
            }

            ValueType getLowerValueBoundAtCurrentState() const {
                return lowerValueBounds[getCurrentMdpState()];
            }

            ValueType getUpperValueBoundAtCurrentState() const {
                return upperValueBounds[getCurrentMdpState()];
            }

            ValueType computeLowerValueBoundAtBelief(BeliefId const& beliefId) const {
                return beliefManager->getWeightedSum(beliefId, pomdpLowerValueBounds);
            }

            ValueType computeUpperValueBoundAtBelief(BeliefId const& beliefId) const {
                 return beliefManager->getWeightedSum(beliefId, pomdpUpperValueBounds);
            }
            
            std::vector<ValueType> const& computeValuesOfExploredMdp(storm::solver::OptimizationDirection const& dir) {
                STORM_LOG_ASSERT(exploredMdp, "Tried to compute values but the MDP is not explored");
                auto property = createStandardProperty(dir, exploredMdp->hasRewardModel());
                auto task = createStandardCheckTask(property);
                
                std::unique_ptr<storm::modelchecker::CheckResult> res(storm::api::verifyWithSparseEngine<ValueType>(exploredMdp, task));
                if (res) {
                    values = std::move(res->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                } else {
                    STORM_LOG_ASSERT(storm::utility::resources::isTerminate(), "Empty check result!");
                    STORM_LOG_ERROR("No result obtained while checking.");
                }
                return values;
            }
            
            ValueType const& getComputedValueAtInitialState() const {
                STORM_LOG_ASSERT(exploredMdp, "Tried to get a value but no MDP was explored.");
                return values[exploredMdp->getInitialStates().getNextSetIndex(0)];
            }
            
        private:
            MdpStateType noState() const {
                return std::numeric_limits<MdpStateType>::max();
            }
            
            std::shared_ptr<storm::logic::Formula const> createStandardProperty(storm::solver::OptimizationDirection const& dir, bool computeRewards) {
                std::string propertyString = computeRewards ? "R" : "P";
                propertyString += storm::solver::minimize(dir) ? "min" : "max";
                propertyString += "=? [F \"target\"]";
                std::vector<storm::jani::Property> propertyVector = storm::api::parseProperties(propertyString);
                return storm::api::extractFormulasFromProperties(propertyVector).front();
            }
            
            storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> createStandardCheckTask(std::shared_ptr<storm::logic::Formula const>& property) {
                //Note: The property should not run out of scope after calling this because the task only stores the property by reference.
                // Therefore, this method needs the property by reference (and not const reference)
                auto task = storm::api::createTask<ValueType>(property, false);
                auto hint = storm::modelchecker::ExplicitModelCheckerHint<ValueType>();
                hint.setResultHint(values);
                auto hintPtr = std::make_shared<storm::modelchecker::ExplicitModelCheckerHint<ValueType>>(hint);
                task.setHint(hintPtr);
                return task;
            }
            
            MdpStateType getCurrentMdpState() const {
                return mdpTransitionsBuilder.getCurrentRowGroupCount() - 1;
            }
            
            MdpStateType getCurrentBeliefId() const {
                return mdpStateToBeliefIdMap[getCurrentMdpState()];
            }
            
            void internalAddTransition(uint64_t const& row, MdpStateType const& column, ValueType const& value) {
                // We first insert the entries of the current row in a separate map.
                // This is to ensure that entries are sorted in the right way (as required for the transition matrix builder)
                STORM_LOG_ASSERT(row >= currentRowCount - 1, "Trying to insert in an already completed row.");
                if (row >= currentRowCount) {
                    // We are going to start a new row, so insert the entries of the old one
                    finishCurrentRow();
                    currentRowCount = row + 1;
                }
                STORM_LOG_ASSERT(mdpTransitionsBuilderCurrentRowEntries.count(column) == 0, "Trying to insert multiple transitions to the same state.");
                mdpTransitionsBuilderCurrentRowEntries[column] = value;
            }
            
            void finishCurrentRow() {
                for (auto const& entry : mdpTransitionsBuilderCurrentRowEntries) {
                    mdpTransitionsBuilder.addNextValue(currentRowCount - 1, entry.first, entry.second);
                }
                mdpTransitionsBuilderCurrentRowEntries.clear();
            }
            
            MdpStateType getMdpState(BeliefId const& beliefId) const {
                if (beliefId < beliefIdsWithMdpState.size() && beliefIdsWithMdpState.get(beliefId)) {
                    return beliefIdToMdpStateMap.at(beliefId);
                } else {
                    return noState();
                }
            }
            
            void insertValueHints(ValueType const& lowerBound, ValueType const& upperBound) {
                lowerValueBounds.push_back(lowerBound);
                upperValueBounds.push_back(upperBound);
                // Take the middle value as a hint
                values.push_back((lowerBound + upperBound) / storm::utility::convertNumber<ValueType, uint64_t>(2));
                STORM_LOG_ASSERT(lowerValueBounds.size() == getCurrentNumberOfMdpStates(), "Value vectors have different size then number of available states.");
                STORM_LOG_ASSERT(lowerValueBounds.size() == upperValueBounds.size() && values.size() == upperValueBounds.size(), "Value vectors have inconsistent size.");
            }
            
            MdpStateType getOrAddMdpState(BeliefId const& beliefId) {
                beliefIdsWithMdpState.grow(beliefId + 1, false);
                if (beliefIdsWithMdpState.get(beliefId)) {
                    return beliefIdToMdpStateMap[beliefId];
                } else {
                    // Add a new MDP state
                    beliefIdsWithMdpState.set(beliefId, true);
                    MdpStateType result = getCurrentNumberOfMdpStates();
                    assert(getCurrentNumberOfMdpStates() == mdpStateToBeliefIdMap.size());
                    mdpStateToBeliefIdMap.push_back(beliefId);
                    beliefIdToMdpStateMap[beliefId] = result;
                    // This new belief needs exploration
                    beliefIdsToExplore.push_back(beliefId);
                    
                    insertValueHints(computeLowerValueBoundAtBelief(beliefId), computeUpperValueBoundAtBelief(beliefId));
                    return result;
                }
            }
            
            // Belief state related information
            std::shared_ptr<BeliefManagerType> beliefManager;
            std::vector<BeliefId> mdpStateToBeliefIdMap;
            std::map<BeliefId, MdpStateType> beliefIdToMdpStateMap;
            storm::storage::BitVector beliefIdsWithMdpState;
            
            // Exploration information
            std::deque<uint64_t> beliefIdsToExplore;
            storm::storage::SparseMatrixBuilder<ValueType> mdpTransitionsBuilder;
            std::map<MdpStateType, ValueType> mdpTransitionsBuilderCurrentRowEntries;
            std::vector<ValueType> mdpActionRewards;
            uint64_t startOfCurrentRowGroup;
            uint64_t currentRowCount;
            
            // Special states during exploration
            boost::optional<MdpStateType> extraTargetState;
            boost::optional<MdpStateType> extraBottomState;
            storm::storage::BitVector targetStates;
            storm::storage::BitVector truncatedStates;
            MdpStateType initialMdpState;

            // Final Mdp
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> exploredMdp;
            
            // Value related information
            std::vector<ValueType> const& pomdpLowerValueBounds;
            std::vector<ValueType> const& pomdpUpperValueBounds;
            std::vector<ValueType> lowerValueBounds;
            std::vector<ValueType> upperValueBounds;
            std::vector<ValueType> values; // Contains an estimate during building and the actual result after a check has performed
            
        };
    }
}