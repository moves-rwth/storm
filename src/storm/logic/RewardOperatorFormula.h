#pragma once

#include "storm/logic/OperatorFormula.h"

namespace storm {
namespace logic {
class RewardOperatorFormula : public OperatorFormula {
   public:
    RewardOperatorFormula(std::shared_ptr<Formula const> const& subformula, boost::optional<std::string> const& rewardModelName = boost::none,
                          OperatorInformation const& operatorInformation = OperatorInformation());

    virtual ~RewardOperatorFormula() {
        // Intentionally left empty.
    }

    virtual bool isRewardOperatorFormula() const override;

    virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

    virtual void gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const override;

    virtual std::ostream& writeToStream(std::ostream& out, bool allowParentheses = false) const override;

    /*!
     * Retrieves whether the reward model refers to a specific reward model.
     *
     * @return True iff the reward model refers to a specific reward model.
     */
    bool hasRewardModelName() const;

    /*!
     * Retrieves the optional representing the reward model name this property refers to.
     *
     * @return The reward model name this property refers to (if any).
     */
    boost::optional<std::string> const& getOptionalRewardModelName() const;

    /*!
     * Retrieves the name of the reward model this property refers to (if any). Note that it is illegal to call
     * this function if the operator does not have a specific reward model it is referring to.
     *
     * @return The name of the reward model this propertye refers to.
     */
    std::string const& getRewardModelName() const;

   private:
    // The (optional) name of the reward model this property refers to.
    boost::optional<std::string> rewardModelName;
};
}  // namespace logic
}  // namespace storm
