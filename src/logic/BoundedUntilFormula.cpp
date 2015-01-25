#include "src/logic/BoundedUntilFormula.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace logic {
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, double lowerBound, double upperBound) : BinaryPathFormula(leftSubformula, rightSubformula), bounds(std::make_pair(lowerBound, upperBound)) {
            STORM_LOG_THROW(lowerBound >= 0 && upperBound >= 0, storm::exceptions::InvalidArgumentException, "Bounded until formula requires non-negative time bounds.");
            STORM_LOG_THROW(lowerBound <= upperBound, storm::exceptions::InvalidArgumentException, "Lower bound of bounded until formula is required to be smaller than the upper bound.");
        }
        
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, uint_fast64_t upperBound) : BinaryPathFormula(leftSubformula, rightSubformula), bounds(upperBound) {
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
        
        bool BoundedUntilFormula::isPctlPathFormula() const {
            return this->isIntegerUpperBounded() && this->getLeftSubformula().isPctlStateFormula() && this->getRightSubformula().isPctlStateFormula();
        }
        
        bool BoundedUntilFormula::isCslPathFormula() const {
            return this->getLeftSubformula().isCslStateFormula() && this->getRightSubformula().isCslStateFormula();
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