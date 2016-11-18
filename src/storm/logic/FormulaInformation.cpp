#include "storm/logic/FormulaInformation.h"

namespace storm {
    namespace logic {
        FormulaInformation::FormulaInformation() {
            this->mContainsRewardOperator = false;
            this->mContainsNextFormula = false;
            this->mContainsBoundedUntilFormula = false;
        }
        
        bool FormulaInformation::containsRewardOperator() const {
            return this->mContainsRewardOperator;
        }
        
        bool FormulaInformation::containsNextFormula() const {
            return this->mContainsNextFormula;
        }
        
        bool FormulaInformation::containsBoundedUntilFormula() const {
            return this->mContainsBoundedUntilFormula;
        }
        
        FormulaInformation FormulaInformation::join(FormulaInformation const& other) {
            FormulaInformation result;
            result.mContainsRewardOperator = this->containsRewardOperator() || other.containsRewardOperator();
            result.mContainsNextFormula = this->containsNextFormula() || other.containsNextFormula();
            result.mContainsBoundedUntilFormula = this->containsBoundedUntilFormula() || other.containsBoundedUntilFormula();
            return result;
        }
        
        FormulaInformation& FormulaInformation::setContainsRewardOperator(bool newValue) {
            this->mContainsRewardOperator = newValue;
            return *this;
        }
        
        FormulaInformation& FormulaInformation::setContainsNextFormula(bool newValue) {
            this->mContainsNextFormula = newValue;
            return *this;
        }
        
        FormulaInformation& FormulaInformation::setContainsBoundedUntilFormula(bool newValue) {
            this->mContainsBoundedUntilFormula = newValue;
            return *this;
        }
    }
}
