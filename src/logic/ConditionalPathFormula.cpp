#include "src/logic/ConditionalPathFormula.h"

namespace storm {
    namespace logic {
        ConditionalPathFormula::ConditionalPathFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, bool isRewardFormula) : BinaryPathFormula(leftSubformula, rightSubformula), isRewardFormula(isRewardFormula) {
            // Intentionally left empty.
        }
        
        bool ConditionalPathFormula::isConditionalPathFormula() const {
            return true;
        }
        
        bool ConditionalPathFormula::isValidProbabilityPathFormula() const {
            return true;
        }
        
        bool ConditionalPathFormula::isPctlWithConditionalPathFormula() const {
            return this->getLeftSubformula().isPctlPathFormula() && this->getRightSubformula().isPctlPathFormula();
        }
        
        bool ConditionalPathFormula::isRewardPathFormula() const {
            return this->isRewardFormula && this->isValidRewardPathFormula();
        }
        
        bool ConditionalPathFormula::isValidRewardPathFormula() const {
            return this->getLeftSubformula().isRewardPathFormula() && !this->getLeftSubformula().isConditionalPathFormula() && this->getRightSubformula().isPctlPathFormula();
        }
        
        std::shared_ptr<Formula> ConditionalPathFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<ConditionalPathFormula>(this->getLeftSubformula().substitute(substitution), this->getRightSubformula().substitute(substitution));
        }
        
        std::ostream& ConditionalPathFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            out << " || ";
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}