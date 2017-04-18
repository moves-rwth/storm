#ifndef STORM_LOGIC_OPERATORFORMULA_H_
#define STORM_LOGIC_OPERATORFORMULA_H_

#include <boost/optional.hpp>

#include "storm/logic/UnaryStateFormula.h"
#include "storm/logic/Bound.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/storage/expressions/Expression.h"

#include "storm/utility/constants.h"

namespace storm {
    namespace logic {

        struct OperatorInformation {
            OperatorInformation(boost::optional<storm::solver::OptimizationDirection> const& optimizationDirection = boost::none, boost::optional<Bound> const& bound = boost::none);

            boost::optional<storm::solver::OptimizationDirection> optimalityType;
            boost::optional<Bound> bound;
        };
        
        class OperatorFormula : public UnaryStateFormula {
        public:
            OperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation = OperatorInformation());
            
            virtual ~OperatorFormula() {
                // Intentionally left empty.
            }

            // Bound-related accessors.
            bool hasBound() const;
            ComparisonType getComparisonType() const;
            void setComparisonType(ComparisonType newComparisonType);
            storm::expressions::Expression const& getThreshold() const;
            template <typename ValueType>
            ValueType getThresholdAs() const;
            void setThreshold(storm::expressions::Expression const& newThreshold);
            Bound const& getBound() const;
            void setBound(Bound const& newBound);
            
            // Optimality-type-related accessors.
            bool hasOptimalityType() const;
            storm::solver::OptimizationDirection const& getOptimalityType() const;
            virtual bool isOperatorFormula() const override;
            
            OperatorInformation const& getOperatorInformation() const;
            
            virtual bool hasQualitativeResult() const override;
            virtual bool hasQuantitativeResult() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        protected:
            OperatorInformation operatorInformation;
        };
    }
}

#endif /* STORM_LOGIC_OPERATORFORMULA_H_ */
