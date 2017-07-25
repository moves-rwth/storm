#pragma once

#include <boost/optional.hpp>

#include "storm/logic/BinaryPathFormula.h"

#include "storm/logic/TimeBound.h"
#include "storm/logic/TimeBoundType.h"

namespace storm {
    namespace logic {
        class BoundedUntilFormula : public BinaryPathFormula {
        public:
            BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, boost::optional<TimeBound> const& lowerBound, boost::optional<TimeBound> const& upperBound, TimeBoundReference const& timeBoundReference);
            BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, std::vector<boost::optional<TimeBound>> const& lowerBounds, std::vector<boost::optional<TimeBound>> const& upperBounds, std::vector<TimeBoundReference> const& timeBoundReferences);

            virtual bool isBoundedUntilFormula() const override;

            virtual bool isProbabilityPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            virtual void gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const override;
            
            TimeBoundReference const& getTimeBoundReference(unsigned i = 0) const;

            
            bool isLowerBoundStrict(unsigned i = 0) const;
            bool hasLowerBound() const;
            bool hasLowerBound(unsigned i) const;
            bool hasIntegerLowerBound(unsigned i = 0) const;

            bool isUpperBoundStrict(unsigned i = 0) const;
            bool hasUpperBound() const;
            bool hasUpperBound(unsigned i) const;
            bool hasIntegerUpperBound(unsigned i = 0) const;

            bool isMultiDimensional() const;
            unsigned getDimension() const;
            
            storm::expressions::Expression const& getLowerBound(unsigned i = 0) const;
            storm::expressions::Expression const& getUpperBound(unsigned i = 0) const;
            
            template <typename ValueType>
            ValueType getLowerBound(unsigned i = 0) const;

            template <typename ValueType>
            ValueType getUpperBound(unsigned i = 0) const;
            
            template <typename ValueType>
            ValueType getNonStrictUpperBound(unsigned i = 0) const;

            std::shared_ptr<BoundedUntilFormula const> restrictToDimension(unsigned i) const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            static void checkNoVariablesInBound(storm::expressions::Expression const& bound);
            
            std::vector<TimeBoundReference> timeBoundReference;
            std::vector<boost::optional<TimeBound>> lowerBound;
            std::vector<boost::optional<TimeBound>> upperBound;
        };
    }
}

