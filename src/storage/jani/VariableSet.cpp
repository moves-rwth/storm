#include "src/storage/jani/VariableSet.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        namespace detail {
            
            VariableSetIterator::VariableSetIterator(VariableSet& variableSet, boost::variant<bool_iter, bint_iter, int_iter> initialIterator) : variableSet(variableSet), it(initialIterator) {
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
            
            Variable& VariableSetIterator::operator*() {
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
            
            ConstVariableSetIterator::ConstVariableSetIterator(VariableSet const& variableSet, boost::variant<bool_iter, bint_iter, int_iter> initialIterator) : variableSet(variableSet), it(initialIterator) {
                // Intentionally left empty.
            }
            
            ConstVariableSetIterator& ConstVariableSetIterator::operator++() {
                incrementIterator();
                return *this;
            }
            
            ConstVariableSetIterator& ConstVariableSetIterator::operator++(int) {
                incrementIterator();
                return *this;
            }
            
            Variable const& ConstVariableSetIterator::operator*() {
                if (it.which() == 0) {
                    return *boost::get<bool_iter>(it);
                } else if (it.which() == 1) {
                    return *boost::get<bint_iter>(it);
                } else {
                    return *boost::get<int_iter>(it);
                }
            }
            
            bool ConstVariableSetIterator::operator==(ConstVariableSetIterator const& other) const {
                return this->it == other.it;
            }
            
            bool ConstVariableSetIterator::operator!=(ConstVariableSetIterator const& other) const {
                return this->it != other.it;
            }
            
            void ConstVariableSetIterator::incrementIterator() {
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
            
            IntegerVariables::IntegerVariables(VariableSet& variableSet) : variableSet(variableSet) {
                // Intentionally left empty.
            }
            
            VariableSetIterator IntegerVariables::begin() {
                return VariableSetIterator(variableSet, variableSet.getBoundedIntegerVariables().begin());
            }
            
            VariableSetIterator IntegerVariables::end() {
                return VariableSetIterator(variableSet, variableSet.getUnboundedIntegerVariables().end());
            }
            
            ConstIntegerVariables::ConstIntegerVariables(VariableSet const& variableSet) : variableSet(variableSet) {
                // Intentionally left empty.
            }
            
            ConstVariableSetIterator ConstIntegerVariables::begin() const {
                return ConstVariableSetIterator(variableSet, variableSet.getBoundedIntegerVariables().begin());
            }
            
            ConstVariableSetIterator ConstIntegerVariables::end() const {
                return ConstVariableSetIterator(variableSet, variableSet.getUnboundedIntegerVariables().end());
            }
        }
        
        VariableSet::VariableSet() {
            // Intentionally left empty.
        }
        
        std::vector<BooleanVariable>& VariableSet::getBooleanVariables() {
            return booleanVariables;
        }
        
        std::vector<BooleanVariable> const& VariableSet::getBooleanVariables() const {
            return booleanVariables;
        }

        std::vector<BoundedIntegerVariable>& VariableSet::getBoundedIntegerVariables() {
            return boundedIntegerVariables;
        }

        std::vector<BoundedIntegerVariable> const& VariableSet::getBoundedIntegerVariables() const {
            return boundedIntegerVariables;
        }

        std::vector<UnboundedIntegerVariable>& VariableSet::getUnboundedIntegerVariables() {
            return unboundedIntegerVariables;
        }

        std::vector<UnboundedIntegerVariable> const& VariableSet::getUnboundedIntegerVariables() const {
            return unboundedIntegerVariables;
        }

        VariableSet::IntegerVariables VariableSet::getIntegerVariables() {
            return IntegerVariables(*this);
        }

        VariableSet::ConstIntegerVariables VariableSet::getIntegerVariables() const {
            return ConstIntegerVariables(*this);
        }
        
        void VariableSet::addBooleanVariable(BooleanVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            booleanVariables.push_back(variable);
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), std::make_pair(0, booleanVariables.size() - 1));
        }
        
        void VariableSet::addBoundedIntegerVariable(BoundedIntegerVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            boundedIntegerVariables.push_back(variable);
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), std::make_pair(1, boundedIntegerVariables.size() - 1));
        }
        
        void VariableSet::addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            unboundedIntegerVariables.push_back(variable);
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), std::make_pair(2, boundedIntegerVariables.size() - 1));
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
            return iterator(*this, booleanVariables.begin());
        }

        VariableSet::const_iterator VariableSet::begin() const {
            return const_iterator(*this, booleanVariables.begin());
        }
        
        VariableSet::iterator VariableSet::end() {
            return iterator(*this, unboundedIntegerVariables.end());
        }

        VariableSet::const_iterator VariableSet::end() const {
            return const_iterator(*this, unboundedIntegerVariables.end());
        }

        Variable const& VariableSet::getVariable(storm::expressions::Variable const& variable) const {
            auto it = variableToVariable.find(variable);
            STORM_LOG_THROW(it != variableToVariable.end(), storm::exceptions::InvalidArgumentException, "Unable to retrieve unknown variable '" << variable.getName() << "'.");
            
            if (it->second.first == 0) {
                return booleanVariables[it->second.second];
            } else if (it->second.first == 1) {
                return boundedIntegerVariables[it->second.second];
            } else {
                return unboundedIntegerVariables[it->second.second];
            }
        }
        
        bool VariableSet::hasVariable(storm::expressions::Variable const& variable) const {
            return variableToVariable.find(variable) != variableToVariable.end();
        }
    }
}