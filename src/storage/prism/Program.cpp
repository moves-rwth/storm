#include "src/storage/prism/Program.h"

#include <algorithm>
#include <sstream>
#include <boost/algorithm/string/join.hpp>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/solver/SmtSolver.h"

namespace storm {
    namespace prism {
        Program::Program(std::shared_ptr<storm::expressions::ExpressionManager> manager, ModelType modelType, std::vector<Constant> const& constants, std::vector<BooleanVariable> const& globalBooleanVariables, std::vector<IntegerVariable> const& globalIntegerVariables, std::vector<Formula> const& formulas, std::vector<Module> const& modules, std::map<std::string, uint_fast64_t> const& actionToIndexMap, std::vector<RewardModel> const& rewardModels, bool fixInitialConstruct, storm::prism::InitialConstruct const& initialConstruct, std::vector<Label> const& labels, std::string const& filename, uint_fast64_t lineNumber, bool finalModel) 
        : LocatedInformation(filename, lineNumber), manager(manager),
            modelType(modelType), constants(constants), constantToIndexMap(),
            globalBooleanVariables(globalBooleanVariables), globalBooleanVariableToIndexMap(),
            globalIntegerVariables(globalIntegerVariables), globalIntegerVariableToIndexMap(), 
            formulas(formulas), formulaToIndexMap(), modules(modules), moduleToIndexMap(), 
            rewardModels(rewardModels), rewardModelToIndexMap(), initialConstruct(initialConstruct), 
            labels(labels), labelToIndexMap(), actionToIndexMap(actionToIndexMap), indexToActionMap(), actions(),
            synchronizingActionIndices(), actionIndicesToModuleIndexMap(), variableToModuleIndexMap()
        {

            // Start by creating the necessary mappings from the given ones.
            this->createMappings();

            // Create a new initial construct if the corresponding flag was set.
            if (fixInitialConstruct) {
                storm::expressions::Expression newInitialExpression = manager->boolean(true);
                
                for (auto const& booleanVariable : this->getGlobalBooleanVariables()) {
                    newInitialExpression = newInitialExpression && storm::expressions::iff(booleanVariable.getExpression(), booleanVariable.getInitialValueExpression());
                }
                for (auto const& integerVariable : this->getGlobalIntegerVariables()) {
                    newInitialExpression = newInitialExpression && integerVariable.getExpression() == integerVariable.getInitialValueExpression();
                }
                for (auto const& module : this->getModules()) {
                    for (auto const& booleanVariable : module.getBooleanVariables()) {
                        newInitialExpression = newInitialExpression && storm::expressions::iff(booleanVariable.getExpression(), booleanVariable.getInitialValueExpression());
                    }
                    for (auto const& integerVariable : module.getIntegerVariables()) {
                        newInitialExpression = newInitialExpression && integerVariable.getExpression() == integerVariable.getInitialValueExpression();
                    }
                }
                this->initialConstruct = storm::prism::InitialConstruct(newInitialExpression, this->getInitialConstruct().getFilename(), this->getInitialConstruct().getLineNumber());
            }

            if (finalModel) {

                // If the model is supposed to be a CTMC, but contains probabilistic commands, we transform them to Markovian
                // commands and issue a warning.
                if (modelType == storm::prism::Program::ModelType::CTMC && storm::settings::generalSettings().isPrismCompatibilityEnabled()) {
                    bool hasProbabilisticCommands = false;
                    for (auto& module : this->modules) {
                        for (auto& command : module.getCommands()) {
                            if (!command.isMarkovian()) {
                                command.setMarkovian(true);
                                hasProbabilisticCommands = true;
                            }
                        }
                    }
                    STORM_LOG_WARN_COND(!hasProbabilisticCommands, "The input model is a CTMC, but uses probabilistic commands like they are used in PRISM. Consider rewriting the commands to use Markovian commands instead.");
                }
                // Then check the validity.
                this->checkValidity(Program::ValidityCheckLevel::VALIDINPUT);
            }
        }
    
        Program::ModelType Program::getModelType() const {
            return modelType;
        }
        
        bool Program::hasUndefinedConstants() const {
            for (auto const& constant : this->getConstants()) {
                if (!constant.isDefined()) {
                    return true;
                }
            }
            return false;
        }
        
        bool Program::hasUndefinedConstantsOnlyInUpdateProbabilitiesAndRewards() const {
            if (!this->hasUndefinedConstants()) {
                return true;
            }
            
            // Gather the variables of all undefined constants.
            std::set<storm::expressions::Variable> undefinedConstantVariables;
            for (auto const& constant : this->getConstants()) {
                if (!constant.isDefined()) {
                    undefinedConstantVariables.insert(constant.getExpressionVariable());
                }
            }
            
            // Now it remains to check that the intersection of the variables used in the program with the undefined
            // constants' variables is empty (except for the update probabilities).
            
            // Start by checking the defining expressions of all defined constants.
            for (auto const& constant : this->getConstants()) {
                if (constant.isDefined()) {
                    if (constant.getExpression().containsVariable(undefinedConstantVariables)) {
                        return false;
                    }
                }
            }
            
            // Now check initial value expressions of global variables.
            for (auto const& booleanVariable : this->getGlobalBooleanVariables()) {
                if (booleanVariable.getInitialValueExpression().containsVariable(undefinedConstantVariables)) {
                    return false;
                }
            }
            for (auto const& integerVariable : this->getGlobalIntegerVariables()) {
                if (integerVariable.getInitialValueExpression().containsVariable(undefinedConstantVariables)) {
                    return false;
                }
                if (integerVariable.getLowerBoundExpression().containsVariable(undefinedConstantVariables)) {
                    return false;
                }
                if (integerVariable.getUpperBoundExpression().containsVariable(undefinedConstantVariables)) {
                    return false;
                }
            }
            
            // Then check the formulas.
            for (auto const& formula : this->getFormulas()) {
                if (formula.getExpression().containsVariable(undefinedConstantVariables)) {
                    return false;
                }
            }
            
            // Proceed by checking each of the modules.
            for (auto const& module : this->getModules()) {
                module.containsVariablesOnlyInUpdateProbabilities(undefinedConstantVariables);
            }

            // Check the reward models.
            for (auto const& rewardModel : this->getRewardModels()) {
                rewardModel.containsVariablesOnlyInRewardValueExpressions(undefinedConstantVariables);
            }
                     
            // Initial construct.
            if (this->getInitialConstruct().getInitialStatesExpression().containsVariable(undefinedConstantVariables)) {
                return false;
            }
            
            // Labels.
            for (auto const& label : this->getLabels()) {
                if (label.getStatePredicateExpression().containsVariable(undefinedConstantVariables)) {
                    return false;
                }
            }
            
            return true;
        }
        
        std::vector<std::reference_wrapper<storm::prism::Constant const>> Program::getUndefinedConstants() const {
            std::vector<std::reference_wrapper<storm::prism::Constant const>> result;
            for (auto const& constant : this->getConstants()) {
                if (!constant.isDefined()) {
                    result.push_back(constant);
                }
            }
            return result;
        }
        
        bool Program::hasConstant(std::string const& constantName) const {
            return this->constantToIndexMap.find(constantName) != this->constantToIndexMap.end();
        }
        
        Constant const& Program::getConstant(std::string const& constantName) const {
            auto const& constantIndexPair = this->constantToIndexMap.find(constantName);
            return this->getConstants()[constantIndexPair->second];
        }
        
        std::vector<Constant> const& Program::getConstants() const {
            return this->constants;
        }
        
        std::map<storm::expressions::Variable, storm::expressions::Expression> Program::getConstantsSubstitution() const {
            std::map<storm::expressions::Variable, storm::expressions::Expression> constantsSubstitution;
            for (auto const& constant : this->getConstants()) {
                if (constant.isDefined()) {
                    constantsSubstitution.emplace(constant.getExpressionVariable(), constant.getExpression());
                }
            }
            return constantsSubstitution;
        }

    
        std::size_t Program::getNumberOfConstants() const {
            return this->getConstants().size();
        }

