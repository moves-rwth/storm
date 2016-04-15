#ifndef STORM_LOGIC_OPERATORFORMULA_H_
#define STORM_LOGIC_OPERATORFORMULA_H_

#include <boost/optional.hpp>

#include "src/logic/UnaryStateFormula.h"
#include "src/logic/Bound.h"
#include "src/solver/OptimizationDirection.h"

namespace storm {
    namespace logic {        
        struct OperatorInformation {
            OperatorInformation(boost::optional<storm::solver::OptimizationDirection> const& optimizationDirection = boost::none, boost::optional<Bound<double>> const& bound = boost::none);

            boost::optional<storm::solver::OptimizationDirection> optimalityType;
            boost::optional<Bound<double>> bound;
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
            double getThreshold() const;
            void setThreshold(double newThreshold);
            Bound<double> const& getBound() const;
            void setBound(Bound<double> const& newBound);
            
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