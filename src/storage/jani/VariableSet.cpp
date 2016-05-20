#include "src/storage/jani/VariableSet.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        namespace detail {
            
            VariableSetIterator::VariableSetIterator(VariableSet const& variableSet, boost::variant<bool_iter, bint_iter, int_iter> initialIterator) : variableSet(variableSet), it(initialIterator) {
                // Intentionally left empty.
            }
            
            VariableSetIterator& VariableSetIterator::operator++() {
                incrementIterator();
                return *this;
            }
            
            VariableSetIterator& VariableSetIterator::operator++(int) {
                incrementIterator();
                return *this;
            }
            
            Variable const& VariableSetIterator::operator*() {
                if (it.which() == 0) {
                    return *boost::get<bool_iter>(it);
                } else if (it.which() == 1) {
                    return *boost::get<bint_iter>(it);
                } else {
                    return *boost::get<int_iter>(it);
                }
            }
            
            bool VariableSetIterator::operator==(VariableSetIterator const& other) const {
                return this->it == other.it;
            }
            
            bool VariableSetIterator::operator!=(VariableSetIterator const& other) const {
                return this->it != other.it;
            }
            
            void VariableSetIterator::incrementIterator() {
                if (it.which() == 0) {
                    bool_iter& tmp = boost::get<bool_iter>(it);
                    if (tmp != variableSet.getBooleanVariables().end()) {
                        ++tmp;
                    } else {
                        it = variableSet.getBoundedIntegerVariables().begin();
                    }
                } else if (it.which() == 1) {
                    bint_iter& tmp = boost::get<bint_iter>(it);
                    if (tmp != variableSet.getBoundedIntegerVariables().end()) {
                        ++tmp;
                    } else {
                        it = variableSet.getUnboundedIntegerVariables().begin();
                    }
                } else {
                    ++boost::get<int_iter>(it);
                }
            }
            
            IntegerVariables::IntegerVariables(VariableSet const& variableSet) : variableSet(variableSet) {
                // Intentionally left empty.
            }
            
            VariableSetIterator IntegerVariables::begin() const {
                return VariableSetIterator(variableSet, variableSet.getBoundedIntegerVariables().begin());
            }
            
            VariableSetIterator IntegerVariables::end() const {
                return VariableSetIterator(variableSet, variableSet.getUnboundedIntegerVariables().end());
            }
        }
        
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
        
        detail::IntegerVariables VariableSet::getIntegerVariables() const {
            return detail::IntegerVariables(*this);
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
        
        detail::VariableSetIterator VariableSet::begin() const {
            return detail::VariableSetIterator(*this, booleanVariables.begin());
        }
        
        detail::VariableSetIterator VariableSet::end() const {
            return detail::VariableSetIterator(*this, unboundedIntegerVariables.end());
        }
        
    }
}