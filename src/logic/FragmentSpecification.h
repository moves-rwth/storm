#ifndef STORM_LOGIC_FRAGMENTSPECIFICATION_H_
#define STORM_LOGIC_FRAGMENTSPECIFICATION_H_

namespace storm {
    namespace logic {
        class FragmentSpecification {
        public:
            FragmentSpecification();
            FragmentSpecification(FragmentSpecification const& other) = default;
            FragmentSpecification(FragmentSpecification&& other) = default;
            FragmentSpecification& operator=(FragmentSpecification const& other) = default;
            FragmentSpecification& operator=(FragmentSpecification&& other) = default;
            
            FragmentSpecification copy() const;
            
            bool areProbabilityOperatorsAllowed() const;
            FragmentSpecification& setProbabilityOperatorsAllowed(bool newValue);
            
            bool areRewardOperatorsAllowed() const;
            FragmentSpecification& setRewardOperatorsAllowed(bool newValue);
            
            bool areTimeOperatorsAllowed() const;
            FragmentSpecification& setTimeOperatorsAllowed(bool newValue);

            bool areLongRunAverageOperatorsAllowed() const;
            FragmentSpecification& setLongRunAverageOperatorsAllowed(bool newValue);

            bool areGloballyFormulasAllowed() const;
            FragmentSpecification& setGloballyFormulasAllowed(bool newValue);

            bool areReachabilityProbabilityFormulasAllowed() const;
            FragmentSpecification& setReachabilityProbabilityFormulasAllowed(bool newValue);

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

            bool areReachbilityTimeFormulasAllowed() const;
            FragmentSpecification& setReachbilityTimeFormulasAllowed(bool newValue);

            bool areNestedOperatorsAllowed() const;
            FragmentSpecification& setNestedOperatorsAllowed(bool newValue);

            bool areNestedPathFormulasAllowed() const;
            FragmentSpecification& setNestedPathFormulasAllowed(bool newValue);
            
            bool areOnlyEventuallyFormuluasInConditionalFormulasAllowed() const;
            FragmentSpecification& setOnlyEventuallyFormuluasInConditionalFormulasAllowed(bool newValue);

            bool areStepBoundedUntilFormulasAllowed() const;
            FragmentSpecification& setStepBoundedUntilFormulasAllowed(bool newValue);
            
            bool areTimeBoundedUntilFormulasAllowed() const;
            FragmentSpecification& setTimeBoundedUntilFormulasAllowed(bool newValue);
            
            bool isVarianceMeasureTypeAllowed() const;
            FragmentSpecification& setVarianceMeasureTypeAllowed(bool newValue);
            
            bool areQuantitativeOperatorResultsAllowed() const;
            FragmentSpecification& setQuantitativeOperatorResultsAllowed(bool newValue);

            bool areQualitativeOperatorResultsAllowed() const;
            FragmentSpecification& setQualitativeOperatorResultsAllowed(bool newValue);
            
            FragmentSpecification& setOperatorsAllowed(bool newValue);
            FragmentSpecification& setTimeAllowed(bool newValue);
            FragmentSpecification& setLongRunAverageProbabilitiesAllowed(bool newValue);
            
        private:
            // Flags that indicate whether it is legal to see such a formula.
            bool probabilityOperator;
            bool rewardOperator;
            bool expectedTimeOperator;
            bool longRunAverageOperator;
            
            bool globallyFormula;
            bool reachabilityProbabilityFormula;
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
            
            bool reachabilityTimeFormula;
            
            // Members that indicate certain restrictions.
            bool nestedOperators;
            bool nestedPathFormulas;
            bool onlyEventuallyFormuluasInConditionalFormulas;
            bool stepBoundedUntilFormulas;
            bool timeBoundedUntilFormulas;
            bool varianceAsMeasureType;
            bool quantitativeOperatorResults;
            bool qualitativeOperatorResults;
        };
        
        // Propositional.
        FragmentSpecification propositional();
        
        // Regular PCTL.
        FragmentSpecification pctl();
        
        // PCTL + cumulative, instantaneous, reachability and long-run rewards.
        FragmentSpecification prctl();
        
        // Regular CSL.
        FragmentSpecification csl();
        
        // CSL + cumulative, instantaneous, reachability and long-run rewards.
        FragmentSpecification csrl();

    }
}

#endif /* STORM_LOGIC_FRAGMENTSPECIFICATION_H_ */