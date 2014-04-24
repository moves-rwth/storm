#include "src/storage/prism/Program.h"
#include "src/exceptions/ExceptionMacros.h"
#include "exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

namespace storm {
    namespace prism {
        Program::Program(ModelType modelType, std::vector<Constant> const& constants, std::vector<BooleanVariable> const& globalBooleanVariables, std::vector<IntegerVariable> const& globalIntegerVariables, std::vector<Formula> const& formulas, std::vector<Module> const& modules, std::vector<RewardModel> const& rewardModels, bool hasInitialStatesExpression, storm::expressions::Expression const& initialStatesExpression, std::vector<Label> const& labels, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), modelType(modelType), constants(constants), constantToIndexMap(), globalBooleanVariables(globalBooleanVariables), globalBooleanVariableToIndexMap(), globalIntegerVariables(globalIntegerVariables), globalIntegerVariableToIndexMap(), formulas(formulas), formulaToIndexMap(), modules(modules), moduleToIndexMap(), rewardModels(rewardModels), rewardModelToIndexMap(), hasInitialStatesExpression(hasInitialStatesExpression), initialStatesExpression(initialStatesExpression), labels(labels), labelToIndexMap(), actions(), actionsToModuleIndexMap(), variableToModuleIndexMap() {
            this->createMappings();
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
            LOG_THROW(nameIndexPair != this->globalBooleanVariableToIndexMap.end(), storm::exceptions::OutOfRangeException, "Unknown boolean variable '" << variableName << "'.");
            return this->getGlobalBooleanVariables()[nameIndexPair->second];
        }

        IntegerVariable const& Program::getGlobalIntegerVariable(std::string const& variableName) const {
            auto const& nameIndexPair = this->globalIntegerVariableToIndexMap.find(variableName);
            LOG_THROW(nameIndexPair != this->globalIntegerVariableToIndexMap.end(), storm::exceptions::OutOfRangeException, "Unknown integer variable '" << variableName << "'.");
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
            LOG_THROW(nameIndexPair != this->moduleToIndexMap.end(), storm::exceptions::OutOfRangeException, "Unknown module '" << moduleName << "'.");
            return this->getModules()[nameIndexPair->second];
        }
        
        std::vector<storm::prism::Module> const& Program::getModules() const {
            return this->modules;
        }
        
        bool Program::definesInitialStatesExpression() const {
            return this->hasInitialStatesExpression;
        }
        
        storm::expressions::Expression Program::getInitialStatesExpression() const {
            // If the program specifies the initial states explicitly, we simply return the expression.
            if (this->definesInitialStatesExpression()) {
                return this->initialStatesExpression;
            } else {
                // Otherwise, we need to assert that all variables are equal to their initial value.
                storm::expressions::Expression result = storm::expressions::Expression::createTrue();
                
                for (auto const& module : this->getModules()) {
                    for (auto const& booleanVariable : module.getBooleanVariables()) {
                        result = result && (storm::expressions::Expression::createBooleanVariable(booleanVariable.getName()).iff(booleanVariable.getInitialValueExpression()));
                    }
                    for (auto const& integerVariable : module.getIntegerVariables()) {
                        result = result && (storm::expressions::Expression::createIntegerVariable(integerVariable.getName()) == integerVariable.getInitialValueExpression());
                    }
                }
                return result;
            }
        }
        
        std::set<std::string> const& Program::getActions() const {
            return this->actions;
        }
        
        std::set<uint_fast64_t> const& Program::getModuleIndicesByAction(std::string const& action) const {
            auto const& actionModuleSetPair = this->actionsToModuleIndexMap.find(action);
            LOG_THROW(actionModuleSetPair != this->actionsToModuleIndexMap.end(), storm::exceptions::OutOfRangeException, "Action name '" << action << "' does not exist.");
            return actionModuleSetPair->second;
        }
        
        uint_fast64_t Program::getModuleIndexByVariable(std::string const& variableName) const {
            auto const& variableNameToModuleIndexPair = this->variableToModuleIndexMap.find(variableName);
            LOG_THROW(variableNameToModuleIndexPair != this->variableToModuleIndexMap.end(), storm::exceptions::OutOfRangeException, "Variable '" << variableName << "' does not exist.");
            return variableNameToModuleIndexPair->second;
        }
        
