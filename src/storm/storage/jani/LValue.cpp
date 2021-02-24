#include "storm/storage/jani/LValue.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace jani {
        
        LValue::LValue(storm::jani::Variable const& variable) : variable(&variable) {
            // Intentionally left empty
        }
        
        LValue::LValue(LValue const& array, std::vector<storm::expressions::Expression> const& index) : arrayIndex(index) {
            STORM_LOG_THROW(array.isVariable(), storm::exceptions::NotSupportedException, "Expecting a variable as base of array");
            variable = &array.getVariable();
        }

        bool LValue::isVariable() const {
            return !arrayIndex;
        }
        
        storm::jani::Variable const& LValue::getVariable() const {
            STORM_LOG_ASSERT(isVariable(), "Tried to get the variable of an LValue, that actually is not a variable.");
            return *variable;
        }
        
        bool LValue::isArrayAccess() const {
            return arrayIndex.is_initialized();
        }

        storm::jani::Variable const&  LValue::getArray() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get the array variable of an LValue that is not an array access.");
            STORM_LOG_ASSERT(variable->isArrayVariable(), "Tried to get the array variable of an array access LValue, but the variable is not of type array.");
            return *variable;
        }

        std::vector<storm::expressions::Expression> const& LValue::getArrayIndex() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get the array index of an LValue that is not an array access.");
            return arrayIndex.get();
        }

        bool LValue::arrayIndexContainsVariable() const {
            for (auto & expr : arrayIndex.get()) {
                if (expr.containsVariables()) {
                    return true;
                }
            }
            return false;
        }
        
        void LValue::setArrayIndex(std::vector<storm::expressions::Expression> const& newIndex) {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to set the array index of an LValue that is not an array access.");
            arrayIndex = newIndex;
        }
        
        bool LValue::isTransient() const {
            return variable->isTransient();
        }
        
        LValue LValue::changeAssignmentVariables(std::map<Variable const*, std::reference_wrapper<Variable const>> const& remapping) const {
            if (isVariable()) {
                auto it = remapping.find(variable);
                if (it == remapping.end()) {
                    return *this;
                } else {
                    return LValue(it->second);
                }
            } else {
                STORM_LOG_ASSERT(isArrayAccess(), "Unhandled LValue.");
                auto it = remapping.find(variable);
                if (it == remapping.end()) {
                    return *this;
                } else {
                    return LValue(LValue(it->second), arrayIndex.get());
                }
            }
        }

        bool LValue::operator<(LValue const& other) const {
            if (isVariable()) {
                return !other.isVariable() || variable->getExpressionVariable() < other.getVariable().getExpressionVariable();
            } else {
            STORM_LOG_ASSERT(isArrayAccess(), "Unhandled LValue.");
                if (other.isVariable()) {
                    return false;
                }
                STORM_LOG_ASSERT(other.isArrayAccess(), "Unhandled LValue.");
                if (getArray().getExpressionVariable() < other.getArray().getExpressionVariable()) {
                    return true;
                } else if (other.getArray().getExpressionVariable() < getArray().getExpressionVariable()) {
                    return false;
                } else {
                    // TODO: is this correct?
                    if (arrayIndex->size() != other.getArrayIndex().size()) {
                        return false;
                    } else {
                        bool less = false;
                        int i = 0;
                        while (!less && i < arrayIndex->size()) {
                            less = std::less<storm::expressions::Expression>()(arrayIndex.get().at(i), other.getArrayIndex().at(i));
                            if (!less && std::less<storm::expressions::Expression>()(other.getArrayIndex().at(i), arrayIndex.get().at(i))) {
                                break;
                            }
                            ++i;
                        }
                        return less;
                    }
                }
            }
        }
        
        bool LValue::operator==(LValue const& other) const {
            if (isVariable()) {
                return other.isVariable() && getVariable().getExpressionVariable() == other.getVariable().getExpressionVariable();
            } else {
                STORM_LOG_ASSERT(isArrayAccess(), "Unhandled LValue.");
                bool equal = other.isArrayAccess() && getArray().getExpressionVariable() == other.getArray().getExpressionVariable() && arrayIndex->size() == other.getArrayIndex().size();
                int i = 0;
                while (equal && i < arrayIndex->size()) {
                    equal &= arrayIndex->at(i).isSyntacticallyEqual(other.getArrayIndex().at(i));
                    ++i;
                }
                return equal;
            }
        }
        
        std::ostream& operator<<(std::ostream& stream, LValue const& lValue) {
            if (lValue.isVariable()) {
                stream << lValue.getVariable().getName();
            } else {
                STORM_LOG_ASSERT(lValue.isArrayAccess(), "Unhandled LValue.");
                stream << lValue.getArray().getName();
                for (auto i = 0; i < lValue.getArrayIndex().size(); ++i) {
                    stream << "[" << lValue.getArrayIndex().at(i) << "]";
                }
            }
            return stream;
        }

        
    }
}