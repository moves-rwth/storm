#include "src/storage/prism/Module.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace prism {
        Module::Module(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), moduleName(moduleName), booleanVariables(booleanVariables), booleanVariableToIndexMap(), integerVariables(integerVariables), integerVariableToIndexMap(), commands(commands), actions(), actionsToCommandIndexMap() {
            // Initialize the internal mappings for fast information retrieval.
            this->createMappings();
        }
        
        Module::Module(Module const& oldModule, std::string const& newModuleName, uint_fast64_t newGlobalCommandIndex, uint_fast64_t newGlobalUpdateIndex, std::map<std::string, std::string> const& renaming, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), moduleName(newModuleName), booleanVariables(), integerVariables(), commands(), actions(), actionsToCommandIndexMap() {
            // Iterate over boolean variables and rename them. If a variable was not renamed, this is an error and an exception is thrown.
            for (auto const& booleanVariable : oldModule.getBooleanVariables()) {
                auto const& renamingPair = renaming.find(booleanVariable.getName());
                LOG_THROW(renamingPair != renaming.end(), storm::exceptions::InvalidArgumentException, "Boolean variable '" << booleanVariable.getName() << " was not renamed.");
                this->booleanVariables.emplace_back(booleanVariable, renamingPair->second, renaming, filename, lineNumber);
            }
           
            // Now do the same for the integer variables.
            for (auto const& integerVariable : oldModule.getIntegerVariables()) {
                auto const& renamingPair = renaming.find(integerVariable.getName());
                LOG_THROW(renamingPair != renaming.end(), storm::exceptions::InvalidArgumentException, "Integer variable '" << integerVariable.getName() << " was not renamed.");
                this->integerVariables.emplace_back(integerVariable, renamingPair->second, renaming, filename, lineNumber);
            }
            
            // Now we are ready to clone all commands and rename them if requested.
            this->commands.reserve(oldModule.getNumberOfCommands());
            for (Command const& command : oldModule.getCommands()) {
                this->commands.emplace_back(command, newGlobalCommandIndex, newGlobalUpdateIndex, renaming, filename, lineNumber);
                ++newGlobalCommandIndex;
                newGlobalUpdateIndex += this->commands.back().getNumberOfUpdates();
            }
            
            // Finally, update internal mappings.
            this->createMappings();
        }
        
        std::size_t Module::getNumberOfBooleanVariables() const {
            return this->booleanVariables.size();
        }
        
        std::size_t Module::getNumberOfIntegerVariables() const {
            return this->integerVariables.size();
        }
        
        storm::prism::BooleanVariable const& Module::getBooleanVariable(std::string const& variableName) const {
            auto const& nameIndexPair = this->booleanVariableToIndexMap.find(variableName);
            LOG_THROW(nameIndexPair != this->booleanVariableToIndexMap.end(), storm::exceptions::InvalidArgumentException, "Unknown boolean variable '" << variableName << "'.");
            return this->getBooleanVariables()[nameIndexPair->second];
        }
        
        std::vector<storm::prism::BooleanVariable> const& Module::getBooleanVariables() const {
            return this->booleanVariables;
        }

        storm::prism::IntegerVariable const& Module::getIntegerVariable(std::string const& variableName) const {
            auto const& nameIndexPair = this->integerVariableToIndexMap.find(variableName);
            LOG_THROW(nameIndexPair != this->integerVariableToIndexMap.end(), storm::exceptions::InvalidArgumentException, "Unknown integer variable '" << variableName << "'.");
            return this->getIntegerVariables()[nameIndexPair->second];
        }
        
        std::vector<storm::prism::IntegerVariable> const& Module::getIntegerVariables() const {
            return this->integerVariables;
        }
        
        std::size_t Module::getNumberOfCommands() const {
            return this->commands.size();
        }
        
        std::size_t Module::getNumberOfUpdates() const {
            std::size_t result = 0;
            for (auto const& command : this->getCommands()) {
                result += command.getNumberOfUpdates();
            }
            return result;
        }
        
        storm::prism::Command const& Module::getCommand(uint_fast64_t index) const {
            return this->commands[index];
        }
        
        std::vector<storm::prism::Command> const& Module::getCommands() const {
            return this->commands;
        }
        
        std::string const& Module::getName() const {
            return this->moduleName;
        }
        
        std::set<std::string> const& Module::getActions() const {
            return this->actions;
        }
        
        bool Module::hasAction(std::string const& action) const {
            auto const& actionEntry = this->actions.find(action);
            return actionEntry != this->actions.end();
        }
        
        std::set<uint_fast64_t> const& Module::getCommandIndicesByAction(std::string const& action) const {
            auto actionsCommandSetPair = this->actionsToCommandIndexMap.find(action);
            if (actionsCommandSetPair != this->actionsToCommandIndexMap.end()) {
                return actionsCommandSetPair->second;
            }
            
            LOG_THROW(false, storm::exceptions::OutOfRangeException, "Action name '" << action << "' does not exist in module.");
        }
        
        void Module::createMappings() {
            // Clear the current mappings.
            this->actionsToCommandIndexMap.clear();
            this->booleanVariableToIndexMap.clear();
            this->integerVariableToIndexMap.clear();
            
            // Create the mappings for the variables.
            for (uint_fast64_t i = 0; i < this->booleanVariables.size(); ++i) {
                this->booleanVariableToIndexMap[this->getBooleanVariables()[i].getName()] = i;
            }
            for (uint_fast64_t i = 0; i < this->integerVariables.size(); ++i) {
                this->integerVariableToIndexMap[this->getIntegerVariables()[i].getName()] = i;
            }
            
            // Add the mapping for all commands.
            for (uint_fast64_t i = 0; i < this->commands.size(); i++) {
                std::string const& action = this->commands[i].getActionName();
                if (action != "") {
                    if (this->actionsToCommandIndexMap.find(action) == this->actionsToCommandIndexMap.end()) {
                        this->actionsToCommandIndexMap.emplace(action, std::set<uint_fast64_t>());
                    }
                    this->actionsToCommandIndexMap[action].insert(i);
                    this->actions.insert(action);
                }
            }
            
            // For all actions that are "in the module", but for which no command exists, we add the mapping to an empty
            // set of commands.
            for (auto const& action : this->actions) {
                if (this->actionsToCommandIndexMap.find(action) == this->actionsToCommandIndexMap.end()) {
                    this->actionsToCommandIndexMap[action] = std::set<uint_fast64_t>();
                }
            }
        }
        
        Module Module::restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet) const {
            // First construct the new vector of commands.
            std::vector<storm::prism::Command> newCommands;
            for (auto const& command : commands) {
                if (indexSet.find(command.getGlobalIndex()) != indexSet.end()) {
                    newCommands.push_back(command);
                }
            }
            
            return Module(this->getName(), this->getBooleanVariables(), this->getIntegerVariables(), newCommands);
        }
        
        std::ostream& operator<<(std::ostream& stream, Module const& module) {
            stream << "module " << module.getName() << std::endl;
            for (auto const& booleanVariable : module.getBooleanVariables()) {
                stream << "\t" << booleanVariable << std::endl;
            }
            for (auto const& integerVariable : module.getIntegerVariables()) {
                stream << "\t" << integerVariable << std::endl;
            }
            for (auto const& command : module.getCommands()) {
                stream << "\t" << command << std::endl;
            }
            stream << "endmodule" << std::endl;
            return stream;
        }

        
    } // namespace ir
} // namespace storm
