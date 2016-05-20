#include "src/storage/jani/VariableSet.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        VariableSet::VariableSet() {
            // Intentionally left empty.
        }
        
        std::vector<BooleanVariable> const& VariableSet::getBooleanVariables() const {
            return booleanVariables;
        }
        
        std::vector<BoundedIntegerVariable> const& VariableSet::getBoundedIntegerVariables() const {
            return boundedIntegerVariables;
        }
        
        std::vector<UnboundedIntegerVariable> const& VariableSet::getUnboundedIntegerVariables() const {
            return unboundedIntegerVariables;
        }
        
        void VariableSet::addBooleanVariable(BooleanVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            booleanVariables.push_back(variable);
            variables.emplace(variable.getName(), booleanVariables.back());
        }
        
        void VariableSet::addBoundedIntegerVariable(BoundedIntegerVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            boundedIntegerVariables.push_back(variable);
            variables.emplace(variable.getName(), boundedIntegerVariables.back());
        }
        
        void VariableSet::addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            unboundedIntegerVariables.push_back(variable);
            variables.emplace(variable.getName(), unboundedIntegerVariables.back());
        }
        
        bool VariableSet::hasVariable(std::string const& name) const {
            return variables.find(name) != variables.end();
        }
        
        Variable const& VariableSet::getVariable(std::string const& name) const {
            auto it = variables.find(name);
            STORM_LOG_THROW(it != variables.end(), storm::exceptions::InvalidArgumentException, "Unable to retrieve unknown variable '" << name << "'.");
            return it->second.get();
        }
        
    }
}