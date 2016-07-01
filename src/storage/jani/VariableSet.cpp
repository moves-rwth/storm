#include "src/storage/jani/VariableSet.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        namespace detail {
            
            template<typename VariableType>
            VariableType& Dereferencer<VariableType>::operator()(std::shared_ptr<VariableType> const& d) const {
                return *d;
            }
            
            template<typename VariableType>
            Variables<VariableType>::Variables(input_iterator it, input_iterator ite) : it(it), ite(ite) {
                // Intentionally left empty.
            }
                
            template<typename VariableType>
            typename Variables<VariableType>::iterator Variables<VariableType>::begin() {
                return boost::make_transform_iterator(it, Dereferencer<VariableType>());
            }
            
            template<typename VariableType>
            typename Variables<VariableType>::iterator Variables<VariableType>::end() {
                return boost::make_transform_iterator(ite, Dereferencer<VariableType>());
            }

            template<typename VariableType>
            ConstVariables<VariableType>::ConstVariables(const_input_iterator it, const_input_iterator ite) : it(it), ite(ite) {
                // Intentionally left empty.
            }
            
            template<typename VariableType>
            typename ConstVariables<VariableType>::const_iterator ConstVariables<VariableType>::begin() {
                return boost::make_transform_iterator(it, Dereferencer<VariableType const>());
            }
            
            template<typename VariableType>
            typename ConstVariables<VariableType>::const_iterator ConstVariables<VariableType>::end() {
                return boost::make_transform_iterator(ite, Dereferencer<VariableType const>());
            }

        }
        
        VariableSet::VariableSet() {
            // Intentionally left empty.
        }
        
        detail::Variables<BooleanVariable> VariableSet::getBooleanVariables() {
            return detail::Variables<BooleanVariable>(booleanVariables.begin(), booleanVariables.end());
        }
        
        detail::ConstVariables<BooleanVariable> VariableSet::getBooleanVariables() const {
            return detail::ConstVariables<BooleanVariable>(booleanVariables.begin(), booleanVariables.end());
        }

        detail::Variables<BoundedIntegerVariable> VariableSet::getBoundedIntegerVariables() {
            return detail::Variables<BoundedIntegerVariable>(boundedIntegerVariables.begin(), boundedIntegerVariables.end());
        }

        detail::ConstVariables<BoundedIntegerVariable> VariableSet::getBoundedIntegerVariables() const {
            return detail::ConstVariables<BoundedIntegerVariable>(boundedIntegerVariables.begin(), boundedIntegerVariables.end());
        }

        detail::Variables<UnboundedIntegerVariable> VariableSet::getUnboundedIntegerVariables() {
            return detail::Variables<UnboundedIntegerVariable>(unboundedIntegerVariables.begin(), unboundedIntegerVariables.end());
        }

        detail::ConstVariables<UnboundedIntegerVariable> VariableSet::getUnboundedIntegerVariables() const {
            return detail::ConstVariables<UnboundedIntegerVariable>(unboundedIntegerVariables.begin(), unboundedIntegerVariables.end());
        }

        void VariableSet::addBooleanVariable(BooleanVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<BooleanVariable> newVariable = std::make_shared<BooleanVariable>(variable);
            variables.push_back(newVariable);
            booleanVariables.push_back(newVariable);
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
        }
        
        void VariableSet::addBoundedIntegerVariable(BoundedIntegerVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<BoundedIntegerVariable> newVariable = std::make_shared<BoundedIntegerVariable>(variable);
            variables.push_back(newVariable);
            boundedIntegerVariables.push_back(newVariable);
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
        }
        
        void VariableSet::addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<UnboundedIntegerVariable> newVariable = std::make_shared<UnboundedIntegerVariable>(variable);
            variables.push_back(newVariable);
            unboundedIntegerVariables.push_back(newVariable);
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
        }
        
        bool VariableSet::hasVariable(std::string const& name) const {
            return nameToVariable.find(name) != nameToVariable.end();
        }
        
        Variable const& VariableSet::getVariable(std::string const& name) const {
            auto it = nameToVariable.find(name);
            STORM_LOG_THROW(it != nameToVariable.end(), storm::exceptions::InvalidArgumentException, "Unable to retrieve unknown variable '" << name << "'.");
            return getVariable(it->second);
        }

        VariableSet::iterator VariableSet::begin() {
            return boost::make_transform_iterator(variables.begin(), detail::Dereferencer<Variable>());
        }

        VariableSet::const_iterator VariableSet::begin() const {
            return boost::make_transform_iterator(variables.begin(), detail::Dereferencer<Variable const>());
        }
        
        VariableSet::iterator VariableSet::end() {
            return boost::make_transform_iterator(variables.end(), detail::Dereferencer<Variable>());
        }

        VariableSet::const_iterator VariableSet::end() const {
            return boost::make_transform_iterator(variables.end(), detail::Dereferencer<Variable const>());
        }

        Variable const& VariableSet::getVariable(storm::expressions::Variable const& variable) const {
            auto it = variableToVariable.find(variable);
            STORM_LOG_THROW(it != variableToVariable.end(), storm::exceptions::InvalidArgumentException, "Unable to retrieve unknown variable '" << variable.getName() << "'.");
            return *it->second;
        }
        
        bool VariableSet::hasVariable(storm::expressions::Variable const& variable) const {
            return variableToVariable.find(variable) != variableToVariable.end();
        }
        
        bool VariableSet::containsBooleanVariable() const {
            return !booleanVariables.empty();
        }
        
        bool VariableSet::containsBoundedIntegerVariable() const {
            return !boundedIntegerVariables.empty();
        }
        
        bool VariableSet::containsUnboundedIntegerVariables() const {
            return !unboundedIntegerVariables.empty();
        }
        
        bool VariableSet::empty() const {
            return !(containsBooleanVariable() || containsBoundedIntegerVariable() || containsUnboundedIntegerVariables());
        }
        
        template class detail::Dereferencer<Variable>;
        template class detail::Dereferencer<BooleanVariable>;
        template class detail::Dereferencer<BoundedIntegerVariable>;
        template class detail::Dereferencer<UnboundedIntegerVariable>;
        template class detail::Dereferencer<Variable const>;
        template class detail::Dereferencer<BooleanVariable const>;
        template class detail::Dereferencer<BoundedIntegerVariable const>;
        template class detail::Dereferencer<UnboundedIntegerVariable const>;
        template class detail::Variables<BooleanVariable>;
        template class detail::Variables<BoundedIntegerVariable>;
        template class detail::Variables<UnboundedIntegerVariable>;
        template class detail::ConstVariables<BooleanVariable>;
        template class detail::ConstVariables<BoundedIntegerVariable>;
        template class detail::ConstVariables<UnboundedIntegerVariable>;

    }
}
