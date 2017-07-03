#ifndef STORM_LOGIC_BOUNDEDUNTILFORMULA_H_
#define STORM_LOGIC_BOUNDEDUNTILFORMULA_H_

#include <boost/optional.hpp>

#include "storm/logic/BinaryPathFormula.h"

#include "storm/logic/TimeBound.h"
#include "storm/logic/TimeBoundType.h"

namespace storm {
    namespace logic {
        class BoundedUntilFormula : public BinaryPathFormula {
        public:
            BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, boost::optional<TimeBound> const& lowerBound, boost::optional<TimeBound> const& upperBound, TimeBoundReference const& timeBoundReference);
            
            virtual bool isBoundedUntilFormula() const override;

            virtual bool isProbabilityPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            TimeBoundReference const& getTimeBoundReference() const;

            
            bool isLowerBoundStrict() const;
            bool hasLowerBound() const;
            bool hasIntegerLowerBound() const;

            bool isUpperBoundStrict() const;
            bool hasUpperBound() const;
            bool hasIntegerUpperBound() const;
            
            storm::expressions::Expression const& getLowerBound() const;
            storm::expressions::Expression const& getUpperBound() const;
            
            template <typename ValueType>
            ValueType getLowerBound() const;

            template <typename ValueType>
            ValueType getUpperBound() const;
            
            template <typename ValueType>
            ValueType getNonStrictUpperBound() const;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            static void checkNoVariablesInBound(storm::expressions::Expression const& bound);
            
            TimeBoundReference timeBoundReference;
            boost::optional<TimeBound> lowerBound;
            boost::optional<TimeBound> upperBound;
        };
    }
}

#endif /* STORM_LOGIC_BOUNDEDUNTILFORMULA_H_ */
