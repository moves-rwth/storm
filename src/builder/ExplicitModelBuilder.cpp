#include "src/builder/ExplicitModelBuilder.h"

#include <map>

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Ctmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/settings/modules/MarkovChainSettings.h"
#include "src/settings/modules/IOSettings.h"

#include "src/generator/PrismNextStateGenerator.h"

#include "src/utility/prism.h"
#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/ConstantsComparator.h"
#include "src/exceptions/WrongFormatException.h"

#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace builder {
       
        /*!
         * A structure that is used to keep track of a reward model currently being built.
         */
        template <typename ValueType>
        class RewardModelBuilder {
        public:
            RewardModelBuilder(storm::generator::RewardModelInformation const& rewardModelInformation) : rewardModelName(rewardModelInformation.getName()), stateRewards(rewardModelInformation.hasStateRewards()), stateRewardVector(), stateActionRewards(rewardModelInformation.hasStateActionRewards()), stateActionRewardVector() {
                STORM_LOG_THROW(!rewardModelInformation.hasTransitionRewards(), storm::exceptions::InvalidArgumentException, "Unable to treat transition rewards.");
            }
            
            storm::models::sparse::StandardRewardModel<ValueType> build(uint_fast64_t rowCount, uint_fast64_t columnCount, uint_fast64_t rowGroupCount) {
                boost::optional<std::vector<ValueType>> optionalStateRewardVector;
                if (hasStateRewards()) {
                    stateRewardVector.resize(rowGroupCount);
                    optionalStateRewardVector = std::move(stateRewardVector);
                }
                
                boost::optional<std::vector<ValueType>> optionalStateActionRewardVector;
                if (hasStateActionRewards()) {
                    stateActionRewardVector.resize(rowCount);
                    optionalStateActionRewardVector = std::move(stateActionRewardVector);
                }
                
                return storm::models::sparse::StandardRewardModel<ValueType>(std::move(optionalStateRewardVector), std::move(optionalStateActionRewardVector));
            }
            
            std::string const& getName() const {
                return rewardModelName;
            }
            
            void addStateReward(ValueType const& value) {
                stateRewardVector.push_back(value);
            }

            void addStateActionReward(ValueType const& value) {
                stateActionRewardVector.push_back(value);
            }
            
            bool hasStateRewards() const {
                return stateRewards;
            }
            
            bool hasStateActionRewards() const {
                return stateActionRewards;
            }
            
        private:
            std::string rewardModelName;

            bool stateRewards;
            bool stateActionRewards;
            
            // The state reward vector.
            std::vector<ValueType> stateRewardVector;
            
            // The state-action reward vector.
            std::vector<ValueType> stateActionRewardVector;
        };
                        
        template <typename ValueType, typename RewardModelType, typename StateType>
        ExplicitModelBuilder<ValueType, RewardModelType, StateType>::ModelComponents::ModelComponents() : transitionMatrix(), stateLabeling(), rewardModels(), choiceLabeling() {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        ExplicitModelBuilder<ValueType, RewardModelType, StateType>::Options::Options() : explorationOrder(storm::settings::getModule<storm::settings::modules::IOSettings>().getExplorationOrder()), buildStateValuations(false) {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        ExplicitModelBuilder<ValueType, RewardModelType, StateType>::ExplicitModelBuilder(std::shared_ptr<storm::generator::NextStateGenerator<ValueType, StateType>> const& generator, Options const& options) : generator(generator), options(options), stateStorage(generator->getStateSize()) {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        storm::storage::sparse::StateValuations const& ExplicitModelBuilder<ValueType, RewardModelType, StateType>::getStateValuations() const {
            STORM_LOG_THROW(static_cast<bool>(stateValuations), storm::exceptions::InvalidOperationException, "The state information was not properly build.");
            return stateValuations.get();
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> ExplicitModelBuilder<ValueType, RewardModelType, StateType>::translate() {
            STORM_LOG_DEBUG("Exploration order is: " << options.explorationOrder);
            ModelComponents modelComponents = buildModelComponents();
            
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> result;
            switch (generator->getModelType()) {
                case storm::generator::ModelType::DTMC:
                    result = std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>(new storm::models::sparse::Dtmc<ValueType, RewardModelType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.rewardModels), std::move(modelComponents.choiceLabeling)));
                    break;
                case storm::generator::ModelType::CTMC:
                    result = std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>(new storm::models::sparse::Ctmc<ValueType, RewardModelType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.rewardModels), std::move(modelComponents.choiceLabeling)));
                    break;
                case storm::generator::ModelType::MDP:
                    result = std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>(new storm::models::sparse::Mdp<ValueType, RewardModelType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.rewardModels), std::move(modelComponents.choiceLabeling)));
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error while creating model from probabilistic program: cannot handle this model type.");
                    break;
            }
            
            return result;
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        StateType ExplicitModelBuilder<ValueType, RewardModelType, StateType>::getOrAddStateIndex(CompressedState const& state) {
            StateType newIndex = static_cast<StateType>(stateStorage.getNumberOfStates());
            
            // Check, if the state was already registered.
            std::pair<StateType, std::size_t> actualIndexBucketPair = stateStorage.stateToId.findOrAddAndGetBucket(state, newIndex);
            
            if (actualIndexBucketPair.first == newIndex) {
                if (options.explorationOrder == ExplorationOrder::Dfs) {
                    statesToExplore.push_front(state);

                    // Reserve one slot for the new state in the remapping.
                    stateRemapping.get().push_back(storm::utility::zero<StateType>());
                } else if (options.explorationOrder == ExplorationOrder::Bfs) {
                    statesToExplore.push_back(state);
                } else {
                    STORM_LOG_ASSERT(false, "Invalid exploration order.");
                }
            }
            
            return actualIndexBucketPair.first;
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> ExplicitModelBuilder<ValueType, RewardModelType, StateType>::buildMatrices(storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<RewardModelBuilder<typename RewardModelType::ValueType>>& rewardModelBuilders) {
            // Create choice labels, if requested,
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabels;
            if (generator->getOptions().isBuildChoiceLabelsSet()) {
                choiceLabels = std::vector<boost::container::flat_set<uint_fast64_t>>();
            }

            // Create a callback for the next-state generator to enable it to request the index of states.
            std::function<StateType (CompressedState const&)> stateToIdCallback = std::bind(&ExplicitModelBuilder<ValueType, RewardModelType, StateType>::getOrAddStateIndex, this, std::placeholders::_1);
            
            // If the exploration order is something different from breadth-first, we need to keep track of the remapping
            // from state ids to row groups. For this, we actually store the reversed mapping of row groups to state-ids
            // and later reverse it.
            if (options.explorationOrder != ExplorationOrder::Bfs) {
                stateRemapping = std::vector<uint_fast64_t>();
            }
            
            // Let the generator create all initial states.
            this->stateStorage.initialStateIndices = generator->getInitialStates(stateToIdCallback);
            
            // Now explore the current state until there is no more reachable state.
            uint_fast64_t currentRowGroup = 0;
            uint_fast64_t currentRow = 0;

            // Perform a search through the model.
            while (!statesToExplore.empty()) {
                // Get the first state in the queue.
                CompressedState currentState = statesToExplore.front();
                StateType currentIndex = stateStorage.stateToId.getValue(currentState);
                statesToExplore.pop_front();
                
                // If the exploration order differs from breadth-first, we remember that this row group was actually
                // filled with the transitions of a different state.
                if (options.explorationOrder != ExplorationOrder::Bfs) {
                    stateRemapping.get()[currentIndex] = currentRowGroup;
                }
                
                STORM_LOG_TRACE("Exploring state with id " << currentIndex << ".");
                
                generator->load(currentState);
                storm::generator::StateBehavior<ValueType, StateType> behavior = generator->expand(stateToIdCallback);
                
                // If there is no behavior, we might have to introduce a self-loop.
                if (behavior.empty()) {
                    if (!storm::settings::getModule<storm::settings::modules::MarkovChainSettings>().isDontFixDeadlocksSet() || !behavior.wasExpanded()) {
                        if (generator->getOptions().isBuildChoiceLabelsSet()) {
                            // Insert empty choice labeling for added self-loop transitions.
                            choiceLabels.get().push_back(boost::container::flat_set<uint_fast64_t>());
                        }
                        if (!generator->isDeterministicModel()) {
                            transitionMatrixBuilder.newRowGroup(currentRow);
                        }
                        
                        transitionMatrixBuilder.addNextValue(currentRow, currentIndex, storm::utility::one<ValueType>());
                        
                        for (auto& rewardModelBuilder : rewardModelBuilders) {
                            if (rewardModelBuilder.hasStateRewards()) {
                                rewardModelBuilder.addStateReward(storm::utility::zero<ValueType>());
                            }
                            
                            if (rewardModelBuilder.hasStateActionRewards()) {
                                rewardModelBuilder.addStateActionReward(storm::utility::zero<ValueType>());
                            }
                        }
                        
                        ++currentRow;
                        ++currentRowGroup;
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error while creating sparse matrix from probabilistic program: found deadlock state (" << generator->toValuation(currentState).toString(true) << "). For fixing these, please provide the appropriate option.");
                    }
                } else {
                    // Add the state rewards to the corresponding reward models.
                    auto stateRewardIt = behavior.getStateRewards().begin();
                    for (auto& rewardModelBuilder : rewardModelBuilders) {
                        if (rewardModelBuilder.hasStateRewards()) {
                            rewardModelBuilder.addStateReward(*stateRewardIt);
                        }
                        ++stateRewardIt;
                    }
                    
                    // If the model is nondeterministic, we need to open a row group.
                    if (!generator->isDeterministicModel()) {
                        transitionMatrixBuilder.newRowGroup(currentRow);
                    }
                    
                    // Now add all choices.
                    for (auto const& choice : behavior) {
                        // Add command labels if requested.
                        if (generator->getOptions().isBuildChoiceLabelsSet()) {
                            choiceLabels.get().push_back(choice.getChoiceLabels());
                        }
                        
                        // Add the probabilistic behavior to the matrix.
                        for (auto const& stateProbabilityPair : choice) {
                            transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                        }
                        
                        // Add the rewards to the reward models.
                        auto choiceRewardIt = choice.getChoiceRewards().begin();
                        for (auto& rewardModelBuilder : rewardModelBuilders) {
                            if (rewardModelBuilder.hasStateActionRewards()) {
                                rewardModelBuilder.addStateActionReward(*choiceRewardIt);
                            }
                            ++choiceRewardIt;
                        }
                        ++currentRow;
                    }
                    ++currentRowGroup;
                }
            }

            // If the exploration order was not breadth-first, we need to fix the entries in the matrix according to
            // (reversed) mapping of row groups to indices.
            if (options.explorationOrder != ExplorationOrder::Bfs) {
                STORM_LOG_ASSERT(stateRemapping, "Unable to fix columns without mapping.");
                std::vector<uint_fast64_t> const& remapping = stateRemapping.get();
                
                // We need to fix the following entities:
                // (a) the transition matrix
                // (b) the initial states
                // (c) the hash map storing the mapping states -> ids
                
                // Fix (a).
                transitionMatrixBuilder.replaceColumns(remapping, 0);

                // Fix (b).
                std::vector<StateType> newInitialStateIndices(this->stateStorage.initialStateIndices.size());
                std::transform(this->stateStorage.initialStateIndices.begin(), this->stateStorage.initialStateIndices.end(), newInitialStateIndices.begin(), [&remapping] (StateType const& state) { return remapping[state]; } );
                std::sort(newInitialStateIndices.begin(), newInitialStateIndices.end());
                this->stateStorage.initialStateIndices = std::move(newInitialStateIndices);
                
                // Fix (c).
                this->stateStorage.stateToId.remap([&remapping] (StateType const& state) { return remapping[state]; } );
            }
            
            return choiceLabels;
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        typename ExplicitModelBuilder<ValueType, RewardModelType, StateType>::ModelComponents ExplicitModelBuilder<ValueType, RewardModelType, StateType>::buildModelComponents() {
            ModelComponents modelComponents;
                        
            // Determine whether we have to combine different choices to one or whether this model can have more than
            // one choice per state.
            bool deterministicModel = generator->isDeterministicModel();
            
            // Prepare the transition matrix builder and the reward model builders.
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);
            std::vector<RewardModelBuilder<typename RewardModelType::ValueType>> rewardModelBuilders;
            for (uint64_t i = 0; i < generator->getNumberOfRewardModels(); ++i) {
                rewardModelBuilders.emplace_back(generator->getRewardModelInformation(i));
            }
            
            modelComponents.choiceLabeling = buildMatrices(transitionMatrixBuilder, rewardModelBuilders);
            modelComponents.transitionMatrix = transitionMatrixBuilder.build();
            
            // Now finalize all reward models.
            for (auto& rewardModelBuilder : rewardModelBuilders) {
                modelComponents.rewardModels.emplace(rewardModelBuilder.getName(), rewardModelBuilder.build(modelComponents.transitionMatrix.getRowCount(), modelComponents.transitionMatrix.getColumnCount(), modelComponents.transitionMatrix.getRowGroupCount()));
            }
            
            // Build the state labeling.
            modelComponents.stateLabeling = buildStateLabeling();
            
            // Finally -- if requested -- build the state information that can be retrieved from the outside.
            if (options.buildStateValuations) {
                stateValuations = storm::storage::sparse::StateValuations(stateStorage.getNumberOfStates());
                for (auto const& bitVectorIndexPair : stateStorage.stateToId) {
                    stateValuations.get().valuations[bitVectorIndexPair.second] = generator->toValuation(bitVectorIndexPair.first);
                }
            }
            
            return modelComponents;
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        storm::models::sparse::StateLabeling ExplicitModelBuilder<ValueType, RewardModelType, StateType>::buildStateLabeling() {
            return generator->label(stateStorage.stateToId, stateStorage.initialStateIndices);
        }
        
        // Explicitly instantiate the class.
        template class ExplicitModelBuilder<double, storm::models::sparse::StandardRewardModel<double>, uint32_t>;
        
#ifdef STORM_HAVE_CARL
        template class ExplicitModelBuilder<double, storm::models::sparse::StandardRewardModel<storm::Interval>, uint32_t>;
        template class ExplicitModelBuilder<RationalFunction, storm::models::sparse::StandardRewardModel<RationalFunction>, uint32_t>;
#endif
    }
}
