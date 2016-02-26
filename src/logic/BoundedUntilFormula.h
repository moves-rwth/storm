#ifndef STORM_LOGIC_BOUNDEDUNTILFORMULA_H_
#define STORM_LOGIC_BOUNDEDUNTILFORMULA_H_

#include <boost/variant.hpp>

#include "src/logic/BinaryPathFormula.h"

namespace storm {
    namespace logic {
        class BoundedUntilFormula : public BinaryPathFormula {
        public:
            BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, double lowerBound, double upperBound);
            BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, uint_fast64_t upperBound);
            BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, boost::variant<uint_fast64_t, std::pair<double, double>> const& bounds);
            
            virtual bool isBoundedUntilFormula() const override;

            virtual bool isProbabilityPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            bool hasDiscreteTimeBound() const;
            
            std::pair<double, double> const& getIntervalBounds() const;
            uint_fast64_t getDiscreteTimeBound() const;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
        private:
            boost::variant<uint_fast64_t, std::pair<double, double>> bounds;
        };
    }
}

#endif /* STORM_LOGIC_BOUNDEDUNTILFORMULA_H_ */