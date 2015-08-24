#include "src/builder/ExplicitPrismModelBuilder.h"

#include <map>

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Ctmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/settings/modules/GeneralSettings.h"

#include "src/utility/prism.h"
#include "src/utility/macros.h"
#include "src/utility/ConstantsComparator.h"
#include "src/exceptions/WrongFormatException.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace builder {
        /*!
         * A structure that is used to keep track of a reward model currently being built.
         */
        template <typename ValueType>
        struct RewardModelBuilder {
        public:
            RewardModelBuilder(bool deterministicModel, bool hasStateRewards, bool hasStateActionRewards, bool hasTransitionRewards) : hasStateRewards(hasStateRewards), hasStateActionRewards(hasStateActionRewards), hasTransitionRewards(hasTransitionRewards), stateRewardVector(), stateActionRewardVector(), transitionRewardMatrixBuilder(0, 0, 0, false, !deterministicModel, 0) {
                // Intentionally left empty.
            }
            
            storm::models::sparse::StandardRewardModel<ValueType> build(uint_fast64_t rowCount, uint_fast64_t columnCount, uint_fast64_t rowGroupCount) {
                boost::optional<std::vector<ValueType>> optionalStateRewardVector;
                if (hasStateRewards) {
                    stateRewardVector.resize(rowGroupCount);
                    optionalStateRewardVector = std::move(stateRewardVector);
                }

                boost::optional<std::vector<ValueType>> optionalStateActionRewardVector;
                if (hasStateActionRewards) {
                    stateActionRewardVector.resize(rowCount);
                    optionalStateActionRewardVector = std::move(stateActionRewardVector);
                }
                
                boost::optional<storm::storage::SparseMatrix<ValueType>> optionalTransitionRewardMatrix;
                if (hasTransitionRewards) {
                    optionalTransitionRewardMatrix = transitionRewardMatrixBuilder.build(rowCount, columnCount, rowGroupCount);
                }
                
                return storm::models::sparse::StandardRewardModel<ValueType>(std::move(optionalStateRewardVector), std::move(optionalStateActionRewardVector), std::move(optionalTransitionRewardMatrix));
            }
            
            bool hasStateRewards;
            bool hasStateActionRewards;
            bool hasTransitionRewards;
            
            // The state reward vector.
            std::vector<ValueType> stateRewardVector;
            
            // The state-action reward vector.
            std::vector<ValueType> stateActionRewardVector;
            
            // A builder that is used for constructing the transition reward matrix.
            storm::storage::SparseMatrixBuilder<ValueType> transitionRewardMatrixBuilder;
        };
        
        template <typename ValueType, typename IndexType>
        ExplicitPrismModelBuilder<ValueType, IndexType>::StateInformation::StateInformation(uint64_t bitsPerState) : stateStorage(bitsPerState, 10000000), bitsPerState(bitsPerState), reachableStates() {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename IndexType>
        ExplicitPrismModelBuilder<ValueType, IndexType>::VariableInformation::BooleanVariableInformation::BooleanVariableInformation(storm::expressions::Variable const& variable, bool initialValue, uint_fast64_t bitOffset) : variable(variable), initialValue(initialValue), bitOffset(bitOffset) {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename IndexType>
        ExplicitPrismModelBuilder<ValueType, IndexType>::VariableInformation::IntegerVariableInformation::IntegerVariableInformation(storm::expressions::Variable const& variable, int_fast64_t initialValue, int_fast64_t lowerBound, int_fast64_t upperBound, uint_fast64_t bitOffset, uint_fast64_t bitWidth) : variable(variable), initialValue(initialValue), lowerBound(lowerBound), upperBound(upperBound), bitOffset(bitOffset), bitWidth(bitWidth) {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename IndexType>
        uint_fast64_t ExplicitPrismModelBuilder<ValueType, IndexType>::VariableInformation::getBitOffset(storm::expressions::Variable const& variable) const {
            auto const& booleanIndex = booleanVariableToIndexMap.find(variable);
            if (booleanIndex != booleanVariableToIndexMap.end()) {
                return booleanVariables[booleanIndex->second].bitOffset;
            }
            auto const& integerIndex = integerVariableToIndexMap.find(variable);
            if (integerIndex != integerVariableToIndexMap.end()) {
                return integerVariables[integerIndex->second].bitOffset;
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot look-up bit index of unknown variable.");
        }

        template <typename ValueType, typename IndexType>
        uint_fast64_t ExplicitPrismModelBuilder<ValueType, IndexType>::VariableInformation::getBitWidth(storm::expressions::Variable const& variable) const {
            auto const& integerIndex = integerVariableToIndexMap.find(variable);
            if (integerIndex != integerVariableToIndexMap.end()) {
                return integerVariables[integerIndex->second].bitWidth;
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot look-up bit width of unknown variable.");
        }
        
        template <typename ValueType, typename IndexType>
        ExplicitPrismModelBuilder<ValueType, IndexType>::ModelComponents::ModelComponents() : transitionMatrix(), stateLabeling(), rewardModels(), choiceLabeling() {
            // Intentionally left empty.
        }

        template <typename ValueType, typename IndexType>
        ExplicitPrismModelBuilder<ValueType, IndexType>::Options::Options() : buildCommandLabels(false), buildAllRewardModels(true), rewardModelsToBuild(), constantDefinitions(), buildAllLabels(true), labelsToBuild(), expressionLabels() {
            // Intentionally left empty.
        }
        
        template <typename ValueType, typename IndexType>
        ExplicitPrismModelBuilder<ValueType, IndexType>::Options::Options(storm::logic::Formula const& formula) : buildCommandLabels(false), buildAllRewardModels(false), rewardModelsToBuild(), constantDefinitions(), buildAllLabels(false), labelsToBuild(std::set<std::string>()), expressionLabels(std::vector<storm::expressions::Expression>()) {
            this->preserveFormula(formula);
        }
        
        template <typename ValueType, typename IndexType>
        ExplicitPrismModelBuilder<ValueType, IndexType>::Options::Options(std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) : buildCommandLabels(false), buildAllRewardModels(false), rewardModelsToBuild(), constantDefinitions(), buildAllLabels(false), labelsToBuild(), expressionLabels() {
            if (formulas.empty()) {
                this->buildAllRewardModels = true;
                this->buildAllLabels = true;
            } else {
                for (auto const& formula : formulas) {
                    this->preserveFormula(*formula);
                }
            }
        }

        template <typename ValueType, typename IndexType>
        void ExplicitPrismModelBuilder<ValueType, IndexType>::Options::preserveFormula(storm::logic::Formula const& formula) {
            // If we are not required to build all reward models, we determine the reward models we need to build.
            if (!buildAllRewardModels) {
                if (formula.containsRewardOperator()) {
                    std::set<std::string> referencedRewardModels = formula.getReferencedRewardModels();
                    rewardModelsToBuild.insert(referencedRewardModels.begin(), referencedRewardModels.end());
                }
            }
            
            // Extract all the labels used in the formula.
            if (!buildAllLabels) {
                if (!labelsToBuild) {
                    labelsToBuild = std::set<std::string>();
                }
                
                std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> atomicLabelFormulas = formula.getAtomicLabelFormulas();
                for (auto const& formula : atomicLabelFormulas) {
                    labelsToBuild.get().insert(formula.get()->getLabel());
                }
            }
            
            // Extract all the expressions used in the formula.
            std::vector<std::shared_ptr<storm::logic::AtomicExpressionFormula const>> atomicExpressionFormulas = formula.getAtomicExpressionFormulas();
            for (auto const& formula : atomicExpressionFormulas) {
                expressionLabels.get().push_back(formula.get()->getExpression());
            }
        }
        
        template <typename ValueType, typename IndexType>
        void ExplicitPrismModelBuilder<ValueType, IndexType>::Options::addConstantDefinitionsFromString(storm::prism::Program const& program, std::string const& constantDefinitionString) {
            std::map<storm::expressions::Variable, storm::expressions::Expression> newConstantDefinitions = storm::utility::prism::parseConstantDefinitionString(program, constantDefinitionString);
            
            // If there is at least one constant that is defined, and the constant definition map does not yet exist,
            // we need to create it.
            if (!constantDefinitions && !newConstantDefinitions.empty()) {
                constantDefinitions = std::map<storm::expressions::Variable, storm::expressions::Expression>();
            }
            
            // Now insert all the entries that need to be defined.
            for (auto const& entry : newConstantDefinitions) {
                constantDefinitions.get().insert(entry);
            }
        }
        
        template <typename ValueType, typename IndexType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> ExplicitPrismModelBuilder<ValueType, IndexType>::translateProgram(storm::prism::Program program, Options const& options) {
            // Start by defining the undefined constants in the model.
            storm::prism::Program preparedProgram;
            if (options.constantDefinitions) {
                preparedProgram = program.defineUndefinedConstants(options.constantDefinitions.get());
            } else {
                preparedProgram = program;
            }
            
            // If the program still contains undefined constants and we are not in a parametric setting, assemble an appropriate error message.
#ifdef STORM_HAVE_CARL
            // If the program either has undefined constants or we are building a parametric model, but the parameters
            // not only appear in the probabilities, we re
            if (!std::is_same<ValueType, storm::RationalFunction>::value && preparedProgram.hasUndefinedConstants()) {
#else
            if (preparedProgram.hasUndefinedConstants()) {
#endif
                std::vector<std::reference_wrapper<storm::prism::Constant const>> undefinedConstants = preparedProgram.getUndefinedConstants();
                std::stringstream stream;
                bool printComma = false;
                for (auto const& constant : undefinedConstants) {
                    if (printComma) {
                        stream << ", ";
                    } else {
                        printComma = true;
                    }
                    stream << constant.get().getName() << " (" << constant.get().getType() << ")";
                }
                stream << ".";
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Program still contains these undefined constants: " + stream.str());
#ifdef STORM_HAVE_CARL
            } else if (std::is_same<ValueType, storm::RationalFunction>::value && !preparedProgram.hasUndefinedConstantsOnlyInUpdateProbabilitiesAndRewards()) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The program contains undefined constants that appear in some places other than update probabilities and reward value expressions, which is not admitted.");
#endif
            }
            
            // If the set of labels we are supposed to built is restricted, we need to remove the other labels from the program.
            if (options.labelsToBuild) {
                if (!options.buildAllLabels) {
                    preparedProgram.filterLabels(options.labelsToBuild.get());
                }
            }
            
            // If we need to build labels for expressions that may appear in some formula, we need to add appropriate
            // labels to the program.
            if (options.expressionLabels) {
                for (auto const& expression : options.expressionLabels.get()) {
                    std::stringstream stream;
                    stream << expression;
                    std::string name = stream.str();
                    if (!preparedProgram.hasLabel(name)) {
                        preparedProgram.addLabel(name, expression);
                    }
                }
            }
            
            // Now that the program is fixed, we we need to substitute all constants with their concrete value.
            preparedProgram = preparedProgram.substituteConstants();
            
            STORM_LOG_DEBUG("Building representation of program :" << std::endl << preparedProgram << std::endl);
                
            // Select the appropriate reward models (after the constants have been substituted).
            std::vector<std::reference_wrapper<storm::prism::RewardModel const>> selectedRewardModels;
                
            // First, we make sure that all selected reward models actually exist.
            for (auto const& rewardModelName : options.rewardModelsToBuild) {
                STORM_LOG_THROW(rewardModelName.empty() || preparedProgram.hasRewardModel(rewardModelName), storm::exceptions::InvalidArgumentException, "Model does not possess a reward model with the name '" << rewardModelName << "'.");
            }
                
            for (auto const& rewardModel : preparedProgram.getRewardModels()) {
                if (options.buildAllRewardModels || options.rewardModelsToBuild.find(rewardModel.getName()) != options.rewardModelsToBuild.end()) {
                    selectedRewardModels.push_back(rewardModel);
                }
            }
            // If no reward model was selected until now and a referenced reward model appears to be unique, we build
            // the only existing reward model (given that no explicit name was given for the referenced reward model).
            if (selectedRewardModels.empty() && preparedProgram.getNumberOfRewardModels() == 1 && options.rewardModelsToBuild.size() == 1 && *options.rewardModelsToBuild.begin() == "") {
                selectedRewardModels.push_back(preparedProgram.getRewardModel(0));
            }
                
            ModelComponents modelComponents = buildModelComponents(preparedProgram, selectedRewardModels, options);
            
            std::shared_ptr<storm::models::sparse::Model<ValueType>> result;
            switch (program.getModelType()) {
                case storm::prism::Program::ModelType::DTMC:
                    result = std::shared_ptr<storm::models::sparse::Model<ValueType>>(new storm::models::sparse::Dtmc<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.rewardModels), std::move(modelComponents.choiceLabeling)));
                    break;
                case storm::prism::Program::ModelType::CTMC:
                    result = std::shared_ptr<storm::models::sparse::Model<ValueType>>(new storm::models::sparse::Ctmc<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.rewardModels), std::move(modelComponents.choiceLabeling)));
                    break;
                case storm::prism::Program::ModelType::MDP:
                    result = std::shared_ptr<storm::models::sparse::Model<ValueType>>(new storm::models::sparse::Mdp<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.rewardModels), std::move(modelComponents.choiceLabeling)));
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error while creating model from probabilistic program: cannot handle this model type.");
                    break;
            }
            
            return result;
        }
        
        template <typename ValueType, typename IndexType>
        void ExplicitPrismModelBuilder<ValueType, IndexType>::unpackStateIntoEvaluator(storm::storage::BitVector const& currentState, VariableInformation const& variableInformation, storm::expressions::ExpressionEvaluator<ValueType>& evaluator) {
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                evaluator.setBooleanValue(booleanVariable.variable, currentState.get(booleanVariable.bitOffset));
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                evaluator.setIntegerValue(integerVariable.variable, currentState.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) + integerVariable.lowerBound);
            }
        }
        
        template <typename ValueType, typename IndexType>
        typename ExplicitPrismModelBuilder<ValueType, IndexType>::CompressedState ExplicitPrismModelBuilder<ValueType, IndexType>::applyUpdate(VariableInformation const& variableInformation, CompressedState const& state, storm::prism::Update const& update, storm::expressions::ExpressionEvaluator<ValueType> const& evaluator) {
            return applyUpdate(variableInformation, state, state, update, evaluator);
        }
        
        template <typename ValueType, typename IndexType>
        typename ExplicitPrismModelBuilder<ValueType, IndexType>::CompressedState ExplicitPrismModelBuilder<ValueType, IndexType>::applyUpdate(VariableInformation const& variableInformation, CompressedState const& state, CompressedState const& baseState, storm::prism::Update const& update, storm::expressions::ExpressionEvaluator<ValueType> const& evaluator) {
            CompressedState newState(state);
            
            auto assignmentIt = update.getAssignments().begin();
            auto assignmentIte = update.getAssignments().end();
            
            // Iterate over all boolean assignments and carry them out.
            auto boolIt = variableInformation.booleanVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getExpression().hasBooleanType(); ++assignmentIt) {
                while (assignmentIt->getVariable() != boolIt->variable) {
                    ++boolIt;
                }
                newState.set(boolIt->bitOffset, evaluator.asBool(assignmentIt->getExpression()));
            }
            
            // Iterate over all integer assignments and carry them out.
            auto integerIt = variableInformation.integerVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getExpression().hasIntegerType(); ++assignmentIt) {
                while (assignmentIt->getVariable() != integerIt->variable) {
                    ++integerIt;
                }
                int_fast64_t assignedValue = evaluator.asInt(assignmentIt->getExpression());
                STORM_LOG_THROW(assignedValue <= integerIt->upperBound, storm::exceptions::WrongFormatException, "The update " << update << " leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << assignmentIt->getVariableName() << "'.");
                newState.setFromInt(integerIt->bitOffset, integerIt->bitWidth, assignedValue - integerIt->lowerBound);
                STORM_LOG_ASSERT(static_cast<int_fast64_t>(newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth)) + integerIt->lowerBound == assignedValue, "Writing to the bit vector bucket failed (read " << newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth) << " but wrote " << assignedValue << ").");
            }
            
            // Check that we processed all assignments.
            STORM_LOG_ASSERT(assignmentIt == assignmentIte, "Not all assignments were consumed.");
            
            return newState;
        }
        
        template <typename ValueType, typename IndexType>
        IndexType ExplicitPrismModelBuilder<ValueType, IndexType>::getOrAddStateIndex(CompressedState const& state, StateInformation& stateInformation, std::queue<storm::storage::BitVector>& stateQueue) {
            uint32_t newIndex = stateInformation.reachableStates.size();
            
            // Check, if the state was already registered.
            std::pair<uint32_t, std::size_t> actualIndexBucketPair = stateInformation.stateStorage.findOrAddAndGetBucket(state, newIndex);
            
            if (actualIndexBucketPair.first == newIndex) {
                stateQueue.push(state);
                stateInformation.reachableStates.push_back(state);
            }
            
            return actualIndexBucketPair.first;
        }
        
        template <typename ValueType, typename IndexType>
        boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> ExplicitPrismModelBuilder<ValueType, IndexType>::getActiveCommandsByActionIndex(storm::prism::Program const& program,storm::expressions::ExpressionEvaluator<ValueType> const& evaluator, uint_fast64_t const& actionIndex) {
            boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> result((std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>()));
            
            // Iterate over all modules.
            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                storm::prism::Module const& module = program.getModule(i);
                
                // If the module has no command labeled with the given action, we can skip this module.
                if (!module.hasActionIndex(actionIndex)) {
                    continue;
                }
                
                std::set<uint_fast64_t> const& commandIndices = module.getCommandIndicesByActionIndex(actionIndex);
                
                // If the module contains the action, but there is no command in the module that is labeled with
                // this action, we don't have any feasible command combinations.
                if (commandIndices.empty()) {
                    return boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>>();
                }
                
                std::vector<std::reference_wrapper<storm::prism::Command const>> commands;
                
                // Look up commands by their indices and add them if the guard evaluates to true in the given state.
                for (uint_fast64_t commandIndex : commandIndices) {
                    storm::prism::Command const& command = module.getCommand(commandIndex);
                    if (evaluator.asBool(command.getGuardExpression())) {
                        commands.push_back(command);
                    }
                }
                
                // If there was no enabled command although the module has some command with the required action label,
                // we must not return anything.
                if (commands.size() == 0) {
                    return boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>>();
                }
                
                result.get().push_back(std::move(commands));
            }
            
            return result;
        }
        
        template <typename ValueType, typename IndexType>
        std::vector<Choice<ValueType>> ExplicitPrismModelBuilder<ValueType, IndexType>::getUnlabeledTransitions(storm::prism::Program const& program, bool discreteTimeModel, StateInformation& stateInformation, VariableInformation const& variableInformation, storm::storage::BitVector const& currentState, bool choiceLabeling, storm::expressions::ExpressionEvaluator<ValueType> const& evaluator, std::queue<storm::storage::BitVector>& stateQueue, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            std::vector<Choice<ValueType>> result;
            
            // Iterate over all modules.
            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                storm::prism::Module const& module = program.getModule(i);
                
                // Iterate over all commands.
                for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
                    storm::prism::Command const& command = module.getCommand(j);
                    
                    // Only consider unlabeled commands.
                    if (command.isLabeled()) continue;
                    
                    // Skip the command, if it is not enabled.
                    if (!evaluator.asBool(command.getGuardExpression())) {
                        continue;
                    }
                    
                    result.push_back(Choice<ValueType>(0, choiceLabeling));
                    Choice<ValueType>& choice = result.back();
                    
                    // Remember the command labels only if we were asked to.
                    if (choiceLabeling) {
                        choice.addChoiceLabel(command.getGlobalIndex());
                    }
                    
                    // Iterate over all updates of the current command.
                    ValueType probabilitySum = storm::utility::zero<ValueType>();
                    for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
                        storm::prism::Update const& update = command.getUpdate(k);
                        
                        // Obtain target state index and add it to the list of known states. If it has not yet been
                        // seen, we also add it to the set of states that have yet to be explored.
                        uint32_t stateIndex = getOrAddStateIndex(applyUpdate(variableInformation, currentState, update, evaluator), stateInformation, stateQueue);
                        
                        // Update the choice by adding the probability/target state to it.
                        ValueType probability = evaluator.asRational(update.getLikelihoodExpression());
                        choice.addProbability(stateIndex, probability);
                        probabilitySum += probability;
                    }

                    // Check that the resulting distribution is in fact a distribution.
                    STORM_LOG_THROW(!discreteTimeModel || comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Probabilities do not sum to one for command '" << command << "' (actually sum to " << probabilitySum << ").");
                }
            }
            
            return result;
        }
        
        template <typename ValueType, typename IndexType>
        std::vector<Choice<ValueType>> ExplicitPrismModelBuilder<ValueType, IndexType>::getLabeledTransitions(storm::prism::Program const& program, bool discreteTimeModel, StateInformation& stateInformation, VariableInformation const& variableInformation, storm::storage::BitVector const& currentState, bool choiceLabeling, storm::expressions::ExpressionEvaluator<ValueType> const& evaluator, std::queue<storm::storage::BitVector>& stateQueue, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            std::vector<Choice<ValueType>> result;
            
            for (uint_fast64_t actionIndex : program.getSynchronizingActionIndices()) {
                boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> optionalActiveCommandLists = getActiveCommandsByActionIndex(program, evaluator, actionIndex);
                
                // Only process this action label, if there is at least one feasible solution.
                if (optionalActiveCommandLists) {
                    std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>> const& activeCommandList = optionalActiveCommandLists.get();
                    std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>::const_iterator> iteratorList(activeCommandList.size());
                    
                    // Initialize the list of iterators.
                    for (size_t i = 0; i < activeCommandList.size(); ++i) {
                        iteratorList[i] = activeCommandList[i].cbegin();
                    }
                    
                    // As long as there is one feasible combination of commands, keep on expanding it.
                    bool done = false;
                    while (!done) {
                        std::unordered_map<CompressedState, ValueType>* currentTargetStates = new std::unordered_map<CompressedState, ValueType>();
                        std::unordered_map<CompressedState, ValueType>* newTargetStates = new std::unordered_map<CompressedState, ValueType>();
                        currentTargetStates->emplace(currentState, storm::utility::one<ValueType>());
                        
                        for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                            storm::prism::Command const& command = *iteratorList[i];
                            
                            for (uint_fast64_t j = 0; j < command.getNumberOfUpdates(); ++j) {
                                storm::prism::Update const& update = command.getUpdate(j);
                                
                                for (auto const& stateProbabilityPair : *currentTargetStates) {
                                    // Compute the new state under the current update and add it to the set of new target states.
                                    CompressedState newTargetState = applyUpdate(variableInformation, stateProbabilityPair.first, currentState, update, evaluator);
                                    newTargetStates->emplace(newTargetState, stateProbabilityPair.second * evaluator.asRational(update.getLikelihoodExpression()));
                                }
                            }
                            
                            // If there is one more command to come, shift the target states one time step back.
                            if (i < iteratorList.size() - 1) {
                                delete currentTargetStates;
                                currentTargetStates = newTargetStates;
                                newTargetStates = new std::unordered_map<CompressedState, ValueType>();
                            }
                        }
                        
                        // At this point, we applied all commands of the current command combination and newTargetStates
                        // contains all target states and their respective probabilities. That means we are now ready to
                        // add the choice to the list of transitions.
                        result.push_back(Choice<ValueType>(actionIndex, choiceLabeling));
                        
                        // Now create the actual distribution.
                        Choice<ValueType>& choice = result.back();
                        
                        // Remember the command labels only if we were asked to.
                        if (choiceLabeling) {
                            // Add the labels of all commands to this choice.
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                choice.addChoiceLabel(iteratorList[i]->get().getGlobalIndex());
                            }
                        }
                        
                        ValueType probabilitySum = storm::utility::zero<ValueType>();
                        for (auto const& stateProbabilityPair : *newTargetStates) {
                            uint32_t actualIndex = getOrAddStateIndex(stateProbabilityPair.first, stateInformation, stateQueue);
                            choice.addProbability(actualIndex, stateProbabilityPair.second);
                            probabilitySum += stateProbabilityPair.second;
                        }
                        
                        // Check that the resulting distribution is in fact a distribution.
                        STORM_LOG_THROW(!discreteTimeModel || comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Sum of update probabilities do not some to one for some command (actually sum to " << probabilitySum << ").");
                        
                        // Dispose of the temporary maps.
                        delete currentTargetStates;
                        delete newTargetStates;
                        
                        // Now, check whether there is one more command combination to consider.
                        bool movedIterator = false;
                        for (int_fast64_t j = iteratorList.size() - 1; j >= 0; --j) {
                            ++iteratorList[j];
                            if (iteratorList[j] != activeCommandList[j].end()) {
                                movedIterator = true;
                            } else {
                                // Reset the iterator to the beginning of the list.
                                iteratorList[j] = activeCommandList[j].begin();
                            }
                        }
                        
                        done = !movedIterator;
                    }
                }
            }
            
            return result;
        }

        template <typename ValueType, typename IndexType>
        boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> ExplicitPrismModelBuilder<ValueType, IndexType>::buildMatrices(storm::prism::Program const& program, VariableInformation const& variableInformation, std::vector<std::reference_wrapper<storm::prism::RewardModel const>> const& selectedRewardModels, StateInformation& stateInformation, bool commandLabels, bool deterministicModel, bool discreteTimeModel, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<RewardModelBuilder<ValueType>>& rewardModelBuilders) {
            // Create choice labels, if requested,
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabels;
            if (commandLabels) {
                choiceLabels = std::vector<boost::container::flat_set<uint_fast64_t>>();
            }
            
            // A comparator that can be used to check whether we actually have distributions.
            storm::utility::ConstantsComparator<ValueType> comparator;
            
            // Initialize a queue and insert the initial state.
            std::queue<storm::storage::BitVector> stateQueue;
            CompressedState initialState(stateInformation.bitsPerState);
            
            // We need to initialize the values of the variables to their initial value.
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                initialState.set(booleanVariable.bitOffset, booleanVariable.initialValue);
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                initialState.setFromInt(integerVariable.bitOffset, integerVariable.bitWidth, static_cast<uint_fast64_t>(integerVariable.initialValue - integerVariable.lowerBound));
            }
            
            // At this point, we determine whether there are reward models with state-action rewards, because we might
            // want to know that quickly later on.
            bool hasStateActionRewards = false;
            for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt) {
                if (rewardModelIt->get().hasStateActionRewards()) {
                    hasStateActionRewards = true;
                    break;
                }
            }
            
            // Insert the initial state in the global state to index mapping and state queue.
            uint32_t stateIndex = getOrAddStateIndex(initialState, stateInformation, stateQueue);
            stateInformation.initialStateIndices.push_back(stateIndex);
            
            // Now explore the current state until there is no more reachable state.
            uint_fast64_t currentRow = 0;
            
            // The evaluator used to determine the truth value of guards and predicates in the *current* state.
            storm::expressions::ExpressionEvaluator<ValueType> evaluator(program.getManager());
            
            // Perform a BFS through the model.
            while (!stateQueue.empty()) {
                // Get the current state and unpack it.
                storm::storage::BitVector currentState = stateQueue.front();
                stateQueue.pop();
                IndexType stateIndex = stateInformation.stateStorage.getValue(currentState);
                unpackStateIntoEvaluator(currentState, variableInformation, evaluator);
                
                // Retrieve all choices for the current state.
                std::vector<Choice<ValueType>> allUnlabeledChoices = getUnlabeledTransitions(program, discreteTimeModel, stateInformation, variableInformation, currentState, commandLabels, evaluator, stateQueue, comparator);
                std::vector<Choice<ValueType>> allLabeledChoices = getLabeledTransitions(program, discreteTimeModel, stateInformation, variableInformation, currentState, commandLabels, evaluator, stateQueue, comparator);
                
                uint_fast64_t totalNumberOfChoices = allUnlabeledChoices.size() + allLabeledChoices.size();
                
                // If the current state does not have a single choice, we equip it with a self-loop if that was
                // requested and issue an error otherwise.
                if (totalNumberOfChoices == 0) {
                    if (!storm::settings::generalSettings().isDontFixDeadlocksSet()) {
                        if (commandLabels) {
                            // Insert empty choice labeling for added self-loop transitions.
                            choiceLabels.get().push_back(boost::container::flat_set<uint_fast64_t>());
                        }
                        if (!deterministicModel) {
                            transitionMatrixBuilder.newRowGroup(currentRow);
                        }
                        
                        transitionMatrixBuilder.addNextValue(currentRow, stateIndex, storm::utility::one<ValueType>());
                        
                        auto builderIt = rewardModelBuilders.begin();
                        for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                            if (rewardModelIt->get().hasStateRewards()) {
                                builderIt->stateRewardVector.push_back(storm::utility::zero<ValueType>());
                            }
                            
                            if (rewardModelIt->get().hasStateActionRewards()) {
                                builderIt->stateActionRewardVector.push_back(storm::utility::zero<ValueType>());
                            }
                        }
                        
                        ++currentRow;
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.");
                    }
                } else if (totalNumberOfChoices == 1) {
                    if (!deterministicModel) {
                        transitionMatrixBuilder.newRowGroup(currentRow);
                    }
                    
                    bool labeledChoice = allUnlabeledChoices.empty() ? true : false;
                    Choice<ValueType> const& globalChoice = labeledChoice ? allLabeledChoices.front() : allUnlabeledChoices.front();
                    
                    auto builderIt = rewardModelBuilders.begin();
                    for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                        if (rewardModelIt->get().hasStateRewards()) {
                            builderIt->stateRewardVector.push_back(storm::utility::zero<ValueType>());
                            for (auto const& stateReward : rewardModelIt->get().getStateRewards()) {
                                if (evaluator.asBool(stateReward.getStatePredicateExpression())) {
                                    builderIt->stateRewardVector.back() += ValueType(evaluator.asRational(stateReward.getRewardValueExpression()));
                                }
                            }
                        }
                        
                        if (rewardModelIt->get().hasStateActionRewards()) {
                            builderIt->stateActionRewardVector.push_back(storm::utility::zero<ValueType>());
                            for (auto const& stateActionReward : rewardModelIt->get().getStateActionRewards()) {
                                if ((labeledChoice && stateActionReward.isLabeled() && stateActionReward.getActionIndex() == globalChoice.getActionIndex()) || (!labeledChoice && !stateActionReward.isLabeled())) {
                                    if (evaluator.asBool(stateActionReward.getStatePredicateExpression())) {
                                        builderIt->stateActionRewardVector.back() += ValueType(evaluator.asRational(stateActionReward.getRewardValueExpression()));
                                    }
                                }
                            }
                        }
                    }
                    
                    for (auto const& stateProbabilityPair : globalChoice) {
                        transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                    }
                    
                    if (commandLabels) {
                        // Now add the resulting distribution as the only choice of the current state.
                        choiceLabels.get().push_back(globalChoice.getChoiceLabels());
                    }
                    
                    ++currentRow;
                } else {
                    // Then, based on whether the model is deterministic or not, either add the choices individually
                    // or compose them to one choice.
                    if (deterministicModel) {
                        Choice<ValueType> globalChoice;

                        // We need to prepare the entries of those vectors that are going to be used.
                        auto builderIt = rewardModelBuilders.begin();
                        for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                            if (rewardModelIt->get().hasStateRewards()) {
                                builderIt->stateRewardVector.push_back(storm::utility::zero<ValueType>());
                                for (auto const& stateReward : rewardModelIt->get().getStateRewards()) {
                                    if (evaluator.asBool(stateReward.getStatePredicateExpression())) {
                                        builderIt->stateRewardVector.back() += ValueType(evaluator.asRational(stateReward.getRewardValueExpression()));
                                    }
                                }
                            }
                            
                            if (rewardModelIt->get().hasStateActionRewards()) {
                                builderIt->stateActionRewardVector.push_back(storm::utility::zero<ValueType>());
                            }
                        }
                        
                        // If there is one state-action reward model, we need to scale the rewards according to the
                        // multiple choices.
                        ValueType totalExitMass = storm::utility::zero<ValueType>();
                        if (hasStateActionRewards) {
                            if (discreteTimeModel) {
                                totalExitMass = static_cast<ValueType>(totalNumberOfChoices);
                            } else {
                                // In the CTMC, we need to compute the exit rate of the state here, sin
                                for (auto const& choice : allUnlabeledChoices) {
                                    totalExitMass += choice.getTotalMass();
                                }
                                for (auto const& choice : allLabeledChoices) {
                                    totalExitMass += choice.getTotalMass();
                                }
                            }
                        }
                        
                        // Combine all the choices and scale them with the total number of choices of the current state.
                        for (auto const& choice : allUnlabeledChoices) {
                            if (commandLabels) {
                                globalChoice.addChoiceLabels(choice.getChoiceLabels());
                            }
                            
                            auto builderIt = rewardModelBuilders.begin();
                            for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                                if (rewardModelIt->get().hasStateActionRewards()) {
                                    for (auto const& stateActionReward : rewardModelIt->get().getStateActionRewards()) {
                                        if (!stateActionReward.isLabeled()) {
                                            if (evaluator.asBool(stateActionReward.getStatePredicateExpression())) {
                                                builderIt->stateActionRewardVector.back() += ValueType(evaluator.asRational(stateActionReward.getRewardValueExpression())) * choice.getTotalMass() / totalExitMass;
                                            }
                                        }
                                    }
                                }
                            }
                            
                            for (auto const& stateProbabilityPair : choice) {
                                if (discreteTimeModel) {
                                    globalChoice.getOrAddEntry(stateProbabilityPair.first) += stateProbabilityPair.second / totalNumberOfChoices;
                                } else {
                                    globalChoice.getOrAddEntry(stateProbabilityPair.first) += stateProbabilityPair.second;
                                }
                            }
                        }
                        for (auto const& choice : allLabeledChoices) {
                            if (commandLabels) {
                                globalChoice.addChoiceLabels(choice.getChoiceLabels());
                            }
                            
                            auto builderIt = rewardModelBuilders.begin();
                            for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                                if (rewardModelIt->get().hasStateActionRewards()) {
                                    for (auto const& stateActionReward : rewardModelIt->get().getStateActionRewards()) {
                                        if (stateActionReward.isLabeled() && stateActionReward.getActionIndex() == choice.getActionIndex()) {
                                            if (evaluator.asBool(stateActionReward.getStatePredicateExpression())) {
                                                builderIt->stateActionRewardVector.back() += ValueType(evaluator.asRational(stateActionReward.getRewardValueExpression())) * choice.getTotalMass() / totalExitMass;
                                            }
                                        }
                                    }
                                }
                            }
                            
                            for (auto const& stateProbabilityPair : choice) {
                                if (discreteTimeModel) {
                                    globalChoice.getOrAddEntry(stateProbabilityPair.first) += stateProbabilityPair.second / totalNumberOfChoices;
                                } else {
                                    globalChoice.getOrAddEntry(stateProbabilityPair.first) += stateProbabilityPair.second;
                                }
                            }
                        }
                        
                        
                        if (commandLabels) {
                            // Now add the resulting distribution as the only choice of the current state.
                            choiceLabels.get().push_back(globalChoice.getChoiceLabels());
                        }
                        
                        for (auto const& stateProbabilityPair : globalChoice) {
                            transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                        }
                        
                        ++currentRow;
                    } else {
                        // If the model is nondeterministic, we add all choices individually.
                        transitionMatrixBuilder.newRowGroup(currentRow);

                        auto builderIt = rewardModelBuilders.begin();
                        for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                            if (rewardModelIt->get().hasStateRewards()) {
                                builderIt->stateRewardVector.push_back(storm::utility::zero<ValueType>());
                                
                                for (auto const& stateReward : rewardModelIt->get().getStateRewards()) {
                                    if (evaluator.asBool(stateReward.getStatePredicateExpression())) {
                                        builderIt->stateRewardVector.back() += ValueType(evaluator.asRational(stateReward.getRewardValueExpression()));
                                    }
                                }
                            }
                        }
                        
                        // First, process all unlabeled choices.
                        for (auto const& choice : allUnlabeledChoices) {
                            std::map<uint_fast64_t, ValueType> stateToRewardMap;
                            if (commandLabels) {
                                choiceLabels.get().emplace_back(std::move(choice.getChoiceLabels()));
                            }
                            
                            auto builderIt = rewardModelBuilders.begin();
                            for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                                if (rewardModelIt->get().hasStateActionRewards()) {
                                    builderIt->stateActionRewardVector.push_back(storm::utility::zero<ValueType>());
                                    for (auto const& stateActionReward : rewardModelIt->get().getStateActionRewards()) {
                                        if (!stateActionReward.isLabeled()) {
                                            if (evaluator.asBool(stateActionReward.getStatePredicateExpression())) {
                                                builderIt->stateActionRewardVector.back() += ValueType(evaluator.asRational(stateActionReward.getRewardValueExpression()));
                                            }
                                        }
                                    }
                                }
                            }
                            
                            for (auto const& stateProbabilityPair : choice) {
                                transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                            }
                            
                            ++currentRow;
                        }
                        
                        // Then, process all labeled choices.
                        for (auto const& choice : allLabeledChoices) {
                            std::map<uint_fast64_t, ValueType> stateToRewardMap;
                            if (commandLabels) {
                                choiceLabels.get().emplace_back(std::move(choice.getChoiceLabels()));
                            }
                            
                            auto builderIt = rewardModelBuilders.begin();
                            for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                                if (rewardModelIt->get().hasStateActionRewards()) {
                                    builderIt->stateActionRewardVector.push_back(storm::utility::zero<ValueType>());
                                    for (auto const& stateActionReward : rewardModelIt->get().getStateActionRewards()) {
                                        if (stateActionReward.isLabeled() && stateActionReward.getActionIndex() == choice.getActionIndex()) {
                                            if (evaluator.asBool(stateActionReward.getStatePredicateExpression())) {
                                                builderIt->stateActionRewardVector.back() += ValueType(evaluator.asRational(stateActionReward.getRewardValueExpression()));
                                            }
                                        }
                                    }
                                }
                            }
                            
                            for (auto const& stateProbabilityPair : choice) {
                                transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                            }
                            
                            ++currentRow;
                        }
                    }
                }
            }
            
            return choiceLabels;
        }
        
        template <typename ValueType, typename IndexType>
        typename ExplicitPrismModelBuilder<ValueType, IndexType>::ModelComponents ExplicitPrismModelBuilder<ValueType, IndexType>::buildModelComponents(storm::prism::Program const& program, std::vector<std::reference_wrapper<storm::prism::RewardModel const>> const& selectedRewardModels, Options const& options) {
            ModelComponents modelComponents;
            
            uint_fast64_t bitOffset = 0;
            VariableInformation variableInformation;
            for (auto const& booleanVariable : program.getGlobalBooleanVariables()) {
                variableInformation.booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), booleanVariable.getInitialValueExpression().evaluateAsBool(), bitOffset);
                ++bitOffset;
                variableInformation.booleanVariableToIndexMap[booleanVariable.getExpressionVariable()] = variableInformation.booleanVariables.size() - 1;
            }
            for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                variableInformation.integerVariables.emplace_back(integerVariable.getExpressionVariable(), integerVariable.getInitialValueExpression().evaluateAsInt(), lowerBound, upperBound, bitOffset, bitwidth);
                bitOffset += bitwidth;
                variableInformation.integerVariableToIndexMap[integerVariable.getExpressionVariable()] = variableInformation.integerVariables.size() - 1;
            }
            for (auto const& module : program.getModules()) {
                for (auto const& booleanVariable : module.getBooleanVariables()) {
                    variableInformation.booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), booleanVariable.getInitialValueExpression().evaluateAsBool(), bitOffset);
                    ++bitOffset;
                    variableInformation.booleanVariableToIndexMap[booleanVariable.getExpressionVariable()] = variableInformation.booleanVariables.size() - 1;
                }
                for (auto const& integerVariable : module.getIntegerVariables()) {
                    int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                    int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    variableInformation.integerVariables.emplace_back(integerVariable.getExpressionVariable(), integerVariable.getInitialValueExpression().evaluateAsInt(), lowerBound, upperBound, bitOffset, bitwidth);
                    bitOffset += bitwidth;
                    variableInformation.integerVariableToIndexMap[integerVariable.getExpressionVariable()] = variableInformation.integerVariables.size() - 1;
                }
            }
            
            // Create the structure for storing the reachable state space.
            uint64_t bitsPerState = ((bitOffset / 64) + 1) * 64;
            StateInformation stateInformation(bitsPerState);
            
            // Determine whether we have to combine different choices to one or whether this model can have more than
            // one choice per state.
            bool deterministicModel = program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::CTMC;
            bool discreteTimeModel = program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::MDP;
            
            // Prepare the transition matrix builder and the reward model builders.
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);
            std::vector<RewardModelBuilder<ValueType>> rewardModelBuilders;
            for (auto const& rewardModel : selectedRewardModels) {
                rewardModelBuilders.emplace_back(deterministicModel, rewardModel.get().hasStateRewards(), rewardModel.get().hasStateActionRewards(), rewardModel.get().hasTransitionRewards());
            }
            
            modelComponents.choiceLabeling = buildMatrices(program, variableInformation, selectedRewardModels, stateInformation, options.buildCommandLabels, deterministicModel, discreteTimeModel, transitionMatrixBuilder, rewardModelBuilders);
            
            modelComponents.transitionMatrix = transitionMatrixBuilder.build();
            
            // Now finalize all reward models.
            auto builderIt = rewardModelBuilders.begin();
            for (auto rewardModelIt = selectedRewardModels.begin(), rewardModelIte = selectedRewardModels.end(); rewardModelIt != rewardModelIte; ++rewardModelIt, ++builderIt) {
                modelComponents.rewardModels.emplace(rewardModelIt->get().getName(), builderIt->build(modelComponents.transitionMatrix.getRowCount(), modelComponents.transitionMatrix.getColumnCount(), modelComponents.transitionMatrix.getRowGroupCount()));
            }
            
            // Finally, build the state labeling.
            modelComponents.stateLabeling = buildStateLabeling(program, variableInformation, stateInformation);
            
            return modelComponents;
        }
        
        template <typename ValueType, typename IndexType>
        storm::models::sparse::StateLabeling ExplicitPrismModelBuilder<ValueType, IndexType>::buildStateLabeling(storm::prism::Program const& program, VariableInformation const& variableInformation, StateInformation const& stateInformation) {
            storm::expressions::ExpressionEvaluator<ValueType> evaluator(program.getManager());
            
            std::vector<storm::prism::Label> const& labels = program.getLabels();
            
            storm::models::sparse::StateLabeling result(stateInformation.reachableStates.size());
            
            // Initialize labeling.
            for (auto const& label : labels) {
                result.addLabel(label.getName());
            }
            for (uint_fast64_t index = 0; index < stateInformation.reachableStates.size(); index++) {
                unpackStateIntoEvaluator(stateInformation.reachableStates[index], variableInformation, evaluator);
                for (auto const& label : labels) {
                    // Add label to state, if the corresponding expression is true.
                    if (evaluator.asBool(label.getStatePredicateExpression())) {
                        result.addLabelToState(label.getName(), index);
                    }
                }
            }
            
            // Also label the initial state with the special label "init".
            result.addLabel("init");
            for (auto index : stateInformation.initialStateIndices) {
                result.addLabelToState("init", index);
            }
            
            return result;
        }
        
        // Explicitly instantiate the class.
        template class ExplicitPrismModelBuilder<double, uint32_t>;
        
#ifdef STORM_HAVE_CARL
        template class ExplicitPrismModelBuilder<RationalFunction, uint32_t>;
#endif
    }
}