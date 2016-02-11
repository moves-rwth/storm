#ifndef STORM_LOGIC_OPERATORFORMULA_H_
#define STORM_LOGIC_OPERATORFORMULA_H_

#include <boost/optional.hpp>

#include "src/logic/UnaryStateFormula.h"
#include "src/solver/OptimizationDirection.h"
#include "src/logic/ComparisonType.h"

namespace storm {
    namespace logic {
        class OperatorFormula : public UnaryStateFormula {
        public:
            OperatorFormula(boost::optional<OptimizationDirection> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula);
            
            virtual ~OperatorFormula() {
                // Intentionally left empty.
            }
            
            bool hasBound() const;
            ComparisonType getComparisonType() const;
            void setComparisonType(ComparisonType);
            double getBound() const;
            void setBound(double);
            bool hasOptimalityType() const;
            OptimizationDirection const& getOptimalityType() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        protected:
            std::string operatorSymbol;
            boost::optional<ComparisonType> comparisonType;
            boost::optional<double> bound;
            boost::optional<OptimizationDirection> optimalityType;
        };
    }
}

#endif /* STORM_LOGIC_OPERATORFORMULA_H_ */