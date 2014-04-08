#include "src/storage/prism/Module.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace prism {
        Module::Module(std::string const& moduleName, std::map<std::string, storm::prism::BooleanVariable> const& booleanVariables, std::map<std::string, storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), moduleName(moduleName), booleanVariables(booleanVariables), integerVariables(integerVariables), commands(commands), actions(), actionsToCommandIndexMap() {
            // Initialize the internal mappings for fast information retrieval.
            this->collectActions();
        }
        
        Module::Module(Module const& oldModule, std::string const& newModuleName, std::map<std::string, std::string> const& renaming, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), moduleName(newModuleName), booleanVariables(), integerVariables(), commands(), actions(), actionsToCommandIndexMap() {
            // Iterate over boolean variables and rename them. If a variable was not renamed, this is an error and an exception is thrown.
            for (auto const& nameVariablePair : oldModule.getBooleanVariables()) {
                auto renamingPair = renaming.find(nameVariablePair.first);
                LOG_THROW(renamingPair == renaming.end(), storm::exceptions::InvalidArgumentException, "Boolean variable " << moduleName << "." << nameVariablePair.first << " was not renamed.");
                this->booleanVariables.emplace(nameVariablePair.first, BooleanVariable(nameVariablePair.second, renamingPair->second, renaming, filename, lineNumber));
            }
            
            // Now do the same for the integer variables.
            for (auto const& nameVariablePair : oldModule.getIntegerVariables()) {
                auto renamingPair = renaming.find(nameVariablePair.first);
                LOG_THROW(renamingPair == renaming.end(), storm::exceptions::InvalidArgumentException, "Integer variable " << moduleName << "." << nameVariablePair.first << " was not renamed.");
                this->integerVariables.emplace(nameVariablePair.first, IntegerVariable(nameVariablePair.second, renamingPair->second, renaming, filename, lineNumber));
            }
            
            // Now we are ready to clone all commands and rename them if requested.
            this->commands.reserve(oldModule.getNumberOfCommands());
            for (Command const& command : oldModule.getCommands()) {
                this->commands.emplace_back(command, command.getGlobalIndex(), renaming);
            }
            
            // Finally, update internal mapping.
            this->collectActions();
        }
        
        std::size_t Module::getNumberOfBooleanVariables() const {
            return this->booleanVariables.size();
        }
        
        std::size_t Module::getNumberOfIntegerVariables() const {
            return this->integerVariables.size();
        }
        
        storm::prism::BooleanVariable const& Module::getBooleanVariable(std::string const& variableName) const {
            auto const& nameVariablePair = this->getBooleanVariables().find(variableName);
            LOG_THROW(nameVariablePair == this->getBooleanVariables().end(), storm::exceptions::InvalidArgumentException, "Unknown boolean variable '" << variableName << "'.");
            return nameVariablePair->second;
        }
        
        std::map<std::string, storm::prism::BooleanVariable> const& Module::getBooleanVariables() const {
            return this->booleanVariables;
        }

        storm::prism::IntegerVariable const& Module::getIntegerVariable(std::string const& variableName) const {
            auto const& nameVariablePair = this->getIntegerVariables().find(variableName);
            LOG_THROW(nameVariablePair == this->getIntegerVariables().end(), storm::exceptions::InvalidArgumentException, "Unknown integer variable '" << variableName << "'.");
            return nameVariablePair->second;
        }
        
        std::map<std::string, storm::prism::IntegerVariable> const& Module::getIntegerVariables() const {
            return this->integerVariables;
        }
        
        std::size_t Module::getNumberOfCommands() const {
            return this->commands.size();
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
        
        void Module::collectActions() {
            // Clear the current mapping.
            this->actionsToCommandIndexMap.clear();
            
            // Add the mapping for all commands.
            for (unsigned int id = 0; id < this->commands.size(); id++) {
                std::string const& action = this->commands[id].getActionName();
                if (action != "") {
                    if (this->actionsToCommandIndexMap.find(action) == this->actionsToCommandIndexMap.end()) {
                        this->actionsToCommandIndexMap.emplace(action, std::set<uint_fast64_t>());
                    }
                    this->actionsToCommandIndexMap[action].insert(id);
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
            for (auto const& nameVariablePair : module.getBooleanVariables()) {
                stream << "\t" << nameVariablePair.second << std::endl;
            }
            for (auto const& nameVariablePair : module.getIntegerVariables()) {
                stream << "\t" << nameVariablePair.second << std::endl;
            }
            for (auto const& command : module.getCommands()) {
                stream << "\t" << command << std::endl;
            }
            stream << "endmodule" << std::endl;
            return stream;
        }

        
    } // namespace ir
} // namespace storm
