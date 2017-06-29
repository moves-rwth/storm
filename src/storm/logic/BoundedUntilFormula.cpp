#include "storm/logic/BoundedUntilFormula.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/logic/FormulaVisitor.h"

#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace logic {
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, boost::optional<TimeBound> const& lowerBound, boost::optional<TimeBound> const& upperBound, TimeBoundType const& timeBoundType) : BinaryPathFormula(leftSubformula, rightSubformula), timeBoundType(timeBoundType), lowerBound(lowerBound), upperBound(upperBound) {
            STORM_LOG_THROW(lowerBound || upperBound, storm::exceptions::InvalidArgumentException, "Bounded until formula requires at least one bound.");
        }
        
        bool BoundedUntilFormula::isBoundedUntilFormula() const {
            return true;
        }
        
        bool BoundedUntilFormula::isProbabilityPathFormula() const {
            return true;
        }
        
        boost::any BoundedUntilFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        TimeBoundType const& BoundedUntilFormula::getTimeBoundType() const {
            return timeBoundType;
        }
        
        bool BoundedUntilFormula::isStepBounded() const {
            return timeBoundType == TimeBoundType::Steps;
        }
        
        bool BoundedUntilFormula::isTimeBounded() const {
            return timeBoundType == TimeBoundType::Time;
        }
        
        bool BoundedUntilFormula::isLowerBoundStrict() const {
            return lowerBound.get().isStrict();
        }
        
        bool BoundedUntilFormula::hasLowerBound() const {
            return static_cast<bool>(lowerBound);
        }
        
        bool BoundedUntilFormula::hasIntegerLowerBound() const {
            return lowerBound.get().getBound().hasIntegerType();
        }

        bool BoundedUntilFormula::isUpperBoundStrict() const {
            return upperBound.get().isStrict();
        }

        bool BoundedUntilFormula::hasUpperBound() const {
            return static_cast<bool>(upperBound);
        }

        bool BoundedUntilFormula::hasIntegerUpperBound() const {
            return upperBound.get().getBound().hasIntegerType();
        }

        storm::expressions::Expression const& BoundedUntilFormula::getLowerBound() const {
            return lowerBound.get().getBound();
        }
        
        storm::expressions::Expression const& BoundedUntilFormula::getUpperBound() const {
            return upperBound.get().getBound();
        }
        
        template <>
        double BoundedUntilFormula::getLowerBound() const {
            checkNoVariablesInBound(this->getLowerBound());
            double bound = this->getLowerBound().evaluateAsDouble();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return bound;
        }
        
        template <>
        double BoundedUntilFormula::getUpperBound() const {
            checkNoVariablesInBound(this->getUpperBound());
            double bound = this->getUpperBound().evaluateAsDouble();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return bound;
        }
        
        template <>
        uint64_t BoundedUntilFormula::getLowerBound() const {
            checkNoVariablesInBound(this->getLowerBound());
            int_fast64_t bound = this->getLowerBound().evaluateAsInt();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return static_cast<uint64_t>(bound);
        }
        
        template <>
        uint64_t BoundedUntilFormula::getUpperBound() const {
            checkNoVariablesInBound(this->getUpperBound());
            int_fast64_t bound = this->getUpperBound().evaluateAsInt();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return static_cast<uint64_t>(bound);
        }
        
        template <>
        double BoundedUntilFormula::getNonStrictUpperBound() const {
            double bound = getUpperBound<double>();
            STORM_LOG_THROW(!isUpperBoundStrict() || bound > 0, storm::exceptions::InvalidPropertyException, "Cannot retrieve non-strict bound from strict zero-bound.");
            return bound;
        }

        template <>
        uint64_t BoundedUntilFormula::getNonStrictUpperBound() const {
            int_fast64_t bound = getUpperBound<uint64_t>();
            if (isUpperBoundStrict()) {
                STORM_LOG_THROW(bound > 0, storm::exceptions::InvalidPropertyException, "Cannot retrieve non-strict bound from strict zero-bound.");
                return bound - 1;
            } else {
                return bound;
            }
        }
        
        void BoundedUntilFormula::checkNoVariablesInBound(storm::expressions::Expression const& bound) {
            STORM_LOG_THROW(!bound.containsVariables(), storm::exceptions::InvalidOperationException, "Cannot evaluate time-bound '" << bound << "' as it contains undefined constants.");
        }
        
        std::ostream& BoundedUntilFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            
            out << " U";
            if (this->hasLowerBound()) {
                if (this->hasUpperBound()) {
                    if (this->isLowerBoundStrict()) {
                        out << "(";
                    } else {
                        out << "[";
                    }
                    out << this->getLowerBound();
                    out << ", ";
                    out << this->getUpperBound();
                    if (this->isUpperBoundStrict()) {
                        out << ")";
                    } else {
                        out << "]";
                    }
                } else {
                    if (this->isLowerBoundStrict()) {
                        out << ">";
                    } else {
                        out << ">=";
                    }
                    out << getLowerBound();
                }
            } else {
                if (this->isUpperBoundStrict()) {
                    out << "<";
                } else {
                    out << "<=";
                }
                out << this->getUpperBound();
            }
            out << " ";
            
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}
