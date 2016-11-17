#include "src/storm/logic/CumulativeRewardFormula.h"

#include "src/storm/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        CumulativeRewardFormula::CumulativeRewardFormula(uint_fast64_t timeBound) : timeBound(timeBound) {
            // Intentionally left empty.
        }
        
        CumulativeRewardFormula::CumulativeRewardFormula(double timeBound) : timeBound(timeBound) {
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
        
        bool CumulativeRewardFormula::hasDiscreteTimeBound() const {
            return timeBound.which() == 0;
        }
        
        uint_fast64_t CumulativeRewardFormula::getDiscreteTimeBound() const {
            return boost::get<uint_fast64_t>(timeBound);
        }
        
        bool CumulativeRewardFormula::hasContinuousTimeBound() const {
            return timeBound.which() == 1;
        }
        
        double CumulativeRewardFormula::getContinuousTimeBound() const {
            if (this->hasDiscreteTimeBound()) {
                return this->getDiscreteTimeBound();
            } else {
                return boost::get<double>(timeBound);
            }
        }
        
        std::ostream& CumulativeRewardFormula::writeToStream(std::ostream& out) const {
            if (this->hasDiscreteTimeBound()) {
                out << "C<=" << this->getDiscreteTimeBound();
            } else {
                out << "C<=" << this->getContinuousTimeBound();
            }
            return out;
        }
    }
}
