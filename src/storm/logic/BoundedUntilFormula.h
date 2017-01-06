#ifndef STORM_LOGIC_BOUNDEDUNTILFORMULA_H_
#define STORM_LOGIC_BOUNDEDUNTILFORMULA_H_

#include <boost/optional.hpp>

#include "storm/logic/BinaryPathFormula.h"

namespace storm {
    namespace logic {
        class UntilBound {
        public:
            UntilBound(bool strict, storm::expressions::Expression bound);
            
            storm::expressions::Expression const& getBound() const;
            bool isStrict() const;
            
        private:
            bool strict;
            storm::expressions::Expression bound;
        };
        
        class BoundedUntilFormula : public BinaryPathFormula {
        public:
            enum class BoundedType {
                Steps,
                Time
            };
            
            BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, boost::optional<UntilBound> const& lowerBound, boost::optional<UntilBound> const& upperBound, BoundedType const& boundedType = BoundedType::Time);
            
            virtual bool isBoundedUntilFormula() const override;

            virtual bool isProbabilityPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            bool isStepBounded() const;
            bool isTimeBounded() const;
            
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

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            BoundedType boundedType;
            
            boost::optional<UntilBound> lowerBound;
            boost::optional<UntilBound> upperBound;
        };
    }
}

#endif /* STORM_LOGIC_BOUNDEDUNTILFORMULA_H_ */
