#ifndef STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_
#define STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_

#include "storm/logic/PathFormula.h"

#include "storm/logic/TimeBound.h"
#include "storm/logic/TimeBoundType.h"

namespace storm {
    namespace logic {
        class CumulativeRewardFormula : public PathFormula {
        public:
            CumulativeRewardFormula(TimeBound const& bound, TimeBoundType const& timeBoundType = TimeBoundType::Time);
            
            virtual ~CumulativeRewardFormula() {
                // Intentionally left empty.
            }

            virtual bool isCumulativeRewardFormula() const override;
            virtual bool isRewardPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            TimeBoundType const& getTimeBoundType() const;
            bool isStepBounded() const;
            bool isTimeBounded() const;
            
            bool isBoundStrict() const;
            bool hasIntegerBound() const;
            
            storm::expressions::Expression const& getBound() const;
            
            template <typename ValueType>
            ValueType getBound() const;
            
            template <typename ValueType>
            ValueType getNonStrictBound() const;
            
        private:
            static void checkNoVariablesInBound(storm::expressions::Expression const& bound);

            TimeBoundType timeBoundType;
            TimeBound bound;
        };
    }
}

#endif /* STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_ */
