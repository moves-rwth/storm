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

        LValue::LValue(storm::jani::Variable const& variable, std::vector<storm::expressions::Expression> const& index) : variable(&variable), arrayIndexVector(index) {
            STORM_LOG_THROW(variable.getType().isArrayType(), storm::exceptions::NotSupportedException, "Expecting an array Variable");
            // Intentionally left empty
        }

        bool LValue::isVariable() const {
            return !isArray();
        }

        bool LValue::isArray() const {
            return variable->getType().isArrayType();
        }
        
        storm::jani::Variable const& LValue::getVariable() const {
            return *variable;
        }
        
        bool LValue::isArrayAccess() const {
            return !arrayIndexVector.empty();
        }

        bool LValue::isFullArrayAccess() const {
            return isArrayAccess() && variable->getType().asArrayType().getNestingDegree() == arrayIndexVector.size();
        }
        
        std::vector<storm::expressions::Expression>& LValue::getArrayIndexVector() {
            STORM_LOG_ASSERT(isArray(), "Tried to get the array index of an LValue that does not refer to an array.");
            return arrayIndexVector;
        }

        std::vector<storm::expressions::Expression> const& LValue::getArrayIndexVector() const {
            STORM_LOG_ASSERT(isArray(), "Tried to get the array index of an LValue that does not refer to an array.");
            return arrayIndexVector;
        }

        bool LValue::arrayIndexContainsVariable() const {
            STORM_LOG_ASSERT(isArray(), "Tried to check for variables in the array index of an LValue that does not refer to an array.");
            for (auto & expr : arrayIndexVector) {
                if (expr.containsVariables()) {
                    return true;
                }
            }
            return false;
        }
        
        void LValue::setArrayIndex(std::vector<storm::expressions::Expression> const& newIndex) {
            STORM_LOG_ASSERT(isArray(), "Tried to set the array index of an LValue thatdoes not refer to an array.");
            arrayIndexVector = newIndex;
        }
        
        void LValue::addArrayAccessIndex(storm::expressions::Expression const& index) {
            STORM_LOG_ASSERT(isArray(), "Tried to add an array access index to an LValue that doesn't consider an array.");
            arrayIndexVector.push_back(index);
        }
        
        bool LValue::isTransient() const {
            return variable->isTransient();
        }
        
        LValue LValue::changeAssignmentVariables(std::map<Variable const*, std::reference_wrapper<Variable const>> const& remapping) const {
            auto it = remapping.find(variable);
            if (it == remapping.end()) {
                return *this;
            } else {
                if (isArray()) {
                    return LValue(it->second, arrayIndexVector);
                } else {
                    return LValue(it->second);
                }
            }
        }

        std::string LValue::getName() const {
            std::string result = getVariable().getName();;
            if (isArrayAccess()) {
                for (uint_fast64_t i = 0; i < getArrayIndexVector().size(); ++i) {
                    result += "[" + getArrayIndexVector().at(i).toString() + "]";
                }
            }
            return result;
        }

        bool LValue::operator<(LValue const& other) const {
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
                    if (arrayIndexVector.size() < other.getArrayIndexVector().size()) {
                        return true;
                    } else if (arrayIndexVector.size() > other.getArrayIndexVector().size()) {
                        return false;
                    } else {
                        bool less = false;
                        uint_fast64_t i = 0;
                        while (!less && i < arrayIndexVector.size()) {
                            less = std::less<storm::expressions::Expression>()(arrayIndexVector.at(i), other.getArrayIndexVector().at(i));
                            if (!less && std::less<storm::expressions::Expression>()(other.getArrayIndexVector().at(i), arrayIndexVector.at(i))) {
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
                    uint_fast64_t i = 0;
                    storm::expressions::JaniSyntacticalEqualityCheckVisitor checker;
                    while (equal && i < arrayIndexVector.size()) {
                        equal &= (checker.isSyntacticallyEqual(getArrayIndexVector().at(i), other.getArrayIndexVector().at(i)));
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
                    for (uint_fast64_t i = 0; i < lValue.getArrayIndexVector().size(); ++i) {
                        stream << "[" << lValue.getArrayIndexVector().at(i) << "]";
                    }
                }
            }
            return stream;
        }

        
    }
}