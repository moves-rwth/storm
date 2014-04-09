#include "src/storage/prism/Program.h"
#include "src/exceptions/ExceptionMacros.h"
#include "exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

namespace storm {
    namespace prism {
        Program::Program(ModelType modelType, std::vector<Constant> const& undefinedConstants, std::vector<Constant> const& definedConstants, std::vector<BooleanVariable> const& globalBooleanVariables, std::vector<IntegerVariable> const& globalIntegerVariables, std::vector<Formula> const& formulas, std::vector<Module> const& modules, std::vector<RewardModel> const& rewardModels, bool hasInitialStatesExpression, storm::expressions::Expression const& initialStatesExpression, std::vector<Label> const& labels, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), modelType(modelType), undefinedConstants(undefinedConstants), definedConstants(definedConstants), globalBooleanVariables(globalBooleanVariables), globalBooleanVariableToIndexMap(), globalIntegerVariables(globalIntegerVariables), globalIntegerVariableToIndexMap(), formulas(formulas), formulaToIndexMap(), modules(modules), moduleToIndexMap(), rewardModels(rewardModels), rewardModelToIndexMap(), hasInitialStatesExpression(hasInitialStatesExpression), initialStatesExpression(initialStatesExpression), labels(labels), labelToIndexMap(), actions(), actionsToModuleIndexMap(), variableToModuleIndexMap() {
            // FIXME: build the mappings for constants, formulas, modules, global variables, reward models and labels.
            // Now build the mapping from action names to module indices so that the lookup can later be performed quickly.
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
        
        Program::ModelType Program::getModelType() const {
            return modelType;
        }
        
        bool Program::hasUndefinedConstants() const {
            return this->undefinedConstants.size() > 0;
        }
        
        std::vector<Constant> const& Program::getUndefinedConstants() const {
            return this->undefinedConstants;
        }
        
        std::vector<Constant> const& Program::getDefinedConstants() const {
            return this->definedConstants;
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
        
        storm::prism::RewardModel const& Program::getRewardModel(std::string const& name) const {
            auto const& nameIndexPair = this->rewardModelToIndexMap.find(name);
            LOG_THROW(nameIndexPair != this->rewardModelToIndexMap.end(), storm::exceptions::OutOfRangeException, "Reward model '" << name << "' does not exist.");
            return this->getRewardModels()[nameIndexPair->second];
        }
        
        std::vector<Label> const& Program::getLabels() const {
            return this->labels;
        }
        
        Program Program::restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet) {
            std::vector<storm::prism::Module> newModules;
            newModules.reserve(this->getNumberOfModules());
            
            for (auto const& module : this->getModules()) {
                newModules.push_back(module.restrictCommands(indexSet));
            }
            
            return Program(this->getModelType(), this->getUndefinedConstants(), this->getDefinedConstants(), this->getGlobalBooleanVariables(), this->getGlobalIntegerVariables(), this->getFormulas(), newModules, this->getRewardModels(), this->definesInitialStatesExpression(), this->getInitialStatesExpression(), this->getLabels());
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
            
            for (auto const& constant : program.getUndefinedConstants()) {
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
