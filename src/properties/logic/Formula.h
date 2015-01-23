#ifndef STORM_LOGIC_FORMULA_H_
#define STORM_LOGIC_FORMULA_H_

#include <memory>
#include <iostream>

#include "src/modelchecker/CheckResult.h"

namespace storm {
    namespace logic {
        // Forward-declare all formula classes.
        class PathFormula;
        class StateFormula;
        class BinaryStateFormula;
        class UnaryStateFormula;
        class BinaryBooleanStateFormula;
        class UnaryBooleanStateFormula;
        class BooleanLiteralFormula;
        class AtomicExpressionFormula;
        class AtomicLabelFormula;
        class UntilFormula;
        class BoundedUntilFormula;
        class EventuallyFormula;
        class GloballyFormula;
        class BinaryPathFormula;
        class UnaryPathFormula;
        class ConditionalPathFormula;
        class NextFormula;
        class SteadyStateFormula;
        class PathRewardFormula;
        class CumulativeRewardFormula;
        class InstantaneousRewardFormula;
        class ReachabilityRewardFormula;
        class ProbabilityOperatorFormula;
        class RewardOperatorFormula;

        // Also foward-declare base model checker class.
        class ModelChecker;
        
        class Formula {
        public:
            // Make the destructor virtual to allow deletion of objects of subclasses via a pointer to this class.
            virtual ~Formula() {
                // Intentionally left empty.
            };
            
            friend std::ostream& operator<<(std::ostream& out, Formula const& formula);
            
            // Methods for querying the exact formula type.
            virtual bool isPathFormula() const;
            virtual bool isStateFormula() const;
            virtual bool isBinaryStateFormula() const;
            virtual bool isUnaryStateFormula() const;
            virtual bool isBinaryBooleanStateFormula() const;
            virtual bool isUnaryBooleanStateFormula() const;
            virtual bool isBooleanLiteralFormula() const;
            virtual bool isTrue() const;
            virtual bool isFalse() const;
            virtual bool isAtomicExpressionFormula() const;
            virtual bool isAtomicLabelFormula() const;
            virtual bool isUntilFormula() const;
            virtual bool isBoundedUntilFormula() const;
            virtual bool isEventuallyFormula() const;
            virtual bool isGloballyFormula() const;
            virtual bool isBinaryPathFormula() const;
            virtual bool isUnaryPathFormula() const;
            virtual bool isConditionalPathFormula() const;
            virtual bool isNextFormula() const;
            virtual bool isSteadyStateFormula() const;
            virtual bool isPathRewardFormula() const;
            virtual bool isCumulativeRewardFormula() const;
            virtual bool isInstantaneousRewardFormula() const;
            virtual bool isReachabilityRewardFormula() const;
            virtual bool isProbabilityOperator() const;
            virtual bool isRewardOperator() const;

            virtual bool isPctlPathFormula() const;
            virtual bool isPctlStateFormula() const;
            virtual bool isPltlFormula() const;
            
            PathFormula& asPathFormula();
            PathFormula const& asPathFormula() const;
        
            StateFormula& asStateFormula();
            StateFormula const& asStateFormula() const;
            
            BinaryStateFormula& asBinaryStateFormula();
            BinaryStateFormula const& asBinaryStateFormula() const;
            
            UnaryStateFormula& asUnaryStateFormula();
            UnaryStateFormula const& asUnaryStateFormula() const;
            
            BinaryBooleanStateFormula& asBinaryBooleanStateFormula();
            BinaryBooleanStateFormula const& asBinaryBooleanStateFormula() const;

            UnaryBooleanStateFormula& asUnaryBooleanStateFormula();
            UnaryBooleanStateFormula const& asUnaryBooleanStateFormula() const;

            BooleanLiteralFormula& asBooleanLiteralFormula();
            BooleanLiteralFormula const& asBooleanLiteralFormula() const;
            
            AtomicExpressionFormula& asAtomicExpressionFormula();
            AtomicExpressionFormula const& asAtomicExpressionFormula() const;
            
            AtomicLabelFormula& asAtomicLabelFormula();
            AtomicLabelFormula const& asAtomicLabelFormula() const;
            
            UntilFormula& asUntilFormula();
            UntilFormula const& asUntilFormula() const;
            
            BoundedUntilFormula& asBoundedUntilFormula();
            BoundedUntilFormula const& asBoundedUntilFormula() const;
            
            EventuallyFormula& asEventuallyFormula();
            EventuallyFormula const& asEventuallyFormula() const;
            
            GloballyFormula& asGloballyFormula();
            GloballyFormula const& asGloballyFormula() const;
            
            BinaryPathFormula& asBinaryPathFormula();
            BinaryPathFormula const& asBinaryPathFormula() const;
            
            UnaryPathFormula& asUnaryPathFormula();
            UnaryPathFormula const& asUnaryPathFormula() const;
            
            ConditionalPathFormula& asConditionalPathFormula();
            ConditionalPathFormula const& asConditionalPathFormula() const;
            
            NextFormula& asNextFormula();
            NextFormula const& asNextFormula() const;
            
            SteadyStateFormula& asSteadyStateFormula();
            SteadyStateFormula const& asSteadyStateFormula() const;
            
            PathRewardFormula& asPathRewardFormula();
            PathRewardFormula const& asPathRewardFormula() const;
            
            CumulativeRewardFormula& asCumulativeRewardFormula();
            CumulativeRewardFormula const& asCumulativeRewardFormula() const;
            
            InstantaneousRewardFormula& asInstantaneousRewardFormula();
            InstantaneousRewardFormula const& asInstantaneousRewardFormula() const;
            
            ReachabilityRewardFormula& asReachabilityRewardFormula();
            ReachabilityRewardFormula const& asReachabilityRewardFormula() const;
            
            ProbabilityOperatorFormula& asProbabilityOperatorFormula();
            ProbabilityOperatorFormula const& asProbabilityOperatorFormula() const;
            
            RewardOperatorFormula& asRewardOperatorFormula();
            RewardOperatorFormula const& asRewardOperatorFormula() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const = 0;
            
        private:
            // Currently empty.
        };
        
        std::ostream& operator<<(std::ostream& out, Formula const& formula);
    }
}

#endif /* STORM_LOGIC_FORMULA_H_ */