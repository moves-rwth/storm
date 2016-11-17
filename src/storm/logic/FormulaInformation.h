#ifndef STORM_LOGIC_FORMULAINFORMATION_H_
#define STORM_LOGIC_FORMULAINFORMATION_H_

namespace storm {
    namespace logic {
        
        class FormulaInformation {
        public:
            FormulaInformation();
            FormulaInformation(FormulaInformation const& other) = default;
            FormulaInformation(FormulaInformation&& other) = default;
            FormulaInformation& operator=(FormulaInformation const& other) = default;
            FormulaInformation& operator=(FormulaInformation&& other) = default;
            
            bool containsRewardOperator() const;
            bool containsNextFormula() const;
            bool containsBoundedUntilFormula() const;
            
            FormulaInformation join(FormulaInformation const& other);
            
            FormulaInformation& setContainsRewardOperator(bool newValue = true);
            FormulaInformation& setContainsNextFormula(bool newValue = true);
            FormulaInformation& setContainsBoundedUntilFormula(bool newValue = true);
            
        private:
            bool mContainsRewardOperator;
            bool mContainsNextFormula;
            bool mContainsBoundedUntilFormula;
        };
        
    }
}

#endif /* STORM_LOGIC_FORMULAINFORMATION_H_ */