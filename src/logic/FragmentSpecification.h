#ifndef STORM_LOGIC_FRAGMENTSPECIFICATION_H_
#define STORM_LOGIC_FRAGMENTSPECIFICATION_H_

namespace storm {
    namespace logic {
        class FragmentSpecification {
        public:
            FragmentSpecification copy() const;
            
            bool areProbabilityOperatorsAllowed() const;
            FragmentSpecification& setProbabilityOperatorsAllowed(bool newValue);
            
            bool areRewardOperatorsAllowed() const;
            FragmentSpecification& setRewardOperatorsAllowed(bool newValue);
            
            bool areExpectedTimeOperatorsAllowed() const;
            FragmentSpecification& setExpectedTimeOperatorsAllowed(bool newValue);

            bool areLongRunAverageOperatorsAllowed() const;
            FragmentSpecification& setLongRunAverageOperatorsAllowed(bool newValue);

            bool areGloballyFormulasAllowed() const;
            FragmentSpecification& setGloballyFormulasAllowed(bool newValue);

            bool areEventuallyFormulasAllowed() const;
            FragmentSpecification& setEventuallyFormulasAllowed(bool newValue);

            bool areNextFormulasAllowed() const;
            FragmentSpecification& setNextFormulasAllowed(bool newValue);

            bool areUntilFormulasAllowed() const;
            FragmentSpecification& setUntilFormulasAllowed(bool newValue);

            bool areBoundedUntilFormulasAllowed() const;
            FragmentSpecification& setBoundedUntilFormulasAllowed(bool newValue);

            bool areAtomicExpressionFormulasAllowed() const;
            FragmentSpecification& setAtomicExpressionFormulasAllowed(bool newValue);

            bool areAtomicLabelFormulasAllowed() const;
            FragmentSpecification& setAtomicLabelFormulasAllowed(bool newValue);

            bool areBooleanLiteralFormulasAllowed() const;
            FragmentSpecification& setBooleanLiteralFormulasAllowed(bool newValue);

            bool areUnaryBooleanStateFormulasAllowed() const;
            FragmentSpecification& setUnaryBooleanStateFormulasAllowed(bool newValue);

            bool areBinaryBooleanStateFormulasAllowed() const;
            FragmentSpecification& setBinaryBooleanStateFormulasAllowed(bool newValue);

            bool areCumulativeRewardFormulasAllowed() const;
            FragmentSpecification& setCumulativeRewardFormulasAllowed(bool newValue);

            bool areInstantaneousRewardFormulasAllowed() const;
            FragmentSpecification& setInstantaneousFormulasAllowed(bool newValue);

            bool areReachabilityRewardFormulasAllowed() const;
            FragmentSpecification& setReachabilityRewardFormulasAllowed(bool newValue);
            
            bool areLongRunAverageRewardFormulasAllowed() const;
            FragmentSpecification& setLongRunAverageRewardFormulasAllowed(bool newValue);

            bool areConditionalProbabilityFormulasAllowed() const;
            FragmentSpecification& setConditionalProbabilityFormulasAllowed(bool newValue);

            bool areConditionalRewardFormulasFormulasAllowed() const;
            FragmentSpecification& setConditionalRewardFormulasAllowed(bool newValue);

            bool areReachbilityExpectedTimeFormulasAllowed() const;
            FragmentSpecification& setReachbilityExpectedTimeFormulasAllowed(bool newValue);

            bool areNestedOperatorsAllowed() const;
            FragmentSpecification& setNestedOperatorsAllowed(bool newValue);

        private:
            // Flags that indicate whether it is legal to see such a formula.
            bool probabilityOperator;
            bool rewardOperator;
            bool expectedTimeOperator;
            bool longRunAverageOperator;
            
            bool globallyFormula;
            bool eventuallyFormula;
            bool nextFormula;
            bool untilFormula;
            bool boundedUntilFormula;
            
            bool atomicExpressionFormula;
            bool atomicLabelFormula;
            bool booleanLiteralFormula;
            bool unaryBooleanStateFormula;
            bool binaryBooleanStateFormula;
            
            bool cumulativeRewardFormula;
            bool instantaneousRewardFormula;
            bool reachabilityRewardFormula;
            bool longRunAverageRewardFormula;
            
            bool conditionalProbabilityFormula;
            bool conditionalRewardFormula;
            
            bool reachabilityExpectedTimeFormula;
            
            // Members that indicate certain restrictions.
            bool nestedOperators;
            
        };
    }
}

#endif /* STORM_LOGIC_FRAGMENTSPECIFICATION_H_ */