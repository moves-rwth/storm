#ifndef STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_
#define STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_

#include <boost/variant.hpp>

#include "src/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class InstantaneousRewardFormula : public PathFormula {
        public:
            InstantaneousRewardFormula(uint_fast64_t timeBound);
            
            InstantaneousRewardFormula(double timeBound);
            
            virtual ~InstantaneousRewardFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isInstantaneousRewardFormula() const override;

            virtual bool isRewardPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            bool hasDiscreteTimeBound() const;
            
            uint_fast64_t getDiscreteTimeBound() const;

            bool hasContinuousTimeBound() const;
            
            double getContinuousTimeBound() const;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
        private:
            boost::variant<uint_fast64_t, double> timeBound;
        };
    }
}

#endif /* STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_ */