        std::vector<BooleanVariable> const& Program::getGlobalBooleanVariables() const {
            return this->globalBooleanVariables;
        }
        
        std::vector<IntegerVariable> const& Program::getGlobalIntegerVariables() const {
            return this->globalIntegerVariables;
        }
        
        bool Program::globalBooleanVariableExists(std::string const& variableName) const {
            return this->globalBooleanVariableToIndexMap.count(variableName) > 0;
        }
        
        bool Program::globalIntegerVariableExists(std::string const& variableName) const {
            return this->globalIntegerVariableToIndexMap.count(variableName) > 0;
        }
        
        

        BooleanVariable const& Program::getGlobalBooleanVariable(std::string const& variableName) const {
            auto const& nameIndexPair = this->globalBooleanVariableToIndexMap.find(variableName);
            STORM_LOG_THROW(nameIndexPair != this->globalBooleanVariableToIndexMap.end(), storm::exceptions::OutOfRangeException, "Unknown boolean variable '" << variableName << "'.");
            return this->getGlobalBooleanVariables()[nameIndexPair->second];
        }

        IntegerVariable const& Program::getGlobalIntegerVariable(std::string const& variableName) const {
            auto const& nameIndexPair = this->globalIntegerVariableToIndexMap.find(variableName);
            STORM_LOG_THROW(nameIndexPair != this->globalIntegerVariableToIndexMap.end(), storm::exceptions::OutOfRangeException, "Unknown integer variable '" << variableName << "'.");
            return this->getGlobalIntegerVariables()[nameIndexPair->second];
        }
        
        std::size_t Program::getNumberOfGlobalBooleanVariables() const {
            return this->getGlobalBooleanVariables().size();
        }
        
        std::size_t Program::getNumberOfGlobalIntegerVariables() const {
            return this->getGlobalIntegerVariables().size();
        }
        
        std::vector<Formula> const& Program::getFormulas() const {
            return this->formulas;
        }
        
        std::size_t Program::getNumberOfFormulas() const {
            return this->getFormulas().size();
        }
        
        std::size_t Program::getNumberOfModules() const {
            return this->getModules().size();
        }
        
        storm::prism::Module const& Program::getModule(uint_fast64_t index) const {
            return this->modules[index];
        }
        
        Module const& Program::getModule(std::string const& moduleName) const {
            auto const& nameIndexPair = this->moduleToIndexMap.find(moduleName);
            STORM_LOG_THROW(nameIndexPair != this->moduleToIndexMap.end(), storm::exceptions::OutOfRangeException, "Unknown module '" << moduleName << "'.");
            return this->getModules()[nameIndexPair->second];
        }
        
        std::vector<storm::prism::Module> const& Program::getModules() const {
            return this->modules;
        }
        
        std::map<std::string, uint_fast64_t> const& Program::getActionNameToIndexMapping() const {
            return actionToIndexMap;
        }
        
        storm::prism::InitialConstruct const& Program::getInitialConstruct() const {
            return this->initialConstruct;
        }
        
        std::set<std::string> const& Program::getActions() const {
            return this->actions;
        }
        
        std::set<uint_fast64_t> const& Program::getSynchronizingActionIndices() const {
            return this->synchronizingActionIndices;
        }
        
        std::string const& Program::getActionName(uint_fast64_t actionIndex) const {
            auto const& indexNamePair = this->indexToActionMap.find(actionIndex);
            STORM_LOG_THROW(indexNamePair != this->indexToActionMap.end(), storm::exceptions::InvalidArgumentException, "Unknown action index " << actionIndex << ".");
            return indexNamePair->second;
        }
        
        std::set<uint_fast64_t> const& Program::getModuleIndicesByAction(std::string const& action) const {
            auto const& nameIndexPair = this->actionToIndexMap.find(action);
            STORM_LOG_THROW(nameIndexPair != this->actionToIndexMap.end(), storm::exceptions::OutOfRangeException, "Action name '" << action << "' does not exist.");
            return this->getModuleIndicesByActionIndex(nameIndexPair->second);
        }
        
        std::set<uint_fast64_t> const& Program::getModuleIndicesByActionIndex(uint_fast64_t actionIndex) const {
            auto const& actionModuleSetPair = this->actionIndicesToModuleIndexMap.find(actionIndex);
            STORM_LOG_THROW(actionModuleSetPair != this->actionIndicesToModuleIndexMap.end(), storm::exceptions::OutOfRangeException, "Action name '" << actionIndex << "' does not exist.");
            return actionModuleSetPair->second;
        }
        
        uint_fast64_t Program::getModuleIndexByVariable(std::string const& variableName) const {
            auto const& variableNameToModuleIndexPair = this->variableToModuleIndexMap.find(variableName);
            STORM_LOG_THROW(variableNameToModuleIndexPair != this->variableToModuleIndexMap.end(), storm::exceptions::OutOfRangeException, "Variable '" << variableName << "' does not exist.");
            return variableNameToModuleIndexPair->second;
        }
        
        bool Program::hasRewardModel() const {
            return !this->rewardModels.empty();
        }
        
        bool Program::hasRewardModel(std::string const& name) const {
            auto const& nameIndexPair = this->rewardModelToIndexMap.find(name);
            return nameIndexPair != this->rewardModelToIndexMap.end();
        }
        
        std::vector<storm::prism::RewardModel> const& Program::getRewardModels() const {
            return this->rewardModels;
        }
        
        std::size_t Program::getNumberOfRewardModels() const {
            return this->getRewardModels().size();
        }
        
        storm::prism::RewardModel const& Program::getRewardModel(std::string const& name) const {
            auto const& nameIndexPair = this->rewardModelToIndexMap.find(name);
            STORM_LOG_THROW(nameIndexPair != this->rewardModelToIndexMap.end(), storm::exceptions::OutOfRangeException, "Reward model '" << name << "' does not exist.");
            return this->getRewardModels()[nameIndexPair->second];
        }
        
        RewardModel const& Program::getRewardModel(uint_fast64_t index) const {
            STORM_LOG_THROW(this->getNumberOfRewardModels() > index, storm::exceptions::OutOfRangeException, "Reward model with index " << index << " does not exist.");
            return this->rewardModels[index];
        }
        
        bool Program::hasLabel(std::string const& labelName) const {
            auto it = std::find_if(labels.begin(), labels.end(), [&labelName] (storm::prism::Label const& label) { return label.getName() == labelName; } );
            return it != labels.end();
        }
        
        std::vector<Label> const& Program::getLabels() const {
            return this->labels;
        }
        
        storm::expressions::Expression const& Program::getLabelExpression(std::string const& label) const {
            auto const& labelIndexPair = labelToIndexMap.find(label);
            STORM_LOG_THROW(labelIndexPair != labelToIndexMap.end(), storm::exceptions::InvalidArgumentException, "Cannot retrieve expression for unknown label '" << label << "'.");
            return this->labels[labelIndexPair->second].getStatePredicateExpression();
        }
        
        std::size_t Program::getNumberOfLabels() const {
            return this->getLabels().size();
        }
        
        void Program::addLabel(std::string const& name, storm::expressions::Expression const& statePredicateExpression) {
            auto it = std::find_if(this->labels.begin(), this->labels.end(), [&name] (storm::prism::Label const& label) { return label.getName() == name; });
            STORM_LOG_THROW(it == this->labels.end(), storm::exceptions::InvalidArgumentException, "Cannot add a label '" << name << "', because a label with that name already exists.");
            this->labels.emplace_back(name, statePredicateExpression);
        }

