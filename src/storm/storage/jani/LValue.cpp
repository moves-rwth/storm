#include "storm/storage/jani/LValue.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace jani {
        
        LValue::LValue(storm::jani::Variable const& variable) : variable(&variable) {
            // Intentionally left empty
        }
        
        LValue::LValue(LValue const& array, storm::expressions::Expression const& index) : arrayIndex(index) {
            STORM_LOG_THROW(array.isVariable(), storm::exceptions::NotSupportedException, "Nested arrays as LValues are currently not implemented.");
            variable = &array.getVariable();
        }
        
        bool LValue::isVariable() const {
            return !arrayIndex.isInitialized();
        }
        
        storm::jani::Variable const& LValue::getVariable() const {
            STORM_LOG_ASSERT(isVariable(), "Tried to get the variable of an LValue, that actually is not a variable.");
            return *variable;
        }
        
        bool LValue::isArrayAccess() const {
            return arrayIndex.isInitialized();
        }
        
        storm::jani::ArrayVariable const&  LValue::getArray() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get the array variable of an LValue that is not an array access.");
            STORM_LOG_ASSERT(variable->isArrayVariable(), "Tried to get the array variable of an array access LValue, but the variable is not of type array.");
            return variable->asArrayVariable();
        }
        
        storm::expressions::Expression const& LValue::getArrayIndex() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get the array index of an LValue that is not an array access.");
            return arrayIndex;
        }
        
        void LValue::setArrayIndex(storm::expressions::Expression const& newIndex) {
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
                    return LValue(LValue(it->second), arrayIndex);
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
                    return std::less<storm::expressions::Expression>()(arrayIndex, other.getArrayIndex());
                }
            }
        }
        
        bool LValue::operator==(LValue const& other) const {
            if (isVariable()) {
                return other.isVariable() && getVariable().getExpressionVariable() == other.getVariable().getExpressionVariable();
            } else {
                STORM_LOG_ASSERT(isArrayAccess(), "Unhandled LValue.");
                return other.isArrayAccess() && getArray().getExpressionVariable() == other.getArray().getExpressionVariable() && getArrayIndex().isSyntacticallyEqual(other.getArrayIndex());
            }
        }
        
        std::ostream& operator<<(std::ostream& stream, LValue const& lValue) {
            if (lValue.isVariable()) {
                stream << lValue.getVariable().getName();
            } else {
                STORM_LOG_ASSERT(lValue.isArrayAccess(), "Unhandled LValue.");
                stream << lValue.getArray().getName() << "[" << lValue.getArrayIndex() << "]";
            }
            return stream;
        }

        
    }
}