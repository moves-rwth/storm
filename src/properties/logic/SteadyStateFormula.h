#ifndef STORM_LOGIC_STEADYSTATEFORMULA_H_
#define STORM_LOGIC_STEADYSTATEFORMULA_H_

#include "src/properties/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class SteadyStateFormula : public OperatorFormula {
        public:
            SteadyStateFormula(std::shared_ptr<Formula> const& subformula);
            SteadyStateFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula> const& subformula);
            SteadyStateFormula(ComparisonType comparisonType, double bound, OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula);
            SteadyStateFormula(OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula);
            
            virtual ~SteadyStateFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isSteadyStateFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            SteadyStateFormula(boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, boost::optional<OptimalityType> optimalityType, std::shared_ptr<Formula> const& subformula);
        };
    }
}

#endif /* STORM_LOGIC_STEADYSTATEFORMULA_H_ */