        void Program::removeLabel(std::string const& name) {
            auto it = std::find_if(this->labels.begin(), this->labels.end(), [&name] (storm::prism::Label const& label) { return label.getName() == name; });
            STORM_LOG_THROW(it != this->labels.end(), storm::exceptions::InvalidArgumentException, "Canno remove unknown label '" << name << "'.");
            this->labels.erase(it);
        }
        
        void Program::filterLabels(std::set<std::string> const& labelSet) {
            std::vector<storm::prism::Label> newLabels;
            newLabels.reserve(labelSet.size());
            
            // Now filter the labels by the criterion whether or not their name appears in the given label set.
            for (auto it = labels.begin(), ite = labels.end(); it != ite; ++it) {
                auto setIt = labelSet.find(it->getName());
                if (setIt != labelSet.end()) {
                    newLabels.emplace_back(*it);
                }
            }
            
            // Move the new labels in place.
            this->labels = std::move(newLabels);
        }
        
        Program Program::restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet) const {
            std::vector<storm::prism::Module> newModules;
            newModules.reserve(this->getNumberOfModules());
            
            for (auto const& module : this->getModules()) {
                newModules.push_back(module.restrictCommands(indexSet));
            }
            
            return Program(this->manager, this->getModelType(), this->getConstants(), this->getGlobalBooleanVariables(), this->getGlobalIntegerVariables(), this->getFormulas(), newModules, this->getActionNameToIndexMapping(), this->getRewardModels(), false, this->getInitialConstruct(), this->getLabels());
        }
        
        void Program::createMappings() {
            // Build the mappings for constants, global variables, formulas, modules, reward models and labels.
            for (uint_fast64_t constantIndex = 0; constantIndex < this->getNumberOfConstants(); ++constantIndex) {
                this->constantToIndexMap[this->getConstants()[constantIndex].getName()] = constantIndex;
            }
            for (uint_fast64_t globalVariableIndex = 0; globalVariableIndex < this->getNumberOfGlobalBooleanVariables(); ++globalVariableIndex) {
                this->globalBooleanVariableToIndexMap[this->getGlobalBooleanVariables()[globalVariableIndex].getName()] = globalVariableIndex;
            }
            for (uint_fast64_t globalVariableIndex = 0; globalVariableIndex < this->getNumberOfGlobalIntegerVariables(); ++globalVariableIndex) {
                this->globalIntegerVariableToIndexMap[this->getGlobalIntegerVariables()[globalVariableIndex].getName()] = globalVariableIndex;
            }
            for (uint_fast64_t formulaIndex = 0; formulaIndex < this->getNumberOfFormulas(); ++formulaIndex) {
                this->formulaToIndexMap[this->getFormulas()[formulaIndex].getName()] = formulaIndex;
            }
            for (uint_fast64_t labelIndex = 0; labelIndex < this->getNumberOfLabels(); ++labelIndex) {
                this->labelToIndexMap[this->getLabels()[labelIndex].getName()] = labelIndex;
            }
            for (uint_fast64_t moduleIndex = 0; moduleIndex < this->getNumberOfModules(); ++moduleIndex) {
                this->moduleToIndexMap[this->getModules()[moduleIndex].getName()] = moduleIndex;
            }
            for (uint_fast64_t rewardModelIndex = 0; rewardModelIndex < this->getNumberOfRewardModels(); ++rewardModelIndex) {
                this->rewardModelToIndexMap[this->getRewardModels()[rewardModelIndex].getName()] = rewardModelIndex;
            }
            
            for (auto const& actionIndexPair : this->getActionNameToIndexMapping()) {
                this->actions.insert(actionIndexPair.first);
                this->indexToActionMap.emplace(actionIndexPair.second, actionIndexPair.first);
                
                // Only let all non-zero indices be synchronizing.
                if (actionIndexPair.second != 0) {
                    this->synchronizingActionIndices.insert(actionIndexPair.second);
                }
            }
            
            // Build the mapping from action names to module indices so that the lookup can later be performed quickly.
            for (unsigned int moduleIndex = 0; moduleIndex < this->getNumberOfModules(); moduleIndex++) {
                Module const& module = this->getModule(moduleIndex);
                
                for (auto const& actionIndex : module.getSynchronizingActionIndices()) {
                    auto const& actionModuleIndicesPair = this->actionIndicesToModuleIndexMap.find(actionIndex);
                    if (actionModuleIndicesPair == this->actionIndicesToModuleIndexMap.end()) {
                        this->actionIndicesToModuleIndexMap[actionIndex] = std::set<uint_fast64_t>();
                    }
                    this->actionIndicesToModuleIndexMap[actionIndex].insert(moduleIndex);
                }
                
                // Put in the appropriate entries for the mapping from variable names to module index.
                for (auto const& booleanVariable : module.getBooleanVariables()) {
                    this->variableToModuleIndexMap[booleanVariable.getName()] = moduleIndex;
                }
                for (auto const& integerVariable : module.getBooleanVariables()) {
                    this->variableToModuleIndexMap[integerVariable.getName()] = moduleIndex;
                }
            }

        }
        
        Program Program::defineUndefinedConstants(std::map<storm::expressions::Variable, storm::expressions::Expression> const& constantDefinitions) const {
            // For sanity checking, we keep track of all undefined constants that we define in the course of this procedure.
            std::set<storm::expressions::Variable> definedUndefinedConstants;
            
            std::vector<Constant> newConstants;
            newConstants.reserve(this->getNumberOfConstants());
            for (auto const& constant : this->getConstants()) {
                // If the constant is already defined, we need to replace the appearances of undefined constants in its
                // defining expression
                if (constant.isDefined()) {
                    // Make sure we are not trying to define an already defined constant.
                    STORM_LOG_THROW(constantDefinitions.find(constant.getExpressionVariable()) == constantDefinitions.end(), storm::exceptions::InvalidArgumentException, "Illegally defining already defined constant '" << constant.getName() << "'.");
                    
                    // Now replace the occurrences of undefined constants in its defining expression.
                    newConstants.emplace_back(constant.getExpressionVariable(), constant.getExpression().substitute(constantDefinitions), constant.getFilename(), constant.getLineNumber());
                } else {
                    auto const& variableExpressionPair = constantDefinitions.find(constant.getExpressionVariable());
                    
                    // If the constant is not defined by the mapping, we leave it like it is.
                    if (variableExpressionPair == constantDefinitions.end()) {
                        newConstants.emplace_back(constant);
                    } else {
                        // Otherwise, we add it to the defined constants and assign it the appropriate expression.
                        definedUndefinedConstants.insert(constant.getExpressionVariable());
                        
                        // Make sure the type of the constant is correct.
                        STORM_LOG_THROW(variableExpressionPair->second.getType() == constant.getType(), storm::exceptions::InvalidArgumentException, "Illegal type of expression defining constant '" << constant.getName() << "'.");
                        
                        // Now create the defined constant.
                        newConstants.emplace_back(constant.getExpressionVariable(), variableExpressionPair->second, constant.getFilename(), constant.getLineNumber());
                    }
                }
            }
            
            // As a sanity check, we make sure that the given mapping does not contain any definitions for identifiers
            // that are not undefined constants.
            for (auto const& constantExpressionPair : constantDefinitions) {
                STORM_LOG_THROW(definedUndefinedConstants.find(constantExpressionPair.first) != definedUndefinedConstants.end(), storm::exceptions::InvalidArgumentException, "Unable to define non-existant constant.");
            }
            
            return Program(this->manager, this->getModelType(), newConstants, this->getGlobalBooleanVariables(), this->getGlobalIntegerVariables(), this->getFormulas(), this->getModules(), this->getActionNameToIndexMapping(), this->getRewardModels(), false, this->getInitialConstruct(), this->getLabels());
        }
        
