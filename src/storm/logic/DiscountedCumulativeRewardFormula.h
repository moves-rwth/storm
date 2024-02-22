#pragma once

#include "storm/logic/PathFormula.h"

#include "storm/logic/TimeBound.h"
#include "storm/logic/TimeBoundType.h"

namespace storm {
namespace logic {
class DiscountedCumulativeRewardFormula : public PathFormula {
   public:
    DiscountedCumulativeRewardFormula(storm::expressions::Expression const discountFactor, TimeBound const& bound,
                                      TimeBoundReference const& timeBoundReference = TimeBoundReference(TimeBoundType::Time),
                                      boost::optional<RewardAccumulation> rewardAccumulation = boost::none);
    DiscountedCumulativeRewardFormula(storm::expressions::Expression const discountFactor, std::vector<TimeBound> const& bounds,
                                      std::vector<TimeBoundReference> const& timeBoundReferences,
                                      boost::optional<RewardAccumulation> rewardAccumulation = boost::none);

    virtual ~DiscountedCumulativeRewardFormula() = default;

    virtual bool isDiscountedCumulativeRewardFormula() const override;
    virtual bool isCumulativeRewardFormula() const override;
    virtual bool isRewardPathFormula() const override;

    bool isMultiDimensional() const;
    unsigned getDimension() const;

    virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

    virtual void gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const override;
    virtual void gatherUsedVariables(std::set<storm::expressions::Variable>& usedVariables) const override;

    virtual std::ostream& writeToStream(std::ostream& out, bool allowParentheses = false) const override;

    TimeBoundReference const& getTimeBoundReference() const;
    TimeBoundReference const& getTimeBoundReference(unsigned i) const;

    bool isBoundStrict() const;
    bool isBoundStrict(unsigned i) const;
    bool hasIntegerBound() const;
    bool hasIntegerBound(unsigned i) const;

    storm::expressions::Expression const& getBound() const;
    storm::expressions::Expression const& getBound(unsigned i) const;

    storm::expressions::Expression const& getDiscountFactor() const;

    template<typename ValueType>
    ValueType getBound() const;

    template<typename ValueType>
    ValueType getBound(unsigned i) const;

    template<typename ValueType>
    ValueType getDiscountFactor() const;

    template<typename ValueType>
    ValueType getNonStrictBound() const;

    std::vector<TimeBound> const& getBounds() const;

    bool hasRewardAccumulation() const;
    RewardAccumulation const& getRewardAccumulation() const;
    std::shared_ptr<DiscountedCumulativeRewardFormula const> stripRewardAccumulation() const;

    std::shared_ptr<DiscountedCumulativeRewardFormula const> restrictToDimension(unsigned i) const;

   private:
    static void checkNoVariablesInBound(storm::expressions::Expression const& bound);
    static void checkNoVariablesInDiscountFactor(storm::expressions::Expression const& factor);

    storm::expressions::Expression const discountFactor;
    std::vector<TimeBoundReference> timeBoundReferences;
    std::vector<TimeBound> bounds;
    boost::optional<RewardAccumulation> rewardAccumulation;
};
}  // namespace logic
}  // namespace storm