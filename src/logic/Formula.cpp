#include "src/logic/Formulas.h"

namespace storm {
    namespace logic {
        bool Formula::isPathFormula() const {
            return false;
        }
        
        bool Formula::isStateFormula() const {
            return false;
        }
        
        bool Formula::isBinaryStateFormula() const {
            return false;
        }
        
        bool Formula::isUnaryStateFormula() const {
            return false;
        }
        
        bool Formula::isBinaryBooleanStateFormula() const {
            return false;
        }
        
        bool Formula::isUnaryBooleanStateFormula() const {
            return false;
        }
        
        bool Formula::isBooleanLiteralFormula() const {
            return false;
        }
        
        bool Formula::isTrue() const {
            return false;
        }
        
        bool Formula::isFalse() const {
            return false;
        }
        
        bool Formula::isAtomicExpressionFormula() const {
            return false;
        }
        
        bool Formula::isAtomicLabelFormula() const {
            return false;
        }
        
        bool Formula::isUntilFormula() const {
            return false;
        }
        
        bool Formula::isBoundedUntilFormula() const {
            return false;
        }
        
        bool Formula::isEventuallyFormula() const {
            return false;
        }
        
        bool Formula::isGloballyFormula() const {
            return false;
        }
        
        bool Formula::isBinaryPathFormula() const {
            return false;
        }
        
        bool Formula::isUnaryPathFormula() const {
            return false;
        }
        
        bool Formula::isConditionalPathFormula() const {
            return false;
        }
        
        bool Formula::isNextFormula() const {
            return false;
        }
        
        bool Formula::isSteadyStateOperatorFormula() const {
            return false;
        }
        
        bool Formula::isPathRewardFormula() const {
            return false;
        }
        
        bool Formula::isCumulativeRewardFormula() const {
            return false;
        }
        
        bool Formula::isInstantaneousRewardFormula() const {
            return false;
        }
        
        bool Formula::isReachabilityRewardFormula() const {
            return false;
        }
        
        bool Formula::isProbabilityOperator() const {
            return false;
        }
        
        bool Formula::isRewardOperator() const {
            return false;
        }
        
        bool Formula::isPctlPathFormula() const {
            return false;
        }
        
        bool Formula::isPctlStateFormula() const {
            return false;
        }
        
        bool Formula::isPltlFormula() const {
            return false;
        }
        
        bool Formula::isPropositionalFormula() const {
            return false;
        }
        
        PathFormula& Formula::asPathFormula() {
            return dynamic_cast<PathFormula&>(*this);
        }
        
        PathFormula const& Formula::asPathFormula() const {
            return dynamic_cast<PathFormula const&>(*this);
        }
        
        StateFormula& Formula::asStateFormula() {
            return dynamic_cast<StateFormula&>(*this);
        }
        
        StateFormula const& Formula::asStateFormula() const {
            return dynamic_cast<StateFormula const&>(*this);
        }
        
        BinaryStateFormula& Formula::asBinaryStateFormula() {
            return dynamic_cast<BinaryStateFormula&>(*this);
        }
        
        BinaryStateFormula const& Formula::asBinaryStateFormula() const {
            return dynamic_cast<BinaryStateFormula const&>(*this);
        }
        
        UnaryStateFormula& Formula::asUnaryStateFormula() {
            return dynamic_cast<UnaryStateFormula&>(*this);
        }
        
        UnaryStateFormula const& Formula::asUnaryStateFormula() const {
            return dynamic_cast<UnaryStateFormula const&>(*this);
        }
        
        ConditionalPathFormula& Formula::asConditionalPathFormula() {
            return dynamic_cast<ConditionalPathFormula&>(*this);
        }
        
        ConditionalPathFormula const& Formula::asConditionalPathFormula() const {
            return dynamic_cast<ConditionalPathFormula const&>(*this);
        }
        
        BinaryBooleanStateFormula& Formula::asBinaryBooleanStateFormula() {
            return dynamic_cast<BinaryBooleanStateFormula&>(*this);
        }
        
        BinaryBooleanStateFormula const& Formula::asBinaryBooleanStateFormula() const {
            return dynamic_cast<BinaryBooleanStateFormula const&>(*this);
        }
        
        UnaryBooleanStateFormula& Formula::asUnaryBooleanStateFormula() {
            return dynamic_cast<UnaryBooleanStateFormula&>(*this);
        }
        
        UnaryBooleanStateFormula const& Formula::asUnaryBooleanStateFormula() const {
            return dynamic_cast<UnaryBooleanStateFormula const&>(*this);
        }
        
        BooleanLiteralFormula& Formula::asBooleanLiteralFormula() {
            return dynamic_cast<BooleanLiteralFormula&>(*this);
        }
        
        BooleanLiteralFormula const& Formula::asBooleanLiteralFormula() const {
            return dynamic_cast<BooleanLiteralFormula const&>(*this);
        }
        
        AtomicExpressionFormula& Formula::asAtomicExpressionFormula() {
            return dynamic_cast<AtomicExpressionFormula&>(*this);
        }
        