        Program Program::substituteConstants() const {
            // We start by creating the appropriate substitution
            std::map<storm::expressions::Variable, storm::expressions::Expression> constantSubstitution;
            std::vector<Constant> newConstants(this->getConstants());
            for (uint_fast64_t constantIndex = 0; constantIndex < newConstants.size(); ++constantIndex) {
                auto const& constant = newConstants[constantIndex];
                
                // Put the corresponding expression in the substitution.
                if(constant.isDefined()) {
                    constantSubstitution.emplace(constant.getExpressionVariable(), constant.getExpression().simplify());
                
                    // If there is at least one more constant to come, we substitute the constants we have so far.
                    if (constantIndex + 1 < newConstants.size()) {
                        newConstants[constantIndex + 1] = newConstants[constantIndex + 1].substitute(constantSubstitution);
                    }
                }
            }
            
            // Now we can substitute the constants in all expressions appearing in the program.
            std::vector<BooleanVariable> newBooleanVariables;
            newBooleanVariables.reserve(this->getNumberOfGlobalBooleanVariables());
            for (auto const& booleanVariable : this->getGlobalBooleanVariables()) {
                newBooleanVariables.emplace_back(booleanVariable.substitute(constantSubstitution));
            }
            
            std::vector<IntegerVariable> newIntegerVariables;
            newBooleanVariables.reserve(this->getNumberOfGlobalIntegerVariables());
            for (auto const& integerVariable : this->getGlobalIntegerVariables()) {
                newIntegerVariables.emplace_back(integerVariable.substitute(constantSubstitution));
            }
            
            std::vector<Formula> newFormulas;
            newFormulas.reserve(this->getNumberOfFormulas());
            for (auto const& formula : this->getFormulas()) {
                newFormulas.emplace_back(formula.substitute(constantSubstitution));
            }
            
            std::vector<Module> newModules;
            newModules.reserve(this->getNumberOfModules());
            for (auto const& module : this->getModules()) {
                newModules.emplace_back(module.substitute(constantSubstitution));
            }
            
            std::vector<RewardModel> newRewardModels;
            newRewardModels.reserve(this->getNumberOfRewardModels());
            for (auto const& rewardModel : this->getRewardModels()) {
                newRewardModels.emplace_back(rewardModel.substitute(constantSubstitution));
            }
            
            storm::prism::InitialConstruct newInitialConstruct = this->getInitialConstruct().substitute(constantSubstitution);
            
            std::vector<Label> newLabels;
            newLabels.reserve(this->getNumberOfLabels());
            for (auto const& label : this->getLabels()) {
                newLabels.emplace_back(label.substitute(constantSubstitution));
            }
            
            return Program(this->manager, this->getModelType(), newConstants, newBooleanVariables, newIntegerVariables, newFormulas, newModules, this->getActionNameToIndexMapping(), newRewardModels, false, newInitialConstruct, newLabels);
        }
        
