#include "storm/logic/BoundedUntilFormula.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/logic/FormulaVisitor.h"

#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, boost::optional<UntilBound> const& lowerBound, boost::optional<UntilBound> const& upperBound, BoundedType const& boundedType) : BinaryPathFormula(leftSubformula, rightSubformula), lowerBound(lowerBound), upperBound(upperBound), boundedType(boundedType) {
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
        
        bool BoundedUntilFormula::isStepBounded() const {
            return boundedType == BoundedType::Steps;
        }
        
        bool BoundedUntilFormula::isTimeBounded() const {
            return boundedType == BoundedType::Time;
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
            double bound = this->getLowerBound().evaluateAsDouble();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return bound;
        }
        
        template <>
        double BoundedUntilFormula::getUpperBound() const {
            double bound = this->getUpperBound().evaluateAsDouble();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return bound;
        }
        
        template <>
        uint64_t BoundedUntilFormula::getLowerBound() const {
            int_fast64_t bound = this->getLowerBound().evaluateAsInt();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return static_cast<uint64_t>(bound);
        }
        
        template <>
        uint64_t BoundedUntilFormula::getUpperBound() const {
            int_fast64_t bound = this->getUpperBound().evaluateAsInt();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return static_cast<uint64_t>(bound);
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
            
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}
