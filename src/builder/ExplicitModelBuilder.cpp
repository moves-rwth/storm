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
        struct RewardModelBuilder {
        public:
            RewardModelBuilder(std::string const& rewardModelName) : rewardModelName(rewardModelName), stateRewardVector(), stateActionRewardVector() {
                // Intentionally left empty.
            }
            
            storm::models::sparse::StandardRewardModel<ValueType> build(uint_fast64_t rowCount, uint_fast64_t columnCount, uint_fast64_t rowGroupCount) {
                boost::optional<std::vector<ValueType>> optionalStateRewardVector;
                if (!stateRewardVector.empty()) {
                    stateRewardVector.resize(rowGroupCount);
                    optionalStateRewardVector = std::move(stateRewardVector);
                }
                
                boost::optional<std::vector<ValueType>> optionalStateActionRewardVector;
                if (!stateActionRewardVector.empty()) {
                    stateActionRewardVector.resize(rowCount);
                    optionalStateActionRewardVector = std::move(stateActionRewardVector);
                }
                
                return storm::models::sparse::StandardRewardModel<ValueType>(std::move(optionalStateRewardVector), std::move(optionalStateActionRewardVector));
            }
            
            std::string rewardModelName;
            
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
        ExplicitModelBuilder<ValueType, RewardModelType, StateType>::ExplicitModelBuilder(storm::prism::Program const& program, Options const& options) : program(storm::utility::prism::preprocessProgram<ValueType>(program, options.constantDefinitions, !options.buildAllLabels ? options.labelsToBuild : boost::none, options.expressionLabels)), options(options), variableInformation(this->program), stateStorage(variableInformation.getTotalBitOffset(true)) {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        storm::storage::sparse::StateValuations const& ExplicitModelBuilder<ValueType, RewardModelType, StateType>::getStateValuations() const {
            STORM_LOG_THROW(static_cast<bool>(stateValuations), storm::exceptions::InvalidOperationException, "The state information was not properly build.");
            return stateValuations.get();
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> ExplicitModelBuilder<ValueType, RewardModelType, StateType>::translate() {
            STORM_LOG_DEBUG("Building representation of program:" << std::endl << program << std::endl);
            STORM_LOG_DEBUG("Exploration order is: " << options.explorationOrder);
            
            // First, we make sure that all selected reward models actually exist.
            for (auto const& rewardModelName : options.rewardModelsToBuild) {
                STORM_LOG_THROW(rewardModelName.empty() || program.hasRewardModel(rewardModelName), storm::exceptions::InvalidArgumentException, "Model does not possess a reward model with the name '" << rewardModelName << "'.");
            }
            
            std::vector<std::string> selectedRewardModels;
            if (options.buildAllRewardModels) {
//                for (auto const& rewardModel : program.getRewardModels()) {
//                    selectedRewardModels.push_back(rewardModel);
//                }
            } else {
                selectedRewardModels = std::vector<std::string>(options.rewardModelsToBuild.begin(), options.rewardModelsToBuild.end());
            }
//            // If no reward model was selected until now and a referenced reward model appears to be unique, we build
//            // the only existing reward model (given that no explicit name was given for the referenced reward model).
//            if (selectedRewardModels.empty() && program.getNumberOfRewardModels() == 1 && options.rewardModelsToBuild.size() == 1 && *options.rewardModelsToBuild.begin() == "") {
//                selectedRewardModels.push_back(program.getRewardModel(0));
//            }
            
            ModelComponents modelComponents = buildModelComponents(selectedRewardModels);
            
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> result;
            switch (program.getModelType()) {
                case storm::prism::Program::ModelType::DTMC:
                    result = std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>(new storm::models::sparse::Dtmc<ValueType, RewardModelType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.rewardModels), std::move(modelComponents.choiceLabeling)));
                    break;
                case storm::prism::Program::ModelType::CTMC:
                    result = std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>(new storm::models::sparse::Ctmc<ValueType, RewardModelType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.rewardModels), std::move(modelComponents.choiceLabeling)));
                    break;
                case storm::prism::Program::ModelType::MDP:
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
        boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> ExplicitModelBuilder<ValueType, RewardModelType, StateType>::buildMatrices(std::vector<std::reference_wrapper<storm::prism::RewardModel const>> const& selectedRewardModels, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<RewardModelBuilder<typename RewardModelType::ValueType>>& rewardModelBuilders, boost::optional<storm::expressions::Expression> const& terminalExpression) {
            // Create choice labels, if requested,
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabels;
            if (options.buildCommandLabels) {
                choiceLabels = std::vector<boost::container::flat_set<uint_fast64_t>>();
            }

            // Create a generator that is able to expand states.
            storm::generator::PrismNextStateGenerator<ValueType, StateType> generator(program, variableInformation, options.buildCommandLabels);
            if (terminalExpression) {
                generator.setTerminalExpression(terminalExpression.get());
            }
            for (auto const& rewardModel : selectedRewardModels) {
                generator.addRewardModel(rewardModel.get());
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
            this->stateStorage.initialStateIndices = generator.getInitialStates(stateToIdCallback);
            
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
                
                generator.load(currentState);
                storm::generator::StateBehavior<ValueType, StateType> behavior = generator.expand(stateToIdCallback);
                
                // If there is no behavior, we might have to introduce a self-loop.
                if (behavior.empty()) {
                    if (!storm::settings::getModule<storm::settings::modules::MarkovChainSettings>().isDontFixDeadlocksSet() || !behavior.wasExpanded()) {
                        if (options.buildCommandLabels) {
                            // Insert empty choice labeling for added self-loop transitions.
                            choiceLabels.get().push_back(boost::container::flat_set<uint_fast64_t>());
                        }
                        if (!generator.isDeterministicModel()) {
                            transitionMatrixBuilder.newRowGroup(currentRow);
                        }
                        
                        transitionMatrixBuilder.addNextValue(currentRow, currentIndex, storm::utility::one<ValueType>());
                        
                        auto builderIt = rewardModelBuilders.begin();
                        for (auto const& rewardModel : selectedRewardModels) {
                            if (rewardModel.get().hasStateRewards()) {
                                builderIt->stateRewardVector.push_back(storm::utility::zero<ValueType>());
                            }
                            
                            if (rewardModel.get().hasStateActionRewards()) {
                                builderIt->stateActionRewardVector.push_back(storm::utility::zero<ValueType>());
                            }
                            ++builderIt;
                        }
                        
                        ++currentRow;
                        ++currentRowGroup;
                    } else {
                        std::cout << storm::generator::unpackStateIntoValuation(currentState, variableInformation, program.getManager()).toString(true) << std::endl;
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException,
                                        "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.");
                    }
                } else {
                    // Add the state rewards to the corresponding reward models.
                    auto builderIt = rewardModelBuilders.begin();
                    auto stateRewardIt = behavior.getStateRewards().begin();
                    for (auto const& rewardModel : selectedRewardModels) {
                        if (rewardModel.get().hasStateRewards()) {
                            builderIt->stateRewardVector.push_back(*stateRewardIt);
                        }
                        ++stateRewardIt;
                        ++builderIt;
                    }
                    
                    // If the model is nondeterministic, we need to open a row group.
                    if (!generator.isDeterministicModel()) {
                        transitionMatrixBuilder.newRowGroup(currentRow);
                    }
                    
                    // Now add all choices.
                    for (auto const& choice : behavior) {
                        // Add command labels if requested.
                        if (options.buildCommandLabels) {
                            choiceLabels.get().push_back(choice.getChoiceLabels());
                        }
                        
                        // Add the probabilistic behavior to the matrix.
                        for (auto const& stateProbabilityPair : choice) {
                            transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                        }
                        
                        // Add the rewards to the reward models.
                        auto builderIt = rewardModelBuilders.begin();
                        auto choiceRewardIt = choice.getChoiceRewards().begin();
                        for (auto const& rewardModel : selectedRewardModels) {
                            if (rewardModel.get().hasStateActionRewards()) {
                                builderIt->stateActionRewardVector.push_back(*choiceRewardIt);
                            }
                            ++choiceRewardIt;
                            ++builderIt;
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
        typename ExplicitModelBuilder<ValueType, RewardModelType, StateType>::ModelComponents ExplicitModelBuilder<ValueType, RewardModelType, StateType>::buildModelComponents(std::vector<std::string> const& selectedRewardModels) {
            ModelComponents modelComponents;
                        
            // Determine whether we have to combine different choices to one or whether this model can have more than
            // one choice per state.
            bool deterministicModel = program.isDeterministicModel();
            
            // Prepare the transition matrix builder and the reward model builders.
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);
            std::vector<RewardModelBuilder<typename RewardModelType::ValueType>> rewardModelBuilders;
            for (auto const& rewardModelName : selectedRewardModels) {
                rewardModelBuilders.emplace_back(rewardModelName);
            }
            
            // If we were asked to treat some states as terminal states, we determine an expression characterizing the
            // terminal states of the model that we pass on to the matrix building routine.
            boost::optional<storm::expressions::Expression> terminalExpression;
            if (options.terminalStates) {
                if (options.terminalStates.get().type() == typeid(storm::expressions::Expression)) {
                    terminalExpression = boost::get<storm::expressions::Expression>(options.terminalStates.get());
                } else {
                    std::string const& labelName = boost::get<std::string>(options.terminalStates.get());
                    terminalExpression = program.getLabelExpression(labelName);
                }
            }
            if (options.negatedTerminalStates) {
                if (options.negatedTerminalStates.get().type() == typeid(storm::expressions::Expression)) {
                    if (terminalExpression) {
                        terminalExpression = terminalExpression.get() || !boost::get<storm::expressions::Expression>(options.negatedTerminalStates.get());
                    } else {
                        terminalExpression = !boost::get<storm::expressions::Expression>(options.negatedTerminalStates.get());
                    }
                } else {
                    std::string const& labelName = boost::get<std::string>(options.negatedTerminalStates.get());
                    if (terminalExpression) {
                        terminalExpression = terminalExpression.get() || !program.getLabelExpression(labelName);
                    } else {
                        terminalExpression = !program.getLabelExpression(labelName);
                    }
                }
            }
            if (terminalExpression) {
                STORM_LOG_TRACE("Making the states satisfying " << terminalExpression.get() << " terminal.");
            }
            
            modelComponents.choiceLabeling = buildMatrices(selectedRewardModels, transitionMatrixBuilder, rewardModelBuilders, terminalExpression);
            modelComponents.transitionMatrix = transitionMatrixBuilder.build();
            
            // Now finalize all reward models.
            auto builderIt = rewardModelBuilders.begin();
            for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                modelComponents.rewardModels.emplace(rewardModelIt->get().getName(), builderIt->build(modelComponents.transitionMatrix.getRowCount(), modelComponents.transitionMatrix.getColumnCount(), modelComponents.transitionMatrix.getRowGroupCount()));
            }
            
            // Build the state labeling.
            modelComponents.stateLabeling = buildStateLabeling();
            
            // Finally -- if requested -- build the state information that can be retrieved from the outside.
            if (options.buildStateValuations) {
                stateValuations = storm::storage::sparse::StateValuations(stateStorage.getNumberOfStates());
                for (auto const& bitVectorIndexPair : stateStorage.stateToId) {
                    stateValuations.get().valuations[bitVectorIndexPair.second] = unpackStateIntoValuation(bitVectorIndexPair.first, variableInformation, program.getManager());
                }
            }
            
            return modelComponents;
        }
        
        template <typename ValueType, typename RewardModelType, typename StateType>
        storm::models::sparse::StateLabeling ExplicitModelBuilder<ValueType, RewardModelType, StateType>::buildStateLabeling() {
            storm::generator::PrismStateLabelingGenerator<ValueType, StateType> generator(program, variableInformation);
            return generator.generate(stateStorage.stateToId, stateStorage.initialStateIndices);
        }
        
        // Explicitly instantiate the class.
        template class ExplicitModelBuilder<double, storm::models::sparse::StandardRewardModel<double>, uint32_t>;
        
#ifdef STORM_HAVE_CARL
        template class ExplicitModelBuilder<double, storm::models::sparse::StandardRewardModel<storm::Interval>, uint32_t>;
        template class ExplicitModelBuilder<RationalFunction, storm::models::sparse::StandardRewardModel<RationalFunction>, uint32_t>;
#endif
    }
}
