#pragma once

#include <boost/optional.hpp>

#include "storm/logic/PathFormula.h"
#include "storm/logic/RewardAccumulation.h"
#include "storm/storage/expressions/Expression.h"

namespace storm {
namespace logic {
class DiscountedTotalRewardFormula : public PathFormula {
   public:
    DiscountedTotalRewardFormula(storm::expressions::Expression const discountFactor, boost::optional<RewardAccumulation> rewardAccumulation = boost::none);

    virtual ~DiscountedTotalRewardFormula() {
        // Intentionally left empty.
    }

    virtual bool isDiscountedTotalRewardFormula() const override;
    virtual bool isTotalRewardFormula() const override;

    virtual bool isRewardPathFormula() const override;
    bool hasRewardAccumulation() const;
    RewardAccumulation const& getRewardAccumulation() const;
    std::shared_ptr<DiscountedTotalRewardFormula const> stripRewardAccumulation() const;

    virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

    virtual std::ostream& writeToStream(std::ostream& out, bool allowParentheses = false) const override;

    storm::expressions::Expression const& getDiscountFactor() const;

    template<typename ValueType>
    ValueType getDiscountFactor() const;

   private:
    static void checkNoVariablesInDiscountFactor(storm::expressions::Expression const& factor);

    storm::expressions::Expression const discountFactor;
    boost::optional<RewardAccumulation> rewardAccumulation;
};
}  // namespace logic
}  // namespace storm
