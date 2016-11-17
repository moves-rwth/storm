#include "storm/logic/BoundedUntilFormula.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, double lowerBound, double upperBound) : BinaryPathFormula(leftSubformula, rightSubformula), bounds(std::make_pair(lowerBound, upperBound)) {
            STORM_LOG_THROW(lowerBound >= 0 && upperBound >= 0, storm::exceptions::InvalidArgumentException, "Bounded until formula requires non-negative time bounds.");
            STORM_LOG_THROW(lowerBound <= upperBound, storm::exceptions::InvalidArgumentException, "Lower bound of bounded until formula is required to be smaller than the upper bound.");
        }
        
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, uint_fast64_t upperBound) : BinaryPathFormula(leftSubformula, rightSubformula), bounds(upperBound) {
            // Intentionally left empty.
        }
        
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, boost::variant<uint_fast64_t, std::pair<double, double>> const& bounds) : BinaryPathFormula(leftSubformula, rightSubformula), bounds(bounds) {
            // Intentionally left empty.
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

        bool BoundedUntilFormula::hasDiscreteTimeBound() const {
            return bounds.which() == 0;
        }
                
        std::pair<double, double> const& BoundedUntilFormula::getIntervalBounds() const {
            return boost::get<std::pair<double, double>>(bounds);
        }
        
        uint_fast64_t BoundedUntilFormula::getDiscreteTimeBound() const {
            return boost::get<uint_fast64_t>(bounds);
        }
        
        std::ostream& BoundedUntilFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            
            out << " U";
            if (!this->hasDiscreteTimeBound()) {
                std::pair<double, double> const& intervalBounds = getIntervalBounds();
                out << "[" << intervalBounds.first << "," << intervalBounds.second << "] ";
            } else {
                out << "<=" << getDiscreteTimeBound() << " ";
            }
            
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}
