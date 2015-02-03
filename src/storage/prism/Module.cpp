#include "src/storage/prism/Module.h"
#include "src/utility/macros.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidAccessException.h"

namespace storm {
    namespace prism {
        Module::Module(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, std::string const& filename, uint_fast64_t lineNumber) : Module(moduleName, booleanVariables, integerVariables, commands, "", std::map<std::string, std::string>(), filename, lineNumber) {
            // Intentionally left empty.
        }
        
        Module::Module(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, std::string const& renamedFromModule, std::map<std::string, std::string> const& renaming, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), moduleName(moduleName), booleanVariables(booleanVariables), booleanVariableToIndexMap(), integerVariables(integerVariables), integerVariableToIndexMap(), commands(commands), actionIndices(), actionIndicesToCommandIndexMap(), renamedFromModule(renamedFromModule), renaming(renaming) {
            // Initialize the internal mappings for fast information retrieval.
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
            STORM_LOG_THROW(nameIndexPair != this->booleanVariableToIndexMap.end(), storm::exceptions::InvalidArgumentException, "Unknown boolean variable '" << variableName << "'.");
            return this->getBooleanVariables()[nameIndexPair->second];
        }
        
        std::vector<storm::prism::BooleanVariable> const& Module::getBooleanVariables() const {
            return this->booleanVariables;
        }
        
        storm::prism::IntegerVariable const& Module::getIntegerVariable(std::string const& variableName) const {
            auto const& nameIndexPair = this->integerVariableToIndexMap.find(variableName);
            STORM_LOG_THROW(nameIndexPair != this->integerVariableToIndexMap.end(), storm::exceptions::InvalidArgumentException, "Unknown integer variable '" << variableName << "'.");
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
        
        std::set<uint_fast64_t> const& Module::getActionIndices() const {
            return this->actionIndices;
        }
        
        bool Module::hasActionIndex(uint_fast64_t actionIndex) const {
            return this->actionIndices.find(actionIndex) != this->actionIndices.end();
        }
        
        bool Module::isRenamedFromModule() const {
            return this->renamedFromModule != "";
        }
        
        std::string const& Module::getBaseModule() const {
            STORM_LOG_THROW(this->isRenamedFromModule(), storm::exceptions::InvalidAccessException, "Unable to retrieve base module of module that was not created by renaming.");
            return this->renamedFromModule;
        }
        
        std::map<std::string, std::string> const& Module::getRenaming() const {
            STORM_LOG_THROW(this->isRenamedFromModule(), storm::exceptions::InvalidAccessException, "Unable to retrieve renaming of module that was not created by renaming.");
            return this->renaming;
        }
        
        std::set<uint_fast64_t> const& Module::getCommandIndicesByActionIndex(uint_fast64_t actionIndex) const {
            auto actionIndicesCommandSetPair = this->actionIndicesToCommandIndexMap.find(actionIndex);
            if (actionIndicesCommandSetPair != this->actionIndicesToCommandIndexMap.end()) {
                return actionIndicesCommandSetPair->second;
            }
            
            STORM_LOG_THROW(false, storm::exceptions::OutOfRangeException, "Action index '" << actionIndex << "' does not exist in module.");
        }
        
        void Module::createMappings() {
            // Clear the current mappings.
            this->actionIndicesToCommandIndexMap.clear();
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
                if (this->commands[i].isLabeled()) {
                    uint_fast64_t actionIndex = this->commands[i].getActionIndex();
                    if (this->actionIndicesToCommandIndexMap.find(actionIndex) == this->actionIndicesToCommandIndexMap.end()) {
                        this->actionIndicesToCommandIndexMap.emplace(actionIndex, std::set<uint_fast64_t>());
                    }
                    this->actionIndicesToCommandIndexMap[actionIndex].insert(i);
                    this->actionIndices.insert(actionIndex);
                }
            }
            
            // For all actions that are "in the module", but for which no command exists, we add the mapping to an empty
            // set of commands.
            for (auto const& actionIndex : this->actionIndices) {
                if (this->actionIndicesToCommandIndexMap.find(actionIndex) == this->actionIndicesToCommandIndexMap.end()) {
                    this->actionIndicesToCommandIndexMap[actionIndex] = std::set<uint_fast64_t>();
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
        
        Module Module::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            std::vector<BooleanVariable> newBooleanVariables;
            newBooleanVariables.reserve(this->getNumberOfBooleanVariables());
            for (auto const& booleanVariable : this->getBooleanVariables()) {
                newBooleanVariables.emplace_back(booleanVariable.substitute(substitution));
            }
            
            std::vector<IntegerVariable> newIntegerVariables;
            newBooleanVariables.reserve(this->getNumberOfIntegerVariables());
            for (auto const& integerVariable : this->getIntegerVariables()) {
                newIntegerVariables.emplace_back(integerVariable.substitute(substitution));
            }
            
            std::vector<Command> newCommands;
            newCommands.reserve(this->getNumberOfCommands());
            for (auto const& command : this->getCommands()) {
                Command newCommand = command.substitute(substitution);
                if (!newCommand.getGuardExpression().isFalse()) {
                    newCommands.emplace_back(newCommand);
                }
            }
            
            return Module(this->getName(), newBooleanVariables, newIntegerVariables, newCommands, this->getFilename(), this->getLineNumber());
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
        
    } // namespace prism
} // namespace storm
