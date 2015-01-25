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
            
            virtual bool isBoundedUntilFormula() const override;

            bool isIntervalBounded() const;
            bool isIntegerUpperBounded() const;
            
            std::pair<double, double> const& getIntervalBounds() const;
            uint_fast64_t getUpperBound() const;
            
            virtual bool isPctlPathFormula() const override;
            virtual bool isCslPathFormula() const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            boost::variant<uint_fast64_t, std::pair<double, double>> bounds;
        };
    }
}

#endif /* STORM_LOGIC_BOUNDEDUNTILFORMULA_H_ */