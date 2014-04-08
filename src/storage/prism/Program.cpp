#include "src/storage/prism/Program.h"
#include "src/exceptions/ExceptionMacros.h"
#include "exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

namespace storm {
    namespace prism {
        Program::Program(ModelType modelType, std::set<std::string> const& undefinedBooleanConstants, std::map<std::string, storm::expressions::Expression> definedBooleanConstants, std::set<std::string> const& undefinedIntegerConstants, std::map<std::string, storm::expressions::Expression> definedIntegerConstants, std::set<std::string> const& undefinedDoubleConstants, std::map<std::string, storm::expressions::Expression> definedDoubleConstants, std::map<std::string, BooleanVariable> const& globalBooleanVariables, std::map<std::string, IntegerVariable> const& globalIntegerVariables, std::vector<storm::prism::Module> const& modules, std::map<std::string, storm::prism::RewardModel> const& rewardModels, bool hasInitialStatesExpression, storm::expressions::Expression const& initialStatesExpression, std::map<std::string, storm::expressions::Expression> const& labels, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), modelType(modelType), undefinedBooleanConstants(undefinedBooleanConstants), definedBooleanConstants(definedBooleanConstants), undefinedIntegerConstants(undefinedIntegerConstants), definedIntegerConstants(definedIntegerConstants), undefinedDoubleConstants(undefinedDoubleConstants), definedDoubleConstants(definedDoubleConstants), globalBooleanVariables(globalBooleanVariables), globalIntegerVariables(globalIntegerVariables), modules(modules), rewardModels(rewardModels), hasInitialStatesExpression(hasInitialStatesExpression), initialStatesExpression(initialStatesExpression), labels(labels), actions(), actionsToModuleIndexMap(), variableToModuleIndexMap() {
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
                    this->variableToModuleIndexMap[booleanVariable.first] = moduleIndex;
                }
                for (auto const& integerVariable : module.getBooleanVariables()) {
                    this->variableToModuleIndexMap[integerVariable.first] = moduleIndex;
                }
            }
        }
        
        Program::ModelType Program::getModelType() const {
            return modelType;
        }
        
        bool Program::hasUndefinedConstants() const {
            return this->hasUndefinedBooleanConstants() || this->hasUndefinedIntegerConstants() || this->hasUndefinedDoubleConstants();
        }
        
        bool Program::hasUndefinedBooleanConstants() const {
            return !this->undefinedBooleanConstants.empty();
        }
        
        bool Program::hasUndefinedIntegerConstants() const {
            return !this->undefinedIntegerConstants.empty();
        }
        
        bool Program::hasUndefinedDoubleConstants() const {
            return !this->undefinedDoubleConstants.empty();
        }
        
        std::set<std::string> const& Program::getUndefinedBooleanConstants() const {
            return this->undefinedBooleanConstants;
        }
        
        std::map<std::string, storm::expressions::Expression> const& Program::getDefinedBooleanConstants() const {
            return this->definedBooleanConstants;
        }

        std::set<std::string> const& Program::getUndefinedIntegerConstants() const {
            return this->undefinedIntegerConstants;
        }
        
        std::map<std::string, storm::expressions::Expression> const& Program::getDefinedIntegerConstants() const {
            return this->definedIntegerConstants;
        }

        std::set<std::string> const& Program::getUndefinedDoubleConstants() const {
            return this->undefinedDoubleConstants;
        }

        std::map<std::string, storm::expressions::Expression> const& Program::getDefinedDoubleConstants() const {
            return this->definedDoubleConstants;
        }

        std::map<std::string, storm::prism::BooleanVariable> const& Program::getGlobalBooleanVariables() const {
            return this->globalBooleanVariables;
        }

        storm::prism::BooleanVariable const& Program::getGlobalBooleanVariable(std::string const& variableName) const {
            auto const& nameVariablePair = this->getGlobalBooleanVariables().find(variableName);
            LOG_THROW(nameVariablePair != this->getGlobalBooleanVariables().end(), storm::exceptions::OutOfRangeException, "Unknown boolean variable '" << variableName << "'.");
            return nameVariablePair->second;
        }
        
        std::map<std::string, storm::prism::IntegerVariable> const& Program::getGlobalIntegerVariables() const {
            return this->globalIntegerVariables;
        }

        storm::prism::IntegerVariable const& Program::getGlobalIntegerVariable(std::string const& variableName) const {
            auto const& nameVariablePair = this->getGlobalIntegerVariables().find(variableName);
            LOG_THROW(nameVariablePair != this->getGlobalIntegerVariables().end(), storm::exceptions::OutOfRangeException, "Unknown integer variable '" << variableName << "'.");
            return nameVariablePair->second;
        }
        
        std::size_t Program::getNumberOfGlobalBooleanVariables() const {
            return this->getGlobalBooleanVariables().size();
        }
        
        std::size_t Program::getNumberOfGlobalIntegerVariables() const {
            return this->getGlobalIntegerVariables().size();
        }
        
        std::size_t Program::getNumberOfModules() const {
            return this->getModules().size();
        }
        
        storm::prism::Module const& Program::getModule(uint_fast64_t index) const {
            return this->modules[index];
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
                        result = result && (storm::expressions::Expression::createBooleanVariable(booleanVariable.second.getName()).iff(booleanVariable.second.getInitialValueExpression()));
                    }
                    for (auto const& integerVariable : module.getIntegerVariables()) {
                        result = result && (storm::expressions::Expression::createIntegerVariable(integerVariable.second.getName()) == integerVariable.second.getInitialValueExpression());
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
        
        std::map<std::string, storm::prism::RewardModel> const& Program::getRewardModels() const {
            return this->rewardModels;
        }
        
        storm::prism::RewardModel const& Program::getRewardModel(std::string const& name) const {
            auto const& nameRewardModelPair = this->getRewardModels().find(name);
            LOG_THROW(nameRewardModelPair != this->getRewardModels().end(), storm::exceptions::OutOfRangeException, "Reward model '" << name << "' does not exist.");
            return nameRewardModelPair->second;
        }
        
        std::map<std::string, storm::expressions::Expression> const& Program::getLabels() const {
            return this->labels;
        }
        
        Program Program::restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet) {
            std::vector<storm::prism::Module> newModules;
            newModules.reserve(this->getNumberOfModules());
            
            for (auto const& module : this->getModules()) {
                newModules.push_back(module.restrictCommands(indexSet));
            }
            
            return Program(this->getModelType(), this->getUndefinedBooleanConstants(), this->getUndefinedIntegerConstants(), this->getUndefinedDoubleConstants(), this->getGlobalBooleanVariables(), this->getGlobalIntegerVariables(), newModules, this->getRewardModels(), this->definesInitialStatesExpression(), this->getInitialStatesExpression(), this->getLabels());
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
            
            for (auto const& element : program.getUndefinedBooleanConstants()) {
                stream << "const bool " << element << ";" << std::endl;
            }
            for (auto const& element : program.getUndefinedIntegerConstants()) {
                stream << "const int " << element << ";" << std::endl;
            }
            for (auto const& element : program.getUndefinedDoubleConstants()) {
                stream << "const double " << element << ";" << std::endl;
            }
            stream << std::endl;
            
            for (auto const& element : program.getGlobalBooleanVariables()) {
                stream << "global " << element.second << std::endl;
            }
            for (auto const& element : program.getGlobalIntegerVariables()) {
                stream << "global " << element.second << std::endl;
            }
            stream << std::endl;
            
            for (auto const& module : program.getModules()) {
                stream << module << std::endl;
            }
            
            for (auto const& rewardModel : program.getRewardModels()) {
                stream << rewardModel.second << std::endl;
            }
            
            for (auto const& label : program.getLabels()) {
                stream << "label \"" << label.first << "\" = " << label.second <<";" << std::endl;
            }
            
            return stream;
        }
        
    } // namespace ir
} // namepsace storm