        AtomicExpressionFormula const& Formula::asAtomicExpressionFormula() const {
            return dynamic_cast<AtomicExpressionFormula const&>(*this);
        }
        
        AtomicLabelFormula& Formula::asAtomicLabelFormula() {
            return dynamic_cast<AtomicLabelFormula&>(*this);
        }
        
        AtomicLabelFormula const& Formula::asAtomicLabelFormula() const {
            return dynamic_cast<AtomicLabelFormula const&>(*this);
        }
        
        UntilFormula& Formula::asUntilFormula() {
            return dynamic_cast<UntilFormula&>(*this);
        }
        
        UntilFormula const& Formula::asUntilFormula() const {
            return dynamic_cast<UntilFormula const&>(*this);
        }
        
        BoundedUntilFormula& Formula::asBoundedUntilFormula() {
            return dynamic_cast<BoundedUntilFormula&>(*this);
        }
        
        BoundedUntilFormula const& Formula::asBoundedUntilFormula() const {
            return dynamic_cast<BoundedUntilFormula const&>(*this);
        }
        
        EventuallyFormula& Formula::asEventuallyFormula() {
            return dynamic_cast<EventuallyFormula&>(*this);
        }
        
        EventuallyFormula const& Formula::asEventuallyFormula() const {
            return dynamic_cast<EventuallyFormula const&>(*this);
        }
        
        GloballyFormula& Formula::asGloballyFormula() {
            return dynamic_cast<GloballyFormula&>(*this);
        }
        
        GloballyFormula const& Formula::asGloballyFormula() const {
            return dynamic_cast<GloballyFormula const&>(*this);
        }
        
        BinaryPathFormula& Formula::asBinaryPathFormula() {
            return dynamic_cast<BinaryPathFormula&>(*this);
        }
        
        BinaryPathFormula const& Formula::asBinaryPathFormula() const {
            return dynamic_cast<BinaryPathFormula const&>(*this);
        }
        
        UnaryPathFormula& Formula::asUnaryPathFormula() {
            return dynamic_cast<UnaryPathFormula&>(*this);
        }
        
        UnaryPathFormula const& Formula::asUnaryPathFormula() const {
            return dynamic_cast<UnaryPathFormula const&>(*this);
        }
        
        NextFormula& Formula::asNextFormula() {
            return dynamic_cast<NextFormula&>(*this);
        }
        
        NextFormula const& Formula::asNextFormula() const {
            return dynamic_cast<NextFormula const&>(*this);
        }
        
        SteadyStateOperatorFormula& Formula::asSteadyStateFormula() {
            return dynamic_cast<SteadyStateOperatorFormula&>(*this);
        }
        
        SteadyStateOperatorFormula const& Formula::asSteadyStateFormula() const {
            return dynamic_cast<SteadyStateOperatorFormula const&>(*this);
        }
        
        PathRewardFormula& Formula::asPathRewardFormula() {
            return dynamic_cast<PathRewardFormula&>(*this);
        }
        
        PathRewardFormula const& Formula::asPathRewardFormula() const {
            return dynamic_cast<PathRewardFormula const&>(*this);
        }
        
        CumulativeRewardFormula& Formula::asCumulativeRewardFormula() {
            return dynamic_cast<CumulativeRewardFormula&>(*this);
        }
        
        CumulativeRewardFormula const& Formula::asCumulativeRewardFormula() const {
            return dynamic_cast<CumulativeRewardFormula const&>(*this);
        }
        
        InstantaneousRewardFormula& Formula::asInstantaneousRewardFormula() {
            return dynamic_cast<InstantaneousRewardFormula&>(*this);
        }
        
        InstantaneousRewardFormula const& Formula::asInstantaneousRewardFormula() const {
            return dynamic_cast<InstantaneousRewardFormula const&>(*this);
        }
        
        ReachabilityRewardFormula& Formula::asReachabilityRewardFormula() {
            return dynamic_cast<ReachabilityRewardFormula&>(*this);
        }
        
        ReachabilityRewardFormula const& Formula::asReachabilityRewardFormula() const {
            return dynamic_cast<ReachabilityRewardFormula const&>(*this);
        }
        
        ProbabilityOperatorFormula& Formula::asProbabilityOperatorFormula() {
            return dynamic_cast<ProbabilityOperatorFormula&>(*this);
        }
        
        ProbabilityOperatorFormula const& Formula::asProbabilityOperatorFormula() const {
            return dynamic_cast<ProbabilityOperatorFormula const&>(*this);
        }
        
        RewardOperatorFormula& Formula::asRewardOperatorFormula() {
            return dynamic_cast<RewardOperatorFormula&>(*this);
        }
        
        RewardOperatorFormula const& Formula::asRewardOperatorFormula() const {
            return dynamic_cast<RewardOperatorFormula const&>(*this);
        }
        
        std::ostream& operator<<(std::ostream& out, Formula const& formula) {
            return formula.writeToStream(out);
        }
    }
}