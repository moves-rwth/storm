#include "src/storage/prism/Program.h"

#include <algorithm>

#include "src/utility/macros.h"
#include "exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace prism {
        Program::Program(ModelType modelType, std::vector<Constant> const& constants, std::vector<BooleanVariable> const& globalBooleanVariables, std::vector<IntegerVariable> const& globalIntegerVariables, std::vector<Formula> const& formulas, std::vector<Module> const& modules, std::vector<RewardModel> const& rewardModels, bool fixInitialConstruct, storm::prism::InitialConstruct const& initialConstruct, std::vector<Label> const& labels, std::string const& filename, uint_fast64_t lineNumber, bool checkValidity) : LocatedInformation(filename, lineNumber), modelType(modelType), constants(constants), constantToIndexMap(), globalBooleanVariables(globalBooleanVariables), globalBooleanVariableToIndexMap(), globalIntegerVariables(globalIntegerVariables), globalIntegerVariableToIndexMap(), formulas(formulas), formulaToIndexMap(), modules(modules), moduleToIndexMap(), rewardModels(rewardModels), rewardModelToIndexMap(), initialConstruct(initialConstruct), labels(labels), labelToIndexMap(), actions(), actionsToModuleIndexMap(), variableToModuleIndexMap() {
            this->createMappings();
            
            // Create a new initial construct if the corresponding flag was set.
            if (fixInitialConstruct) {
                if (this->getInitialConstruct().getInitialStatesExpression().isFalse()) {
                    storm::expressions::Expression newInitialExpression = storm::expressions::Expression::createTrue();
                    
                    for (auto const& booleanVariable : this->getGlobalBooleanVariables()) {
                        newInitialExpression = newInitialExpression && (storm::expressions::Expression::createBooleanVariable(booleanVariable.getName()).iff(booleanVariable.getInitialValueExpression()));
                    }
                    for (auto const& integerVariable : this->getGlobalIntegerVariables()) {
                        newInitialExpression = newInitialExpression && (storm::expressions::Expression::createIntegerVariable(integerVariable.getName()) == integerVariable.getInitialValueExpression());
                    }
                    for (auto const& module : this->getModules()) {
                        for (auto const& booleanVariable : module.getBooleanVariables()) {
                            newInitialExpression = newInitialExpression && (storm::expressions::Expression::createBooleanVariable(booleanVariable.getName()).iff(booleanVariable.getInitialValueExpression()));
                        }
                        for (auto const& integerVariable : module.getIntegerVariables()) {
                            newInitialExpression = newInitialExpression && (storm::expressions::Expression::createIntegerVariable(integerVariable.getName()) == integerVariable.getInitialValueExpression());
                        }
                    }
                    this->initialConstruct = storm::prism::InitialConstruct(newInitialExpression, this->getInitialConstruct().getFilename(), this->getInitialConstruct().getLineNumber());
                }
            }
            
            if (checkValidity) {
                this->checkValidity();
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
        
        std::size_t Program::getNumberOfConstants() const {
            return this->getConstants().size();
        }

        std::vector<BooleanVariable> const& Program::getGlobalBooleanVariables() const {
            return this->globalBooleanVariables;
        }
        
        std::vector<IntegerVariable> const& Program::getGlobalIntegerVariables() const {
            return this->globalIntegerVariables;
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
        
        storm::prism::InitialConstruct const& Program::getInitialConstruct() const {
            return this->initialConstruct;
        }
        
        std::set<std::string> const& Program::getActions() const {
            return this->actions;
        }
        
        std::set<uint_fast64_t> const& Program::getModuleIndicesByAction(std::string const& action) const {
            auto const& actionModuleSetPair = this->actionsToModuleIndexMap.find(action);
            STORM_LOG_THROW(actionModuleSetPair != this->actionsToModuleIndexMap.end(), storm::exceptions::OutOfRangeException, "Action name '" << action << "' does not exist.");
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
        
        std::vector<Label> const& Program::getLabels() const {
            return this->labels;
        }
        
        std::size_t Program::getNumberOfLabels() const {
            return this->getLabels().size();
        }
        
        Program Program::restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet) const {
            std::vector<storm::prism::Module> newModules;
            newModules.reserve(this->getNumberOfModules());
            
            for (auto const& module : this->getModules()) {
                newModules.push_back(module.restrictCommands(indexSet));
            }
            
            return Program(this->getModelType(), this->getConstants(), this->getGlobalBooleanVariables(), this->getGlobalIntegerVariables(), this->getFormulas(), newModules, this->getRewardModels(), false, this->getInitialConstruct(), this->getLabels());
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
            for (uint_fast64_t moduleIndex = 0; moduleIndex < this->getNumberOfModules(); ++moduleIndex) {
                this->moduleToIndexMap[this->getModules()[moduleIndex].getName()] = moduleIndex;
            }
            for (uint_fast64_t rewardModelIndex = 0; rewardModelIndex < this->getNumberOfRewardModels(); ++rewardModelIndex) {
                this->rewardModelToIndexMap[this->getRewardModels()[rewardModelIndex].getName()] = rewardModelIndex;
            }
            for (uint_fast64_t labelIndex = 0; labelIndex < this->getNumberOfLabels(); ++labelIndex) {
                this->labelToIndexMap[this->getLabels()[labelIndex].getName()] = labelIndex;
            }
            
            // Build the mapping from action names to module indices so that the lookup can later be performed quickly.
            for (unsigned int moduleIndex = 0; moduleIndex < this->getNumberOfModules(); moduleIndex++) {
                Module const& module = this->getModule(moduleIndex);
                
                for (auto const& action : module.getActions()) {
                    auto const& actionModuleIndicesPair = this->actionsToModuleIndexMap.find(action);
                    if (actionModuleIndicesPair == this->actionsToModuleIndexMap.end()) {
                        this->actionsToModuleIndexMap[action] = std::set<uint_fast64_t>();
                    }
                    this->actionsToModuleIndexMap[action].insert(moduleIndex);
                    this->actions.insert(action);
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
        
        Program Program::defineUndefinedConstants(std::map<std::string, storm::expressions::Expression> const& constantDefinitions) const {
            // For sanity checking, we keep track of all undefined constants that we define in the course of this
            // procedure.
            std::set<std::string> definedUndefinedConstants;
            
            std::vector<Constant> newConstants;
            newConstants.reserve(this->getNumberOfConstants());
            for (auto const& constant : this->getConstants()) {
                // If the constant is already defined, we need to replace the appearances of undefined constants in its
                // defining expression
                if (constant.isDefined()) {
                    // Make sure we are not trying to define an already defined constant.
                    STORM_LOG_THROW(constantDefinitions.find(constant.getName()) == constantDefinitions.end(), storm::exceptions::InvalidArgumentException, "Illegally defining already defined constant '" << constant.getName() << "'.");
                    
                    // Now replace the occurrences of undefined constants in its defining expression.
                    newConstants.emplace_back(constant.getType(), constant.getName(), constant.getExpression().substitute(constantDefinitions), constant.getFilename(), constant.getLineNumber());
                } else {
                    auto const& variableExpressionPair = constantDefinitions.find(constant.getName());
                    
                    // If the constant is not defined by the mapping, we leave it like it is.
                    if (variableExpressionPair == constantDefinitions.end()) {
                        newConstants.emplace_back(constant);
                    } else {
                        // Otherwise, we add it to the defined constants and assign it the appropriate expression.
                        definedUndefinedConstants.insert(constant.getName());
                        
                        // Make sure the type of the constant is correct.
                        STORM_LOG_THROW(variableExpressionPair->second.getReturnType() == constant.getType(), storm::exceptions::InvalidArgumentException, "Illegal type of expression defining constant '" << constant.getName() << "'.");
                        
                        // Now create the defined constant.
                        newConstants.emplace_back(constant.getType(), constant.getName(), variableExpressionPair->second, constant.getFilename(), constant.getLineNumber());
                    }
                }
            }
            
            // As a sanity check, we make sure that the given mapping does not contain any definitions for identifiers
            // that are not undefined constants.
            for (auto const& constantExpressionPair : constantDefinitions) {
                STORM_LOG_THROW(definedUndefinedConstants.find(constantExpressionPair.first) != definedUndefinedConstants.end(), storm::exceptions::InvalidArgumentException, "Unable to define non-existant constant.");
            }
            
            return Program(this->getModelType(), newConstants, this->getGlobalBooleanVariables(), this->getGlobalIntegerVariables(), this->getFormulas(), this->getModules(), this->getRewardModels(), false, this->getInitialConstruct(), this->getLabels());
        }
        
        Program Program::substituteConstants() const {
            // We start by creating the appropriate substitution
            std::map<std::string, storm::expressions::Expression> constantSubstitution;
            std::vector<Constant> newConstants(this->getConstants());
            for (uint_fast64_t constantIndex = 0; constantIndex < newConstants.size(); ++constantIndex) {
                auto const& constant = newConstants[constantIndex];
                STORM_LOG_THROW(constant.isDefined(), storm::exceptions::InvalidArgumentException, "Cannot substitute constants in program that contains undefined constants.");
                
                // Put the corresponding expression in the substitution.
                constantSubstitution.emplace(constant.getName(), constant.getExpression());
                
                // If there is at least one more constant to come, we substitute the costants we have so far.
                if (constantIndex + 1 < newConstants.size()) {
                    newConstants[constantIndex + 1] = newConstants[constantIndex + 1].substitute(constantSubstitution);
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
            
            return Program(this->getModelType(), newConstants, newBooleanVariables, newIntegerVariables, newFormulas, newModules, newRewardModels, false, newInitialConstruct, newLabels);
        }
        
        void Program::checkValidity() const {
            // We need to construct a mapping from identifiers to their types, so we can type-check the expressions later.
            std::map<std::string, storm::expressions::ExpressionReturnType> identifierToTypeMap;
            
            // Start by checking the constant declarations.
            std::set<std::string> allIdentifiers;
            std::set<std::string> globalIdentifiers;
            std::set<std::string> constantNames;
            for (auto const& constant : this->getConstants()) {
                // Check for duplicate identifiers.
                STORM_LOG_THROW(allIdentifiers.find(constant.getName()) == allIdentifiers.end(), storm::exceptions::WrongFormatException, "Error in " << constant.getFilename() << ", line " << constant.getLineNumber() << ": duplicate identifier '" << constant.getName() << "'.");
                
                // Check defining expressions of defined constants.
                if (constant.isDefined()) {
                    std::set<std::string> containedIdentifiers = constant.getExpression().getVariables();
                    bool isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << constant.getFilename() << ", line " << constant.getLineNumber() << ": defining expression refers to unknown identifiers.");
                }
                
                // Finally, register the type of the constant for later type checks.
                identifierToTypeMap.emplace(constant.getName(), constant.getType());
                
                // Record the new identifier for future checks.
                constantNames.insert(constant.getName());
                allIdentifiers.insert(constant.getName());
                globalIdentifiers.insert(constant.getName());
            }
            
            // Now we check the variable declarations. We start with the global variables.
            std::set<std::string> variableNames;
            for (auto const& variable : this->getGlobalBooleanVariables()) {
                // Check for duplicate identifiers.
                STORM_LOG_THROW(allIdentifiers.find(variable.getName()) == allIdentifiers.end(), storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": duplicate identifier '" << variable.getName() << "'.");
                
                // Check the initial value of the variable.
                std::set<std::string> containedIdentifiers = variable.getInitialValueExpression().getVariables();
                bool isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": initial value expression refers to unknown constants.");

                // Register the type of the constant for later type checks.
                identifierToTypeMap.emplace(variable.getName(), storm::expressions::ExpressionReturnType::Bool);
                
                // Record the new identifier for future checks.
                variableNames.insert(variable.getName());
                allIdentifiers.insert(variable.getName());
                globalIdentifiers.insert(variable.getName());
            }
            for (auto const& variable : this->getGlobalIntegerVariables()) {
                // Check for duplicate identifiers.
                STORM_LOG_THROW(allIdentifiers.find(variable.getName()) == allIdentifiers.end(), storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": duplicate identifier '" << variable.getName() << "'.");
                
                // Check that bound expressions of the range.
                std::set<std::string> containedIdentifiers = variable.getLowerBoundExpression().getVariables();
                bool isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": lower bound expression refers to unknown constants.");

                containedIdentifiers = variable.getLowerBoundExpression().getVariables();
                isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": upper bound expression refers to unknown constants.");
                
                // Check the initial value of the variable.
                containedIdentifiers = variable.getInitialValueExpression().getVariables();
                isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": initial value expression refers to unknown constants.");
                
                // Register the type of the constant for later type checks.
                identifierToTypeMap.emplace(variable.getName(), storm::expressions::ExpressionReturnType::Int);

                // Record the new identifier for future checks.
                variableNames.insert(variable.getName());
                allIdentifiers.insert(variable.getName());
                globalIdentifiers.insert(variable.getName());
            }

            // Now go through the variables of the modules.
            for (auto const& module : this->getModules()) {
                for (auto const& variable : module.getBooleanVariables()) {
                    // Check for duplicate identifiers.
                    STORM_LOG_THROW(allIdentifiers.find(variable.getName()) == allIdentifiers.end(), storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": duplicate identifier '" << variable.getName() << "'.");
                    
                    // Check the initial value of the variable.
                    std::set<std::string> containedIdentifiers = variable.getInitialValueExpression().getVariables();
                    bool isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": initial value expression refers to unknown constants.");
                    
                    // Register the type of the constant for later type checks.
                    identifierToTypeMap.emplace(variable.getName(), storm::expressions::ExpressionReturnType::Bool);
                    
                    // Record the new identifier for future checks.
                    variableNames.insert(variable.getName());
                    allIdentifiers.insert(variable.getName());
                }
                for (auto const& variable : module.getIntegerVariables()) {
                    // Check for duplicate identifiers.
                    STORM_LOG_THROW(allIdentifiers.find(variable.getName()) == allIdentifiers.end(), storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": duplicate identifier '" << variable.getName() << "'.");
                    
                    // Register the type of the constant for later type checks.
                    identifierToTypeMap.emplace(variable.getName(), storm::expressions::ExpressionReturnType::Int);
                    
                    // Check that bound expressions of the range.
                    std::set<std::string> containedIdentifiers = variable.getLowerBoundExpression().getVariables();
                    bool isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": lower bound expression refers to unknown constants.");

                    containedIdentifiers = variable.getLowerBoundExpression().getVariables();
                    isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": upper bound expression refers to unknown constants.");
                    
                    // Check the initial value of the variable.
                    containedIdentifiers = variable.getInitialValueExpression().getVariables();
                    isValid = std::includes(constantNames.begin(), constantNames.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << variable.getFilename() << ", line " << variable.getLineNumber() << ": initial value expression refers to unknown constants.");
                    
                    // Record the new identifier for future checks.
                    variableNames.insert(variable.getName());
                    allIdentifiers.insert(variable.getName());
                }
            }
            
            // Create the set of valid identifiers for future checks.
            std::set<std::string> variablesAndConstants;
            std::set_union(variableNames.begin(), variableNames.end(), constantNames.begin(), constantNames.end(), std::inserter(variablesAndConstants, variablesAndConstants.begin()));
            
            // Check the commands of the modules.
            for (auto const& module : this->getModules()) {
                std::set<std::string> legalIdentifiers = globalIdentifiers;
                for (auto const& variable : module.getBooleanVariables()) {
                    legalIdentifiers.insert(variable.getName());
                }
                for (auto const& variable : module.getIntegerVariables()) {
                    legalIdentifiers.insert(variable.getName());
                }
                
                for (auto const& command : module.getCommands()) {
                    // Check the guard.
                    std::set<std::string> containedIdentifiers = command.getGuardExpression().getVariables();
                    bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": guard refers to unknown identifiers.");
                    STORM_LOG_THROW(command.getGuardExpression().hasBooleanReturnType(), storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": expression for guard must evaluate to type 'bool'.");
                    
                    // Check all updates.
                    for (auto const& update : command.getUpdates()) {
                        containedIdentifiers = update.getLikelihoodExpression().getVariables();
                        isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                        STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": likelihood expression refers to unknown identifiers.");
                        
                        // Check all assignments.
                        std::set<std::string> alreadyAssignedIdentifiers;
                        for (auto const& assignment : update.getAssignments()) {
                            if (legalIdentifiers.find(assignment.getVariableName()) == legalIdentifiers.end()) {
                                if (allIdentifiers.find(assignment.getVariableName()) != allIdentifiers.end()) {
                                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": assignment illegally refers to variable '" << assignment.getVariableName() << "'.");
                                } else {
                                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": assignment refers to unknown variable '" << assignment.getVariableName() << "'.");
                                }
                            }
                            STORM_LOG_THROW(alreadyAssignedIdentifiers.find(assignment.getVariableName()) == alreadyAssignedIdentifiers.end(), storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": duplicate assignment to variable '" << assignment.getVariableName() << "'.");
                            auto variableTypePair = identifierToTypeMap.find(assignment.getVariableName());
                            STORM_LOG_THROW(variableTypePair->second == assignment.getExpression().getReturnType(), storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": illegally assigning a value of type '" << assignment.getExpression().getReturnType() << "' to variable '" << variableTypePair->first << "' of type '" << variableTypePair->second << "'.");
                            
                            containedIdentifiers = assignment.getExpression().getVariables();
                            isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                            STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << command.getFilename() << ", line " << command.getLineNumber() << ": likelihood expression refers to unknown identifiers.");
                            
                            // Add the current variable to the set of assigned variables (of this update).
                            alreadyAssignedIdentifiers.insert(assignment.getVariableName());
                        }
                    }
                }
            }
            
            // Now check the reward models.
            for (auto const& rewardModel : this->getRewardModels()) {
                for (auto const& stateReward : rewardModel.getStateRewards()) {
                    std::set<std::string> containedIdentifiers = stateReward.getStatePredicateExpression().getVariables();
                    bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << stateReward.getFilename() << ", line " << stateReward.getLineNumber() << ": state reward expression refers to unknown identifiers.");
                    STORM_LOG_THROW(stateReward.getStatePredicateExpression().hasBooleanReturnType(), storm::exceptions::WrongFormatException, "Error in " << stateReward.getFilename() << ", line " << stateReward.getLineNumber() << ": state predicate must evaluate to type 'bool'.");
                    
                    containedIdentifiers = stateReward.getRewardValueExpression().getVariables();
                    isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << stateReward.getFilename() << ", line " << stateReward.getLineNumber() << ": state reward value expression refers to unknown identifiers.");
                    STORM_LOG_THROW(stateReward.getRewardValueExpression().hasNumericalReturnType(), storm::exceptions::WrongFormatException, "Error in " << stateReward.getFilename() << ", line " << stateReward.getLineNumber() << ": reward value expression must evaluate to numerical type.");
                }
                
                for (auto const& transitionReward : rewardModel.getTransitionRewards()) {
                    std::set<std::string> containedIdentifiers = transitionReward.getStatePredicateExpression().getVariables();
                    bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": state reward expression refers to unknown identifiers.");
                    STORM_LOG_THROW(transitionReward.getStatePredicateExpression().hasBooleanReturnType(), storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": state predicate must evaluate to type 'bool'.");
                    
                    containedIdentifiers = transitionReward.getRewardValueExpression().getVariables();
                    isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                    STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": state reward value expression refers to unknown identifiers.");
                    STORM_LOG_THROW(transitionReward.getRewardValueExpression().hasNumericalReturnType(), storm::exceptions::WrongFormatException, "Error in " << transitionReward.getFilename() << ", line " << transitionReward.getLineNumber() << ": reward value expression must evaluate to numerical type.");
                }
            }
            
            // Check the initial states expression.
            std::set<std::string> containedIdentifiers = this->getInitialConstruct().getInitialStatesExpression().getVariables();
            bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
            STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << this->getInitialConstruct().getFilename() << ", line " << this->getInitialConstruct().getLineNumber() << ": initial expression refers to unknown identifiers.");
            
            // Check the labels.
            for (auto const& label : this->getLabels()) {
                // Check for duplicate identifiers.
                STORM_LOG_THROW(allIdentifiers.find(label.getName()) == allIdentifiers.end(), storm::exceptions::WrongFormatException, "Error in " << label.getFilename() << ", line " << label.getLineNumber() << ": duplicate identifier '" << label.getName() << "'.");
                
                std::set<std::string> containedIdentifiers = label.getStatePredicateExpression().getVariables();
                bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << label.getFilename() << ", line " << label.getLineNumber() << ": label expression refers to unknown identifiers.");
                STORM_LOG_THROW(label.getStatePredicateExpression().hasBooleanReturnType(), storm::exceptions::WrongFormatException, "Error in " << label.getFilename() << ", line " << label.getLineNumber() << ": label predicate must evaluate to type 'bool'.");
            }
            
            // Check the formulas.
            for (auto const& formula : this->getFormulas()) {
                // Check for duplicate identifiers.
                STORM_LOG_THROW(allIdentifiers.find(formula.getName()) == allIdentifiers.end(), storm::exceptions::WrongFormatException, "Error in " << formula.getFilename() << ", line " << formula.getLineNumber() << ": duplicate identifier '" << formula.getName() << "'.");
                
                std::set<std::string> containedIdentifiers = formula.getExpression().getVariables();
                bool isValid = std::includes(variablesAndConstants.begin(), variablesAndConstants.end(), containedIdentifiers.begin(), containedIdentifiers.end());
                STORM_LOG_THROW(isValid, storm::exceptions::WrongFormatException, "Error in " << formula.getFilename() << ", line " << formula.getLineNumber() << ": formula expression refers to unknown identifiers.");
                
                // Record the new identifier for future checks.
                allIdentifiers.insert(formula.getName());
            }
        }
        
        std::ostream& operator<<(std::ostream& stream, Program const& program) {
            switch (program.getModelType()) {
                case Program::ModelType::UNDEFINED: stream << "undefined"; break;
                case Program::ModelType::DTMC: stream << "dtmc"; break;
                case Program::ModelType::CTMC: stream << "ctmc"; break;
                case Program::ModelType::MDP: stream << "mdp"; break;
                case Program::ModelType::CTMDP: stream << "ctmdp"; break;
                case Program::ModelType::MA: stream << "ma"; break;
            }
            stream << std::endl;
            
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
