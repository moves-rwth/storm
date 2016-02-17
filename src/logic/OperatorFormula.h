#ifndef STORM_LOGIC_OPERATORFORMULA_H_
#define STORM_LOGIC_OPERATORFORMULA_H_

#include <boost/optional.hpp>

#include "src/logic/UnaryStateFormula.h"
#include "src/logic/Bound.h"
#include "src/solver/OptimizationDirection.h"

namespace storm {
    namespace logic {
        class OperatorFormula : public UnaryStateFormula {
        public:
            OperatorFormula(boost::optional<storm::solver::OptimizationDirection> optimalityType, boost::optional<Bound<double>> bound, std::shared_ptr<Formula const> const& subformula);
            
            virtual ~OperatorFormula() {
                // Intentionally left empty.
            }
            
            bool hasBound() const;
            ComparisonType getComparisonType() const;
            void setComparisonType(ComparisonType newComparisonType);
            double getThreshold() const;
            void setThreshold(double newThreshold);
            Bound<double> const& getBound() const;
            void setBound(Bound<double> const& newBound);
            bool hasOptimalityType() const;
            storm::solver::OptimizationDirection const& getOptimalityType() const;
            virtual bool isOperatorFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        protected:
            std::string operatorSymbol;
            boost::optional<Bound<double>> bound;
            boost::optional<storm::solver::OptimizationDirection> optimalityType;
        };
    }
}

#endif /* STORM_LOGIC_OPERATORFORMULA_H_ */