#include "storm/storage/jani/LValue.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/visitor/JaniSyntacticalEqualityCheckVisitor.h"

namespace storm {
    namespace jani {
        
        LValue::LValue(storm::jani::Variable const& variable) : variable(&variable) {
            // Intentionally left empty
        }
        
        LValue::LValue(LValue const& array, std::vector<storm::expressions::Expression> const& index, std::vector<size_t> const& sizes) : arrayIndexVector(index), sizes(sizes) {
            STORM_LOG_THROW(array.isVariable(), storm::exceptions::NotSupportedException, "Expecting a variable as base of array");
            STORM_LOG_ASSERT(arrayIndexVector->size() <= sizes.size(), "Expecting arrayIndexVector and sizes vector to have the same size");
            variable = &array.getVariable();
            arrayIndex = arrayIndexVector->at(0);
            storm::expressions::ExpressionManager const& manager = arrayIndex.get().getManager();
            for (auto i = 1; i < arrayIndexVector->size(); ++i) {
                arrayIndex = arrayIndex.get() * manager.integer(sizes.at(i)) + arrayIndexVector->at(i);
            }
        }

        LValue::LValue(LValue const& array, storm::expressions::Expression const& index, size_t size) : arrayIndex(index) {
            STORM_LOG_THROW(array.isVariable(), storm::exceptions::NotSupportedException, "Expecting a variable as base of array");
            variable = &array.getVariable();
            arrayIndexVector = {index};
            sizes = {size};
        }

        bool LValue::isVariable() const {
            return !isArrayAccess();
        }
        
        storm::jani::Variable const& LValue::getVariable() const {
            STORM_LOG_ASSERT(isVariable(), "Tried to get the variable of an LValue, that actually is not a variable.");
            return *variable;
        }
        
        bool LValue::isArrayAccess() const {
            return arrayIndex.is_initialized() && arrayIndexVector.is_initialized();
        }

        storm::jani::Variable const&  LValue::getArray() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get the array variable of an LValue that is not an array access.");
            STORM_LOG_ASSERT(variable->isArrayVariable(), "Tried to get the array variable of an array access LValue, but the variable is not of type array.");
            return *variable;
        }

        storm::expressions::Expression const& LValue::getArrayIndex() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get the array index of an LValue that is not an array access.");
            return arrayIndex.get();
        }

        const std::vector<size_t> & LValue::getSizes() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get sizes of an LValue that is not an array access.");
            return sizes;
        }

        const size_t & LValue::getSizeAt(int i) const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get size of arrayindex " << i << " of an LValue that is not an array access.");
            STORM_LOG_ASSERT(i < sizes.size(), "Tried to get size of arrayindex " << i << " but there are only" << sizes.size() << " entries");
            return sizes.at(i);
        }

        const size_t LValue::getTotalSize() const {
            size_t result = 1;
            for (auto& array : sizes) {
                result *= array;
            }
            return result;
        }

        std::vector<storm::expressions::Expression> const& LValue::getArrayIndexVector() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to get the array index of an LValue that is not an array access.");
            return arrayIndexVector.get();
        }

        bool LValue::arrayIndexContainsVariable() const {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to check for variables in the array index of an LValue that is not an array access.");
            for (auto & expr : arrayIndexVector.get()) {
                if (expr.containsVariables()) {
                    return true;
                }
            }
            return false;
        }
        
        void LValue::setArrayIndex(std::vector<storm::expressions::Expression> const& newIndex) {
            STORM_LOG_ASSERT(isArrayAccess(), "Tried to set the array index of an LValue that is not an array access.");
            arrayIndexVector = newIndex;
            arrayIndex = arrayIndexVector->at(0);
            storm::expressions::ExpressionManager const& manager = arrayIndex.get().getManager();
            for (auto i = 1; i < sizes.size(); ++i) {
                arrayIndex = arrayIndex.get() * manager.integer(sizes.at(i)) + arrayIndexVector->at(i);
            }
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
                    return LValue(LValue(it->second), arrayIndexVector.get(), sizes);
                }
            }
        }

        std::string LValue::getName() const {
            std::string result;
            if (isVariable()) {
                result = getVariable().getName();
            } else {
                STORM_LOG_ASSERT(isArrayAccess(), "Unhandled LValue.");
                result += getArray().getName();
                for (auto i = 0; i < getArrayIndexVector().size(); ++i) {
                    result += "[" + getArrayIndexVector().at(i).toString() + "]";
                }
            }
            return result;
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
                    if (arrayIndexVector->size() != other.getArrayIndexVector().size()) {
                        return false;
                    } else {
                        bool less = false;
                        int i = 0;
                        // TODO: check if this is correct
                        while (!less && i < arrayIndexVector->size()) {
                            less = std::less<storm::expressions::Expression>()(arrayIndexVector.get().at(i), other.getArrayIndexVector().at(i));
                            if (!less && std::less<storm::expressions::Expression>()(other.getArrayIndexVector().at(i), arrayIndexVector.get().at(i))) {
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
                bool equal = other.isArrayAccess() && getArray().getExpressionVariable() == other.getArray().getExpressionVariable()
                        && arrayIndexVector->size() == other.getArrayIndexVector().size();
                int i = 0;
                storm::expressions::JaniSyntacticalEqualityCheckVisitor checker;
                while (equal && i < arrayIndexVector->size()) {
                    equal &= (getSizeAt(i) == other.getSizeAt(i) && checker.isSyntacticallyEqual(getArrayIndexVector().at(i), other.getArrayIndexVector().at(i)));
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
                for (auto i = 0; i < lValue.getArrayIndexVector().size(); ++i) {
                    stream << "[" << lValue.getArrayIndexVector().at(i) << "]";
                }
            }
            return stream;
        }

        
    }
}