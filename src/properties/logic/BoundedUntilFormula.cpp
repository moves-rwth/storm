#include "src/properties/logic/BoundedUntilFormula.h"

namespace storm {
    namespace logic {
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula, double lowerBound, double upperBound) : BinaryPathFormula(leftSubformula, rightSubformula), bounds(std::make_pair(lowerBound, upperBound)) {
            // Intentionally left empty.
        }
        
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula, uint_fast64_t upperBound) : BinaryPathFormula(leftSubformula, rightSubformula), bounds(upperBound) {
            // Intentionally left empty.
        }
        
        bool BoundedUntilFormula::isBoundedUntilFormula() const {
            return true;
        }
        
        bool BoundedUntilFormula::isIntervalBounded() const {
            return bounds.which() == 1;
        }
        
        bool BoundedUntilFormula::isIntegerUpperBounded() const {
            return bounds.which() == 0;
        }
        
        std::pair<double, double> const& BoundedUntilFormula::getIntervalBounds() const {
            return boost::get<std::pair<double, double>>(bounds);
        }
        
        uint_fast64_t BoundedUntilFormula::getUpperBound() const {
            return boost::get<uint_fast64_t>(bounds);
        }
        
        std::ostream& BoundedUntilFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            
            out << " U";
            if (this->isIntervalBounded()) {
                std::pair<double, double> const& intervalBounds = getIntervalBounds();
                out << "[" << intervalBounds.first << "," << intervalBounds.second << "] ";
            } else {
                out << "<=" << getUpperBound() << " ";
            }
            
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}