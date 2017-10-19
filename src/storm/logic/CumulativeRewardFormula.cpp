#include "storm/logic/CumulativeRewardFormula.h"

#include "storm/logic/FormulaVisitor.h"

#include "storm/utility/macros.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace logic {
        CumulativeRewardFormula::CumulativeRewardFormula(TimeBound const& bound, TimeBoundReference const& timeBoundReference) : timeBoundReference(timeBoundReference), bound(bound) {
            // Intentionally left empty.
        }

        bool CumulativeRewardFormula::isCumulativeRewardFormula() const {
            return true;
        }
        
        bool CumulativeRewardFormula::isRewardPathFormula() const {
            return true;
        }
        
        boost::any CumulativeRewardFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        void CumulativeRewardFormula::gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const {
            if (this->isRewardBounded()) {
                referencedRewardModels.insert(this->getTimeBoundReference().getRewardName());
            }
        }
        
        TimeBoundType CumulativeRewardFormula::getTimeBoundType() const {
            return timeBoundReference.getType();
        }
        
        TimeBoundReference const& CumulativeRewardFormula::getTimeBoundReference() const {
            return timeBoundReference;
        }

        bool CumulativeRewardFormula::isStepBounded() const {
            return getTimeBoundType() == TimeBoundType::Steps;
        }
        
        bool CumulativeRewardFormula::isTimeBounded() const {
            return getTimeBoundType() == TimeBoundType::Time;
        }
        
        bool CumulativeRewardFormula::isRewardBounded() const {
            return getTimeBoundType() == TimeBoundType::Reward;
        }
        
        bool CumulativeRewardFormula::isBoundStrict() const {
            return bound.isStrict();
        }
        
        bool CumulativeRewardFormula::hasIntegerBound() const {
            return bound.getBound().hasIntegerType();
        }
        
        storm::expressions::Expression const& CumulativeRewardFormula::getBound() const {
            return bound.getBound();
        }
        
        template <>
        double CumulativeRewardFormula::getBound() const {
            checkNoVariablesInBound(bound.getBound());
            double value = bound.getBound().evaluateAsDouble();
            STORM_LOG_THROW(value >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return value;
        }

        template <>
        storm::RationalNumber CumulativeRewardFormula::getBound() const {
            checkNoVariablesInBound(bound.getBound());
            storm::RationalNumber value = bound.getBound().evaluateAsRational();
            STORM_LOG_THROW(value >= storm::utility::zero<storm::RationalNumber>(), storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return value;
        }

        template <>
        uint64_t CumulativeRewardFormula::getBound() const {
            checkNoVariablesInBound(bound.getBound());
            uint64_t value = bound.getBound().evaluateAsInt();
            STORM_LOG_THROW(value >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return value;
        }
        
        template <>
        double CumulativeRewardFormula::getNonStrictBound() const {
            double bound = getBound<double>();
            STORM_LOG_THROW(bound > 0, storm::exceptions::InvalidPropertyException, "Cannot retrieve non-strict bound from strict zero-bound.");
            return bound;
        }
        
        template <>
        uint64_t CumulativeRewardFormula::getNonStrictBound() const {
            int_fast64_t bound = getBound<uint64_t>();
            if (isBoundStrict()) {
                STORM_LOG_THROW(bound > 0, storm::exceptions::InvalidPropertyException, "Cannot retrieve non-strict bound from strict zero-bound.");
                return bound - 1;
            } else {
                return bound;
            }
        }
        
        void CumulativeRewardFormula::checkNoVariablesInBound(storm::expressions::Expression const& bound) {
            STORM_LOG_THROW(!bound.containsVariables(), storm::exceptions::InvalidOperationException, "Cannot evaluate time-bound '" << bound << "' as it contains undefined constants.");
        }
        
        std::ostream& CumulativeRewardFormula::writeToStream(std::ostream& out) const {
            out << "C";
             if(getTimeBoundReference().isRewardBound()) {
                out << "{\"" << getTimeBoundReference().getRewardName() << "\"}";
             }
             out << "<=" << this->getBound();
            return out;
        }
    }
}