        void Program::checkValidity(Program::ValidityCheckLevel lvl) const {
            
            // Start by checking the constant declarations.
            std::set<storm::expressions::Variable> all;
            std::set<storm::expressions::Variable> allGlobals;
            std::set<storm::expressions::Variable> globalVariables;
            std::set<storm::expressions::Variable> constants;
            for (auto const& constant : this->getConstants()) {
                // Check defining expressions of defined constants.
                if (constant.isDefined()) {
                    std::set<storm::expressions::Variable> containedVariables = constant.getExpression().getVariables();
                    std::set<storm::expressions::Variable> illegalVariables;
                    std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                    bool isValid = illegalVariables.empty();
                    
                    if (!isValid) {
                        std::vector<std::string> illegalVariableNames;
                        for (auto const& var : illegalVariables) {
                            illegalVariableNames.push_back(var.getName());
                        }
                        STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << constant.getFilename() << ", line " << constant.getLineNumber() << ": defining expression refers to unknown identifiers: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                    }
                }
                
                // Record the new identifier for future checks.
                constants.insert(constant.getExpressionVariable());
                all.insert(constant.getExpressionVariable());
                allGlobals.insert(constant.getExpressionVariable());
            }
            
            // Now we check the variable declarations. We start with the global variables.
            std::set<storm::expressions::Variable> variables;
            for (auto const& variable : this->getGlobalBooleanVariables()) {
                // Check the initial value of the variable.
                std::set<storm::expressions::Variable> containedVariables = variable.getInitialValueExpression().getVariables();
                std::set<storm::expressions::Variable> illegalVariables;
                std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                bool isValid = illegalVariables.empty();

                if (!isValid) {
                    std::vector<std::string> illegalVariableNames;
                    for (auto const& var : illegalVariables) {
                        illegalVariableNames.push_back(var.getName());
                    }
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": initial value expression refers to unknown constants: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                }

                // Record the new identifier for future checks.
                variables.insert(variable.getExpressionVariable());
                all.insert(variable.getExpressionVariable());
                allGlobals.insert(variable.getExpressionVariable());
                globalVariables.insert(variable.getExpressionVariable());
            }
            for (auto const& variable : this->getGlobalIntegerVariables()) {
                // Check that bound expressions of the range.
                std::set<storm::expressions::Variable> containedVariables = variable.getLowerBoundExpression().getVariables();
                std::set<storm::expressions::Variable> illegalVariables;
                std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                bool isValid = illegalVariables.empty();

                if (!isValid) {
                    std::vector<std::string> illegalVariableNames;
                    for (auto const& var : illegalVariables) {
                        illegalVariableNames.push_back(var.getName());
                    }
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": lower bound expression refers to unknown constants: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                }

                containedVariables = variable.getLowerBoundExpression().getVariables();
                std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                isValid = illegalVariables.empty();
                if (!isValid) {
                    std::vector<std::string> illegalVariableNames;
                    for (auto const& var : illegalVariables) {
                        illegalVariableNames.push_back(var.getName());
                    }
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": upper bound expression refers to unknown constants: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                }
                
                // Check the initial value of the variable.
                containedVariables = variable.getInitialValueExpression().getVariables();
                std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                isValid = illegalVariables.empty();
                if (!isValid) {
                    std::vector<std::string> illegalVariableNames;
                    for (auto const& var : illegalVariables) {
                        illegalVariableNames.push_back(var.getName());
                    }
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": initial value expression refers to unknown constants: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                }
                
                // Record the new identifier for future checks.
                variables.insert(variable.getExpressionVariable());
                all.insert(variable.getExpressionVariable());
                allGlobals.insert(variable.getExpressionVariable());
                globalVariables.insert(variable.getExpressionVariable());
            }

            // Now go through the variables of the modules.
            for (auto const& module : this->getModules()) {
                for (auto const& variable : module.getBooleanVariables()) {
                    // Check the initial value of the variable.
                    std::set<storm::expressions::Variable> containedVariables = variable.getInitialValueExpression().getVariables();
                    std::set<storm::expressions::Variable> illegalVariables;
                    std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                    bool isValid = illegalVariables.empty();
                    if (!isValid) {
                        std::vector<std::string> illegalVariableNames;
                        for (auto const& var : illegalVariables) {
                            illegalVariableNames.push_back(var.getName());
                        }
                        STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": initial value expression refers to unknown constants: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                    }
                    
                    // Record the new identifier for future checks.
                    variables.insert(variable.getExpressionVariable());
                    all.insert(variable.getExpressionVariable());
                }
                for (auto const& variable : module.getIntegerVariables()) {
                    // Check that bound expressions of the range.
                    std::set<storm::expressions::Variable> containedVariables = variable.getLowerBoundExpression().getVariables();
                    std::set<storm::expressions::Variable> illegalVariables;
                    std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                    bool isValid = illegalVariables.empty();
                    if (!isValid) {
                        std::vector<std::string> illegalVariableNames;
                        for (auto const& var : illegalVariables) {
                            illegalVariableNames.push_back(var.getName());
                        }
                        STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": lower bound expression refers to unknown constants: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                    }
                    
                    containedVariables = variable.getLowerBoundExpression().getVariables();
                    illegalVariables.clear();
                    std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                    isValid = illegalVariables.empty();
                    if (!isValid) {
                        std::vector<std::string> illegalVariableNames;
                        for (auto const& var : illegalVariables) {
                            illegalVariableNames.push_back(var.getName());
                        }
                        STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": upper bound expression refers to unknown constants: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                    }
                    
                    // Check the initial value of the variable.
                    containedVariables = variable.getInitialValueExpression().getVariables();
                    illegalVariables.clear();
                    std::set_difference(containedVariables.begin(), containedVariables.end(), constants.begin(), constants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                    isValid = illegalVariables.empty();
                    if (!isValid) {
                        std::vector<std::string> illegalVariableNames;
                        for (auto const& var : illegalVariables) {
                            illegalVariableNames.push_back(var.getName());
                        }
                        STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": initial value expression refers to unknown constants: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                    }
                    
                    // Record the new identifier for future checks.
                    variables.insert(variable.getExpressionVariable());
                    all.insert(variable.getExpressionVariable());
                }
            }
            
            // Create the set of valid identifiers for future checks.
            std::set<storm::expressions::Variable> variablesAndConstants;
            std::set_union(variables.begin(), variables.end(), constants.begin(), constants.end(), std::inserter(variablesAndConstants, variablesAndConstants.begin()));
            
            
            // Check the commands of the modules.
            bool hasProbabilisticCommand = false;
            bool hasMarkovianCommand = false;
            bool hasLabeledMarkovianCommand = false;
            for (auto const& module : this->getModules()) {
                std::set<storm::expressions::Variable> legalVariables = globalVariables;
                for (auto const& variable : module.getBooleanVariables()) {
                    legalVariables.insert(variable.getExpressionVariable());
                }
                for (auto const& variable : module.getIntegerVariables()) {
                    legalVariables.insert(variable.getExpressionVariable());
                }
                
                for (auto& command : module.getCommands()) {
                    // Check the guard.
                    std::set<storm::expressions::Variable> containedVariables = command.getGuardExpression().getVariables();
                    std::set<storm::expressions::Variable> illegalVariables;
                    std::set_difference(containedVariables.begin(), containedVariables.end(), variablesAndConstants.begin(), variablesAndConstants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                    bool isValid = illegalVariables.empty();
                    if (!isValid) {
                        std::vector<std::string> illegalVariableNames;
                        for (auto const& var : illegalVariables) {
                            illegalVariableNames.push_back(var.getName());
                        }
                        STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": guard " << command.getGuardExpression()  << " refers to unknown identifiers: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                    }
                    STORM_LOG_THROW(command.getGuardExpression().hasBooleanType(), storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": expression for guard must evaluate to type 'bool'.");
                    
                    // Record which types of commands were seen.
                    if (command.isMarkovian()) {
                        hasMarkovianCommand = true;
                    } else {
                        hasProbabilisticCommand = true;
                    }
                    
                    // If the command is Markovian and labeled, we throw an error or raise a warning, depending on
                    // whether or not the PRISM compatibility mode was enabled.
                    if (command.isMarkovian() && command.isLabeled()) {
                        hasLabeledMarkovianCommand = true;
                    }
                    
                    // Check all updates.
                    for (auto const& update : command.getUpdates()) {
                        containedVariables = update.getLikelihoodExpression().getVariables();
                        illegalVariables.clear();
                        std::set_difference(containedVariables.begin(), containedVariables.end(), variablesAndConstants.begin(), variablesAndConstants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                        isValid = illegalVariables.empty();
                        if (!isValid) {
                            std::vector<std::string> illegalVariableNames;
                            for (auto const& var : illegalVariables) {
                                illegalVariableNames.push_back(var.getName());
                            }
                            STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": likelihood expression refers to unknown identifiers: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                        }
                        
                        // Check all assignments.
                        std::set<storm::expressions::Variable> alreadyAssignedVariables;
                        for (auto const& assignment : update.getAssignments()) {
                            storm::expressions::Variable assignedVariable = manager->getVariable(assignment.getVariableName());

                            if (legalVariables.find(assignedVariable) == legalVariables.end()) {
                                if (all.find(assignedVariable) != all.end()) {
                                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": assignment illegally refers to variable '" << assignment.getVariableName() << "'.");
                                } else {
                                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": assignment refers to unknown variable '" << assignment.getVariableName() << "'.");
                                }
                            }
                            STORM_LOG_THROW(alreadyAssignedVariables.find(assignedVariable) == alreadyAssignedVariables.end(), storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": duplicate assignment to variable '" << assignment.getVariableName() << "'.");
                            STORM_LOG_THROW(assignedVariable.getType() == assignment.getExpression().getType(), storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": illegally assigning a value of type '" << assignment.getExpression().getType() << "' to variable '" << assignment.getVariableName() << "' of type '" << assignedVariable.getType() << "'.");
                            
                            containedVariables = assignment.getExpression().getVariables();
                            illegalVariables.clear();
                            std::set_difference(containedVariables.begin(), containedVariables.end(), variablesAndConstants.begin(), variablesAndConstants.end(), std::inserter(illegalVariables, illegalVariables.begin()));
                            isValid = illegalVariables.empty();
                            if (!isValid) {
                                std::vector<std::string> illegalVariableNames;
                                for (auto const& var : illegalVariables) {
                                    illegalVariableNames.push_back(var.getName());
                                }
                                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": assigned expression refers to unknown identifiers: " << boost::algorithm::join(illegalVariableNames, ",") << ".");
                            }
                            
                            // Add the current variable to the set of assigned variables (of this update).
                            alreadyAssignedVariables.insert(assignedVariable);
                        }
                    }
                }
            }
            
            if (hasLabeledMarkovianCommand) {
                if (storm::settings::generalSettings().isPrismCompatibilityEnabled()) {
                    STORM_LOG_WARN_COND(false, "The model uses synchronizing Markovian commands. This may lead to unexpected verification results, because of unclear semantics.");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "The model uses synchronizing Markovian commands. This may lead to unexpected verification results, because of unclear semantics.");
                }
            }
            
            if (this->getModelType() == Program::ModelType::DTMC || this->getModelType() == Program::ModelType::MDP) {
                STORM_LOG_THROW(!hasMarkovianCommand, storm::exceptions::WrongFormatException, "Discrete-time model must not have Markovian commands.");
            } else if (this->getModelType() == Program::ModelType::CTMC) {
                STORM_LOG_THROW(!hasProbabilisticCommand, storm::exceptions::WrongFormatException, "The input model is a CTMC, but uses probabilistic commands like they are used in PRISM. Please use Markovian commands instead or turn on the PRISM compatibility mode using the appropriate flag.");
            }
            
            // Now check the reward models.
            for (auto const& rewardModel : this->getRewardModels()) {
                for (auto const& stateReward : rewardModel.getStateRewards()) {
                    std::set<storm::expressions::Variable> containedVariables = stateReward.getStatePredicateExpression().getVariables();
                    bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << stateReward.getFilename() << ", line " << stateReward.getLineNumber() << ": state reward expression refers to unknown identifiers.");
                    STORM_LOG_THROW(stateReward.getStatePredicateExpression().hasBooleanType(), storm::exceptions::WrongFormatException, "Error in " << stateReward.getFilename() << ", line " << stateReward.getLineNumber() << ": state predicate must evaluate to type 'bool'.");
                    
                    containedVariables = stateReward.getRewardValueExpression().getVariables();
                    isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << stateReward.getFilename() << ", line " << stateReward.getLineNumber() << ": state reward value expression refers to unknown identifiers.");
                    STORM_LOG_THROW(stateReward.getRewardValueExpression().hasNumericalType(), storm::exceptions::WrongFormatException, "Error in " << stateReward.getFilename() << ", line " << stateReward.getLineNumber() << ": reward value expression must evaluate to numerical type.");
                }
                
                for (auto const& stateActionReward : rewardModel.getStateActionRewards()) {
                    std::set<storm::expressions::Variable> containedVariables = stateActionReward.getStatePredicateExpression().getVariables();
                    bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << stateActionReward.getFilename() << ", line " << stateActionReward.getLineNumber() << ": state reward expression refers to unknown identifiers.");
                    STORM_LOG_THROW(stateActionReward.getStatePredicateExpression().hasBooleanType(), storm::exceptions::WrongFormatException, "Error in " << stateActionReward.getFilename() << ", line " << stateActionReward.getLineNumber() << ": state predicate must evaluate to type 'bool'.");
                    
                    containedVariables = stateActionReward.getRewardValueExpression().getVariables();
                    isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << stateActionReward.getFilename() << ", line " << stateActionReward.getLineNumber() << ": state reward value expression refers to unknown identifiers.");
                    STORM_LOG_THROW(stateActionReward.getRewardValueExpression().hasNumericalType(), storm::exceptions::WrongFormatException, "Error in " << stateActionReward.getFilename() << ", line " << stateActionReward.getLineNumber() << ": reward value expression must evaluate to numerical type.");
                }
                
                for (auto const& transitionReward : rewardModel.getTransitionRewards()) {
                    std::set<storm::expressions::Variable> containedVariables = transitionReward.getSourceStatePredicateExpression().getVariables();
                    bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": state reward expression refers to unknown identifiers.");
                    STORM_LOG_THROW(transitionReward.getSourceStatePredicateExpression().hasBooleanType(), storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": state predicate must evaluate to type 'bool'.");

                    containedVariables = transitionReward.getTargetStatePredicateExpression().getVariables();
                    isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": state reward expression refers to unknown identifiers.");
                    STORM_LOG_THROW(transitionReward.getTargetStatePredicateExpression().hasBooleanType(), storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": state predicate must evaluate to type 'bool'.");

                    
                    containedVariables = transitionReward.getRewardValueExpression().getVariables();
                    isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": state reward value expression refers to unknown identifiers.");
                    STORM_LOG_THROW(transitionReward.getRewardValueExpression().hasNumericalType(), storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": reward value expression must evaluate to numerical type.");
                }
            }
            
            // Check the initial states expression.
            std::set<storm::expressions::Variable> containedIdentifiers = this->getInitialConstruct().getInitialStatesExpression().getVariables();
            bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
            STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << this->getInitialConstruct().getFilename() << ", line " << this->getInitialConstruct().getLineNumber() << ": initial expression refers to unknown identifiers.");
            
            // Check the labels.
            for (auto const& label : this->getLabels()) {
                std::set<storm::expressions::Variable> containedVariables = label.getStatePredicateExpression().getVariables();
                bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << label.getFilename() << ", line " << label.getLineNumber() << ": label expression refers to unknown identifiers.");
                STORM_LOG_THROW(label.getStatePredicateExpression().hasBooleanType(), storm::exceptions::WrongFormatException, "Error in " << label.getFilename() << ", line " << label.getLineNumber() << ": label predicate must evaluate to type 'bool'.");
            }
            
            // Check the formulas.
            for (auto const& formula : this->getFormulas()) {
                std::set<storm::expressions::Variable> containedVariables = formula.getExpression().getVariables();
                bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedVariables.begin(), containedVariables.end());
                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << formula.getFilename() << ", line " << formula.getLineNumber() << ": formula expression refers to unknown identifiers.");
            }
            