        std::vector<storm::prism::RewardModel> const& Program::getRewardModels() const {
            return this->rewardModels;
        }
        
        std::size_t Program::getNumberOfRewardModels() const {
            return this->getRewardModels().size();
        }
        
        storm::prism::RewardModel const& Program::getRewardModel(std::string const& name) const {
            auto const& nameIndexPair = this->rewardModelToIndexMap.find(name);
            LOG_THROW(nameIndexPair != this->rewardModelToIndexMap.end(), storm::exceptions::OutOfRangeException, "Reward model '" << name << "' does not exist.");
            return this->getRewardModels()[nameIndexPair->second];
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
            
            return Program(this->getModelType(), this->getConstants(), this->getGlobalBooleanVariables(), this->getGlobalIntegerVariables(), this->getFormulas(), newModules, this->getRewardModels(), this->definesInitialStatesExpression(), this->getInitialStatesExpression(), this->getLabels());
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
                    LOG_THROW(constantDefinitions.find(constant.getName()) == constantDefinitions.end(), storm::exceptions::InvalidArgumentException, "Illegally defining already defined constant '" << constant.getName() << "'.");
                    
                    // Now replace the occurrences of undefined constants in its defining expression.
                    newConstants.emplace_back(constant.getType(), constant.getName(), constant.getExpression().substitute<std::map>(constantDefinitions), constant.getFilename(), constant.getLineNumber());
                } else {
                    auto const& variableExpressionPair = constantDefinitions.find(constant.getName());
                    
                    // If the constant is not defined by the mapping, we leave it like it is.
                    if (variableExpressionPair == constantDefinitions.end()) {
                        newConstants.emplace_back(constant);
                    } else {
                        // Otherwise, we add it to the defined constants and assign it the appropriate expression.
                        definedUndefinedConstants.insert(constant.getName());
                        
                        // Make sure the type of the constant is correct.
                        LOG_THROW(variableExpressionPair->second.getReturnType() == constant.getType(), storm::exceptions::InvalidArgumentException, "Illegal type of expression defining constant '" << constant.getName() << "'.");
                        
                        // Now create the defined constant.
                        newConstants.emplace_back(constant.getType(), constant.getName(), variableExpressionPair->second, constant.getFilename(), constant.getLineNumber());
                    }
                }
            }
            
            // As a sanity check, we make sure that the given mapping does not contain any definitions for identifiers
            // that are not undefined constants.
            for (auto const& constantExpressionPair : constantDefinitions) {
                LOG_THROW(definedUndefinedConstants.find(constantExpressionPair.first) != definedUndefinedConstants.end(), storm::exceptions::InvalidArgumentException, "Unable to define non-existant constant.");
            }
            
            return Program(this->getModelType(), newConstants, this->getGlobalBooleanVariables(), this->getGlobalIntegerVariables(), this->getFormulas(), this->getModules(), this->getRewardModels(), this->definesInitialStatesExpression(), this->getInitialStatesExpression(), this->getLabels());
        }
        
        Program Program::substituteConstants() const {
            // We start by creating the appropriate substitution
            std::map<std::string, storm::expressions::Expression> constantSubstitution;
            std::vector<Constant> newConstants(this->getConstants());
            for (uint_fast64_t constantIndex = 0; constantIndex < newConstants.size(); ++constantIndex) {
                auto const& constant = newConstants[constantIndex];
                //LOG_THROW(constant.isDefined(), storm::exceptions::InvalidArgumentException, "Cannot substitute constants in program that contains undefined constants.");
                
                // Put the corresponding expression in the substitution.
                if(constant.isDefined())
                {
                    constantSubstitution.emplace(constant.getName(), constant.getExpression());
                
                
                    // If there is at least one more constant to come, we substitute the costants we have so far.
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
            
            storm::expressions::Expression newInitialStateExpression = this->getInitialStatesExpression().substitute<std::map>(constantSubstitution);
            
            std::vector<Label> newLabels;
            newLabels.reserve(this->getNumberOfLabels());
            for (auto const& label : this->getLabels()) {
                newLabels.emplace_back(label.substitute(constantSubstitution));
            }
            
            return Program(this->getModelType(), newConstants, newBooleanVariables, newIntegerVariables, newFormulas, newModules, newRewardModels, this->definesInitialStatesExpression(), newInitialStateExpression, newLabels);
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
        
    } // namespace ir
} // namepsace storm
