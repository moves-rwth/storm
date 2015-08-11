#ifndef STORM_LOGIC_REWARDOPERATORFORMULA_H_
#define STORM_LOGIC_REWARDOPERATORFORMULA_H_

#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class RewardOperatorFormula : public OperatorFormula {
        public:
            RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, std::shared_ptr<Formula const> const& subformula);
            RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula);
            RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula);

            virtual ~RewardOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isRewardOperatorFormula() const override;

            virtual bool isPctlStateFormula() const override;
            virtual bool containsRewardOperator() const override;
            virtual bool containsNestedRewardOperators() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
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
    }
}

#endif /* STORM_LOGIC_REWARDOPERATORFORMULA_H_ */