            if(lvl >= Program::ValidityCheckLevel::READYFORPROCESSING) {
                // We check for each global variable and each labeled command, whether there is at most one instance writing to that variable.
                std::set<std::pair<std::string, std::string>> globalBVarsWrittenToByCommand;
                std::set<std::pair<std::string, std::string>> globalIVarsWrittenToByCommand;
                for(auto const& module : this->getModules()) {
                    std::set<std::pair<std::string, std::string>> globalBVarsWrittenToByCommandInThisModule;
                    std::set<std::pair<std::string, std::string>> globalIVarsWrittenToByCommandInThisModule;
                    for (auto const& command : module.getCommands()) {
                        if(!command.isLabeled()) continue;
                        for (auto const& update : command.getUpdates()) {
                             for (auto const& assignment : update.getAssignments()) {
                                 if(this->globalBooleanVariableExists(assignment.getVariable().getName())) {
                                     globalBVarsWrittenToByCommandInThisModule.insert({assignment.getVariable().getName(), command.getActionName()});
                                 }
                                 else if(this->globalIntegerVariableExists(assignment.getVariable().getName())) {
                                     globalIVarsWrittenToByCommandInThisModule.insert({assignment.getVariable().getName(), command.getActionName()});
                                 }
                             }
                        }
                    }
                    for(auto const& entry : globalIVarsWrittenToByCommandInThisModule) {
                        if(globalIVarsWrittenToByCommand.find(entry) != globalIVarsWrittenToByCommand.end()) {
                            STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error in " << module.getFilename() << ", line " << module.getLineNumber() << ": assignment of (possibly) synchronizing command with label '" << entry.second << "' writes to global variable '" << entry.first << "'.");
                        }
                    }
                    for(auto const& entry : globalBVarsWrittenToByCommandInThisModule) {
                        if(globalBVarsWrittenToByCommand.find(entry) != globalBVarsWrittenToByCommand.end()) {
                            STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error in " << module.getFilename() << ", line " << module.getLineNumber() << ": assignment of (possibly) synchronizing command with label '" << entry.second << "' writes to global variable '" << entry.first << "'.");
                        }
                    }
                }
            }
        }
        
        Program Program::simplify() {
            std::vector<Module> newModules;
            std::vector<Constant> newConstants = this->getConstants();
            for (auto const& module : this->getModules()) {
                // Remove identity assignments from the updates
                std::vector<Command> newCommands;
                for (auto const& command : module.getCommands()) {
                    newCommands.emplace_back(command.removeIdentityAssignmentsFromUpdates());
                }
                
                // Substitute Variables by Global constants if possible.
                
                std::map<storm::expressions::Variable, storm::expressions::Expression> booleanVars;
                std::map<storm::expressions::Variable, storm::expressions::Expression> integerVars;
                for (auto const& variable : module.getBooleanVariables()) {
                    booleanVars.emplace(variable.getExpressionVariable(), variable.getInitialValueExpression());
                }
                for (auto const& variable : module.getIntegerVariables()) {
                    integerVars.emplace(variable.getExpressionVariable(), variable.getInitialValueExpression());
                }
                
                for (auto const& command : newCommands) {
                    // Check all updates.
                    for (auto const& update : command.getUpdates()) {
                        // Check all assignments.
                        for (auto const& assignment : update.getAssignments()) {
                            auto bit = booleanVars.find(assignment.getVariable());
                            if(bit != booleanVars.end()) {
                                booleanVars.erase(bit);
                            } else {
                                auto iit = integerVars.find(assignment.getVariable());
                                if(iit != integerVars.end()) {
                                    integerVars.erase(iit);
                                }
                            }
                        }
                    }
                }
            
                std::vector<storm::prism::BooleanVariable> newBVars;
                for(auto const& variable : module.getBooleanVariables()) {
                    if(booleanVars.count(variable.getExpressionVariable()) == 0) {
                        newBVars.push_back(variable);
                    }
                }
                std::vector<storm::prism::IntegerVariable> newIVars;
                for(auto const& variable : module.getIntegerVariables()) {
                    if(integerVars.count(variable.getExpressionVariable()) == 0) {
                        newIVars.push_back(variable);
                    }
                }
                
                newModules.emplace_back(module.getName(), newBVars, newIVars, newCommands);
                
                for(auto const& entry : booleanVars) {
                    newConstants.emplace_back(entry.first, entry.second);
                }
                
                for(auto const& entry : integerVars) {
                    newConstants.emplace_back(entry.first, entry.second);
                }
            }
            
            return replaceModulesAndConstantsInProgram(newModules, newConstants).substituteConstants();
            
        }
        
        Program Program::replaceModulesAndConstantsInProgram(std::vector<Module> const& newModules, std::vector<Constant> const& newConstants) {
            return Program(this->manager, modelType, newConstants, getGlobalBooleanVariables(), getGlobalIntegerVariables(), getFormulas(), newModules, getActionNameToIndexMapping(), getRewardModels(), false, getInitialConstruct(), getLabels(), "", 0, false);
        }
        
        Program Program::flattenModules(std::unique_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) const {
            // If the current program has only one module, we can simply return a copy.
            if (this->getNumberOfModules() == 1) {
                return Program(*this);
            }
            
            STORM_LOG_THROW(this->getModelType() == ModelType::DTMC || this->getModelType() == ModelType::MDP, storm::exceptions::InvalidTypeException, "Unable to flatten modules for model of type '" << this->getModelType() << "'.");
            
            // Otherwise, we need to actually flatten the contained modules.
            
            // Get an SMT solver for computing the possible guard combinations.
            std::unique_ptr<storm::solver::SmtSolver> solver = smtSolverFactory->create(*manager);
            
            // Set up the data we need to gather to create the flat module.
            std::stringstream newModuleName;
            std::vector<storm::prism::BooleanVariable> allBooleanVariables;
            std::vector<storm::prism::IntegerVariable> allIntegerVariables;
            std::vector<storm::prism::Command> newCommands;
            uint_fast64_t nextCommandIndex = 0;
            uint_fast64_t nextUpdateIndex = 0;
            
            // Assert the values of the constants.
            for (auto const& constant : this->getConstants()) {
                if (constant.isDefined()) {
                    solver->add(constant.getExpressionVariable() == constant.getExpression());
                }
            }
            
            // Assert the bounds of the global variables.
            for (auto const& variable : this->getGlobalIntegerVariables()) {
                solver->add(variable.getExpression() >= variable.getLowerBoundExpression());
                solver->add(variable.getExpression() <= variable.getUpperBoundExpression());
            }
            
            // Make the global variables local, such that the resulting module covers all occurring variables. Note that
            // this is just for simplicity and is not needed.
            allBooleanVariables.insert(allBooleanVariables.end(), this->getGlobalBooleanVariables().begin(), this->getGlobalBooleanVariables().end());
            allIntegerVariables.insert(allIntegerVariables.end(), this->getGlobalIntegerVariables().begin(), this->getGlobalIntegerVariables().end());
            
            // Now go through the modules, gather the variables, construct the name of the new module and assert the
            // bounds of the discovered variables.
            for (auto const& module : this->getModules()) {
                newModuleName << module.getName() << "_";
                allBooleanVariables.insert(allBooleanVariables.end(), module.getBooleanVariables().begin(), module.getBooleanVariables().end());
                allIntegerVariables.insert(allIntegerVariables.end(), module.getIntegerVariables().begin(), module.getIntegerVariables().end());
                
                for (auto const& variable : module.getIntegerVariables()) {
                    solver->add(variable.getExpression() >= variable.getLowerBoundExpression());
                    solver->add(variable.getExpression() <= variable.getUpperBoundExpression());
                }
                
                // The commands without a synchronizing action name, can simply be copied (plus adjusting the global
                // indices of the command and its updates).
                for (auto const& command : module.getCommands()) {
                    if (!command.isLabeled()) {
                        std::vector<storm::prism::Update> updates;
                        updates.reserve(command.getUpdates().size());
                        
                        for (auto const& update : command.getUpdates()) {
                            updates.push_back(storm::prism::Update(nextUpdateIndex, update.getLikelihoodExpression(), update.getAssignments(), update.getFilename(), 0));
                            ++nextUpdateIndex;
                        }
                        
                        newCommands.push_back(storm::prism::Command(nextCommandIndex, command.isMarkovian(), actionToIndexMap.find("")->second, "", command.getGuardExpression(), updates, command.getFilename(), 0));
                        ++nextCommandIndex;
                    }
                }
            }
            
            // Save state of solver so that we can always restore the point where we have exactly the constant values
            // and variables bounds on the assertion stack.
            solver->push();
            
            // Now we need to enumerate all possible combinations of synchronizing commands. For this, we iterate over
            // all actions and let the solver enumerate the possible combinations of commands that can be enabled together.
            for (auto const& actionIndex : this->getSynchronizingActionIndices()) {
                bool noCombinationsForAction = false;
                
                // Prepare the list that stores for each module the list of commands with the given action.
                std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>> possibleCommands;
                
                for (auto const& module : this->getModules()) {
                    // If the module has no command with this action, we can skip it.
                    if (!module.hasActionIndex(actionIndex)) {
                        continue;
                    }
                    
                    std::set<uint_fast64_t> const& commandIndices = module.getCommandIndicesByActionIndex(actionIndex);
                    
                    // If there is no command even though the module has this action, there is no valid command
                    // combination with this action.
                    if (commandIndices.empty()) {
                        noCombinationsForAction = true;
                        break;
                    }
                    
                    // Prepare empty list of commands for this module.
                    possibleCommands.push_back(std::vector<std::reference_wrapper<storm::prism::Command const>>());
                    
                    // Add references to the commands labeled with the current action.
                    for (auto const& commandIndex : commandIndices) {
                        possibleCommands.back().push_back(module.getCommand(commandIndex));
                    }
                }
                
                // If there are no valid combinations for the action, we need to skip the generation of synchronizing
                // commands.
                if (!noCombinationsForAction) {
                    // Save the solver state to be able to restore it when this action index is done.
                    solver->push();
                    
                    // Start by creating a fresh auxiliary variable for each command and link it with the guard.
                    std::vector<std::vector<storm::expressions::Variable>> commandVariables(possibleCommands.size());
                    std::vector<storm::expressions::Variable> allCommandVariables;
                    for (uint_fast64_t outerIndex = 0; outerIndex < possibleCommands.size(); ++outerIndex) {
                        // Create auxiliary variables and link them with the guards.
                        for (uint_fast64_t innerIndex = 0; innerIndex < possibleCommands[outerIndex].size(); ++innerIndex) {
                            commandVariables[outerIndex].push_back(manager->declareFreshBooleanVariable());
                            allCommandVariables.push_back(commandVariables[outerIndex].back());
                            solver->add(implies(commandVariables[outerIndex].back(), possibleCommands[outerIndex][innerIndex].get().getGuardExpression()));
                        }
                        
                        storm::expressions::Expression atLeastOneCommandFromModule = manager->boolean(false);
                        for (auto const& commandVariable : commandVariables[outerIndex]) {
                            atLeastOneCommandFromModule = atLeastOneCommandFromModule || commandVariable;
                        }
                        solver->add(atLeastOneCommandFromModule);
                    }
                    
                    // Now we are in a position to start the enumeration over all command variables.
                    solver->allSat(allCommandVariables, [&] (storm::solver::SmtSolver::ModelReference& modelReference) -> bool {
                        // Now we need to reconstruct the chosen commands from the valuation of the command variables.
                        std::vector<std::vector<std::reference_wrapper<Command const>>> chosenCommands(possibleCommands.size());

                        for (uint_fast64_t outerIndex = 0; outerIndex < commandVariables.size(); ++outerIndex) {
                            for (uint_fast64_t innerIndex = 0; innerIndex < commandVariables[outerIndex].size(); ++innerIndex) {
                                if (modelReference.getBooleanValue(commandVariables[outerIndex][innerIndex])) {
                                    chosenCommands[outerIndex].push_back(possibleCommands[outerIndex][innerIndex]);
                                }
                            }
                        }
                        
                        // Now that we have retrieved the commands, we need to build their synchronizations and add them
                        // to the flattened module.
                        std::vector<std::vector<std::reference_wrapper<Command const>>::const_iterator> iterators;
                        for (auto const& element : chosenCommands) {
                            iterators.push_back(element.begin());
                        }
                        
                        bool movedAtLeastOneIterator = false;
                        std::vector<std::reference_wrapper<Command const>> commandCombination(chosenCommands.size(), chosenCommands.front().front());
                        do {
                            for (uint_fast64_t index = 0; index < iterators.size(); ++index) {
                                commandCombination[index] = *iterators[index];
                            }

                            newCommands.push_back(synchronizeCommands(nextCommandIndex, actionIndex, nextUpdateIndex, indexToActionMap.find(actionIndex)->second, commandCombination));
                            
                            // Move the counters appropriately.
                            ++nextCommandIndex;
                            nextUpdateIndex += newCommands.back().getNumberOfUpdates();

                            movedAtLeastOneIterator = false;
                            for (uint_fast64_t index = 0; index < iterators.size(); ++index) {
                                ++iterators[index];
                                if (iterators[index] != chosenCommands[index].cend()) {
                                    movedAtLeastOneIterator = true;
                                    break;
                                } else {
                                    iterators[index] = chosenCommands[index].cbegin();
                                }
                            }
                        } while (movedAtLeastOneIterator);
                        
                        return true;
                    });
                    
                    solver->pop();
                }
            }
            
            // Finally, we can create the module and the program and return it.
            storm::prism::Module singleModule(newModuleName.str(), allBooleanVariables, allIntegerVariables, newCommands, this->getFilename(), 0);
            return Program(manager, this->getModelType(), this->getConstants(), std::vector<storm::prism::BooleanVariable>(), std::vector<storm::prism::IntegerVariable>(), this->getFormulas(), {singleModule}, actionToIndexMap, this->getRewardModels(), false, this->getInitialConstruct(), this->getLabels(), this->getFilename(), 0, true);
        }

        std::unordered_map<uint_fast64_t, std::string> Program::buildCommandIndexToActionNameMap() const {
            std::unordered_map<uint_fast64_t, std::string> res;
            for(auto const& m : this->modules) {
                for(auto const& c : m.getCommands()) {
                    res.emplace(c.getGlobalIndex(), c.getActionName());
                }
            }
            return res;
        }

        std::unordered_map<uint_fast64_t, std::string> Program::buildActionIndexToActionNameMap() const {
            std::unordered_map<uint_fast64_t, std::string> res;
            for(auto const& nameIndexPair : actionToIndexMap) {
                res.emplace(nameIndexPair.second, nameIndexPair.first);
            }
            return res;
        }

        std::unordered_map<uint_fast64_t, uint_fast64_t> Program::buildCommandIndexToActionIndex() const {
            std::unordered_map<uint_fast64_t, uint_fast64_t> res;
            for(auto const& m : this->modules) {
                for(auto const& c : m.getCommands()) {
                    res.emplace(c.getGlobalIndex(), c.getActionIndex());
                }
            }
            return res;

        };
        
        Command Program::synchronizeCommands(uint_fast64_t newCommandIndex, uint_fast64_t actionIndex, uint_fast64_t firstUpdateIndex, std::string const& actionName, std::vector<std::reference_wrapper<Command const>> const& commands) const {
            // To construct the synchronous product of the commands, we need to store a list of its updates.
            std::vector<storm::prism::Update> newUpdates;
            uint_fast64_t numberOfUpdates = 1;
            for (uint_fast64_t i = 0; i < commands.size(); ++i) {
                numberOfUpdates *= commands[i].get().getNumberOfUpdates();
            }
            newUpdates.reserve(numberOfUpdates);
            
            // Initialize all update iterators.
            std::vector<std::vector<storm::prism::Update>::const_iterator> updateIterators;
            for (uint_fast64_t i = 0; i < commands.size(); ++i) {
                updateIterators.push_back(commands[i].get().getUpdates().cbegin());
            }
            
            bool doneUpdates = false;
            do {
                // We create the new likelihood expression by multiplying the particapting updates' expressions.
                storm::expressions::Expression newLikelihoodExpression = updateIterators[0]->getLikelihoodExpression();
                for (uint_fast64_t i = 1; i < updateIterators.size(); ++i) {
                    newLikelihoodExpression = newLikelihoodExpression * updateIterators[i]->getLikelihoodExpression();
                }
                
                // Now concatenate all assignments of all participating updates.
                std::vector<storm::prism::Assignment> newAssignments;
                for (uint_fast64_t i = 0; i < updateIterators.size(); ++i) {
                    newAssignments.insert(newAssignments.end(), updateIterators[i]->getAssignments().begin(), updateIterators[i]->getAssignments().end());
                }
                
                // Then we are ready to create the new update.
                newUpdates.push_back(storm::prism::Update(firstUpdateIndex, newLikelihoodExpression, newAssignments, this->getFilename(), 0));
                ++firstUpdateIndex;
                
                // Now check whether there is some update combination we have not yet explored.
                bool movedIterator = false;
                for (int_fast64_t j = updateIterators.size() - 1; j >= 0; --j) {
                    ++updateIterators[j];
                    if (updateIterators[j] != commands[j].get().getUpdates().cend()) {
                        movedIterator = true;
                        break;
                    } else {
                        // Reset the iterator to the beginning of the list.
                        updateIterators[j] = commands[j].get().getUpdates().cbegin();
                    }
                }
                
                doneUpdates = !movedIterator;
            } while (!doneUpdates);
            
            storm::expressions::Expression newGuard = commands[0].get().getGuardExpression();
            for (uint_fast64_t i = 1; i < commands.size(); ++i) {
                newGuard = newGuard && commands[i].get().getGuardExpression();
            }
            
            return Command(newCommandIndex, false, actionIndex, actionName, newGuard, newUpdates, this->getFilename(), 0);
        }

        uint_fast64_t Program::numberOfActions() const {
            return this->actions.size();
        }

        uint_fast64_t Program::largestActionIndex() const {
            assert(numberOfActions() != 0);
            return this->indexToActionMap.rbegin()->first;
        }
        
        storm::expressions::ExpressionManager const& Program::getManager() const {
            return *this->manager;
        }

        storm::expressions::ExpressionManager& Program::getManager() {
            return *this->manager;
        }

        std::ostream& operator<<(std::ostream& out, Program::ModelType const& type) {
            switch (type) {
                case Program::ModelType::UNDEFINED: out << "undefined"; break;
                case Program::ModelType::DTMC: out << "dtmc"; break;
                case Program::ModelType::CTMC: out << "ctmc"; break;
                case Program::ModelType::MDP: out << "mdp"; break;
                case Program::ModelType::CTMDP: out << "ctmdp"; break;
                case Program::ModelType::MA: out << "ma"; break;
            }
            return out;
        }
        
        std::ostream& operator<<(std::ostream& stream, Program const& program) {
            stream << program.getModelType() << std::endl;
            for (auto const& constant : program.getConstants()) {
                stream << constant << std::endl;
            }
            stream << std::endl;
            
            for (auto const& variable : program.getGlobalBooleanVariables()) {
                stream << "global " << variable << std::endl;
            }
            for (auto const& variable : program.getGlobalIntegerVariables()) {
                stream << "global " << variable << std::endl;
            }
            stream << std::endl;
            
            for (auto const& formula : program.getFormulas()) {
                stream << formula << std::endl;
            }
            stream << std::endl;
            
            for (auto const& module : program.getModules()) {
                stream << module << std::endl;
            }
            
            for (auto const& rewardModel : program.getRewardModels()) {
                stream << rewardModel << std::endl;
            }
            
            for (auto const& label : program.getLabels()) {
                stream << label << std::endl;
            }
            
            return stream;
        }
        
        
        
    } // namespace prism
} // namepsace storm
