#ifndef STORM_LOGIC_OPERATORFORMULA_H_
#define STORM_LOGIC_OPERATORFORMULA_H_

#include <boost/optional.hpp>

#include "src/properties/logic/UnaryStateFormula.h"
#include "src/properties/logic/OptimalityType.h"
#include "src/properties/logic/ComparisonType.h"

namespace storm {
    namespace logic {
        class OperatorFormula : public UnaryStateFormula {
        public:
            OperatorFormula(boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, boost::optional<OptimalityType> optimalityType, std::shared_ptr<Formula> const& subformula);
            
            virtual ~OperatorFormula() {
                // Intentionally left empty.
            }
            
            bool hasBound() const;
            ComparisonType const& getComparisonType() const;
            double getBound() const;
            bool hasOptimalityType() const;
            OptimalityType const& getOptimalityType() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            std::string operatorSymbol;
            boost::optional<ComparisonType> comparisonType;
            boost::optional<double> bound;
            boost::optional<OptimalityType> optimalityType;
        };
    }
}

#endif /* STORM_LOGIC_OPERATORFORMULA_H_ */