#include "storm/storage/jani/LValue.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/visitor/JaniSyntacticalEqualityCheckVisitor.h"

namespace storm {
    namespace jani {
        
        LValue::LValue(storm::jani::Variable const& variable) : variable(&variable) {
            STORM_LOG_ASSERT(!variable.isArrayVariable(), "LValue for variable " << variable.getName() << " not possible as the variable is a array variable, we need an index and sizes for this");
            // Intentionally left empty
        }

        LValue::LValue(storm::jani::Variable const& variable, std::vector<storm::expressions::Expression> const index, std::vector<size_t> const& sizes) : variable(&variable), arrayIndexVector(index), sizes(sizes) {
            STORM_LOG_THROW(variable.isArrayVariable(), storm::exceptions::NotSupportedException, "Expecting an array Variable");
            STORM_LOG_ASSERT(arrayIndexVector->size() <= sizes.size(), "Expecting arrayIndexVector size to be smaller or equal than the size of the sizes vector for variable: " << variable.getName());
            // Intentionally left empty
        }

        LValue::LValue(storm::jani::Variable const& variable, std::vector<size_t> const& sizes) : variable(&variable), sizes(sizes) {
            STORM_LOG_THROW(variable.isArrayVariable(), storm::exceptions::NotSupportedException, "Expecting an array Variable");
            arrayIndexVector = std::vector<storm::expressions::Expression>();
        }

        LValue::LValue(storm::jani::Variable const& variable, storm::expressions::Expression const& index, size_t size) : variable(&variable) {
            STORM_LOG_THROW(variable.isArrayVariable(), storm::exceptions::NotSupportedException, "Expecting an array Variable");
            arrayIndexVector = {index};
            sizes = {size};
        }

        bool LValue::isVariable() const {
            return !isArray();
        }

        bool LValue::isArray() const {
            return variable->isArrayVariable();
        }
        
        storm::jani::Variable const& LValue::getVariable() const {
            return *variable;
        }
        
        bool LValue::isArrayAccess() const {
            return arrayIndexVector.is_initialized();
        }

        bool LValue::isFullArrayAccess() const {
            return isArrayAccess() && arrayIndexVector->size() == sizes.size();
        }

        storm::expressions::Expression LValue::getArrayIndex() const {
            STORM_LOG_ASSERT(isFullArrayAccess(), "Tried to get the array index of an LValue that is not a full array access.");
            auto res = arrayIndexVector->at(0);
            for (auto i = 1; i < sizes.size(); ++i) {
                res = res * res.getManager().integer(sizes.at(i)) + arrayIndexVector->at(i);
            }
            return res;
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
            STORM_LOG_ASSERT(isArray(), "Tried to set the array index of an LValue that is not an array access.");
            STORM_LOG_ASSERT(newIndex.size() <= sizes.size(), "Expecting arrayIndexVector size to be smaller or equal than the size of the sizes vector for variable: " << variable->getName());
            arrayIndexVector = newIndex;
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
                STORM_LOG_ASSERT(isArray(), "Unhandled LValue.");
                auto it = remapping.find(variable);
                if (it == remapping.end()) {
                    return *this;
                } else {
                    return LValue(it->second, arrayIndexVector.get(), sizes);
                }
            }
        }

        std::string LValue::getName() const {
            std::string result = getVariable().getName();;
            if (isArrayAccess()) {
                for (auto i = 0; i < getArrayIndexVector().size(); ++i) {
                    result += "[" + getArrayIndexVector().at(i).toString() + "]";
                }
            }
            return result;
        }

        bool LValue::operator<(LValue const& other) const {
            // TODO: is this correct in this way?
            if (isVariable()) {
                return !other.isVariable() || variable->getExpressionVariable() < other.getVariable().getExpressionVariable();
            } else {
            STORM_LOG_ASSERT(isArray(), "Unhandled LValue.");
                if (other.isVariable()) {
                    return false;
                }
                STORM_LOG_ASSERT(other.isArray(), "Unhandled LValue.");
                if (getVariable().getExpressionVariable() < other.getVariable().getExpressionVariable()) {
                    return true;
                } else if (other.getVariable().getExpressionVariable() < getVariable().getExpressionVariable()) {
                    return false;
                } else {
                    if (arrayIndexVector->size() != other.getArrayIndexVector().size()) {
                        return false;
                    } else {
                        bool less = false;
                        int i = 0;
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
                STORM_LOG_ASSERT(isArray(), "Unhandled LValue.");
                bool equal = other.isArray() && getVariable().getExpressionVariable() == other.getVariable().getExpressionVariable();
                if (isArrayAccess() && other.isArrayAccess()) {
                    // Either for both there are array access indices available
                    equal &= getArrayIndexVector().size() == other.getArrayIndexVector().size();
                    int i = 0;
                    storm::expressions::JaniSyntacticalEqualityCheckVisitor checker;
                    while (equal && i < arrayIndexVector->size()) {
                        equal &= (getSizeAt(i) == other.getSizeAt(i) && checker.isSyntacticallyEqual(getArrayIndexVector().at(i), other.getArrayIndexVector().at(i)));
                        ++i;
                    }
                } else {
                    // Or they both don't have any index
                    equal &= !isArrayAccess() && !other.isArrayAccess();
                }
                return equal;
            }
        }
        
        std::ostream& operator<<(std::ostream& stream, LValue const& lValue) {
            stream << lValue.getVariable().getName();

            if (lValue.isArray()) {
                if (lValue.isArrayAccess()) {
                    for (auto i = 0; i < lValue.getArrayIndexVector().size(); ++i) {
                        stream << "[" << lValue.getArrayIndexVector().at(i) << "]";
                    }
                }
            }
            return stream;
        }

        
    }
}