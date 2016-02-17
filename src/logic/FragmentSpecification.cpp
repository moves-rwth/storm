#include "src/logic/FragmentSpecification.h"

namespace storm {
    namespace logic {
        
        FragmentSpecification FragmentSpecification::copy() const {
            return FragmentSpecification(*this);
        }
        
        bool FragmentSpecification::areProbabilityOperatorsAllowed() const {
            return probabilityOperator;
        }
        
        FragmentSpecification& FragmentSpecification::setProbabilityOperatorsAllowed(bool newValue) {
            this->probabilityOperator = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areRewardOperatorsAllowed() const {
            return rewardOperator;
        }
        
        FragmentSpecification& FragmentSpecification::setRewardOperatorsAllowed(bool newValue) {
            this->rewardOperator = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areExpectedTimeOperatorsAllowed() const {
            return expectedTimeOperator;
        }
        
        FragmentSpecification& FragmentSpecification::setExpectedTimeOperatorsAllowed(bool newValue) {
            this->expectedTimeOperator = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areLongRunAverageOperatorsAllowed() const {
            return longRunAverageOperator;
        }
        
        FragmentSpecification& FragmentSpecification::setLongRunAverageOperatorsAllowed(bool newValue) {
            this->longRunAverageOperator = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areGloballyFormulasAllowed() const {
            return globallyFormula;
        }
        
        FragmentSpecification& FragmentSpecification::setGloballyFormulasAllowed(bool newValue) {
            this->globallyFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areEventuallyFormulasAllowed() const {
            return eventuallyFormula;
        }
        
        FragmentSpecification& FragmentSpecification::setEventuallyFormulasAllowed(bool newValue) {
            this->eventuallyFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areNextFormulasAllowed() const {
            return nextFormula;
        }
        
        FragmentSpecification& FragmentSpecification::setNextFormulasAllowed(bool newValue) {
            this->nextFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areUntilFormulasAllowed() const {
            return untilFormula;
        }
        
        FragmentSpecification& FragmentSpecification::setUntilFormulasAllowed(bool newValue) {
            this->untilFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areBoundedUntilFormulasAllowed() const {
            return boundedUntilFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setBoundedUntilFormulasAllowed(bool newValue) {
            this->boundedUntilFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areAtomicExpressionFormulasAllowed() const {
            return atomicExpressionFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setAtomicExpressionFormulasAllowed(bool newValue) {
            this->atomicExpressionFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areAtomicLabelFormulasAllowed() const {
            return atomicLabelFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setAtomicLabelFormulasAllowed(bool newValue) {
            this->atomicLabelFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areBooleanLiteralFormulasAllowed() const {
            return booleanLiteralFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setBooleanLiteralFormulasAllowed(bool newValue) {
            this->booleanLiteralFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areUnaryBooleanStateFormulasAllowed() const {
            return unaryBooleanStateFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setUnaryBooleanStateFormulasAllowed(bool newValue) {
            this->unaryBooleanStateFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areBinaryBooleanStateFormulasAllowed() const {
            return binaryBooleanStateFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setBinaryBooleanStateFormulasAllowed(bool newValue) {
            this->binaryBooleanStateFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areCumulativeRewardFormulasAllowed() const {
            return cumulativeRewardFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setCumulativeRewardFormulasAllowed(bool newValue) {
            this->cumulativeRewardFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areInstantaneousRewardFormulasAllowed() const {
            return instantaneousRewardFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setInstantaneousFormulasAllowed(bool newValue) {
            this->instantaneousRewardFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areReachabilityRewardFormulasAllowed() const {
            return reachabilityRewardFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setReachabilityRewardFormulasAllowed(bool newValue) {
            this->reachabilityRewardFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areLongRunAverageRewardFormulasAllowed() const {
            return longRunAverageRewardFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setLongRunAverageRewardFormulasAllowed(bool newValue) {
            this->longRunAverageRewardFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areConditionalProbabilityFormulasAllowed() const {
            return conditionalProbabilityFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setConditionalProbabilityFormulasAllowed(bool newValue) {
            this->conditionalProbabilityFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areConditionalRewardFormulasFormulasAllowed() const {
            return conditionalRewardFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setConditionalRewardFormulasAllowed(bool newValue) {
            this->conditionalRewardFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areReachbilityExpectedTimeFormulasAllowed() const {
            return reachabilityExpectedTimeFormula;
        }
            
        FragmentSpecification& FragmentSpecification::setReachbilityExpectedTimeFormulasAllowed(bool newValue) {
            this->reachabilityExpectedTimeFormula = newValue;
            return *this;
        }
        
        bool FragmentSpecification::areNestedOperatorsAllowed() const {
            return this->nestedOperators;
        }
            
        FragmentSpecification& FragmentSpecification::setNestedOperatorsAllowed(bool newValue) {
            this->nestedOperators = newValue;
            return *this;
        }
        
    }
}