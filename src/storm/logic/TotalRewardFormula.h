#ifndef STORM_LOGIC_TOTALREWARDFORMULA_H_
#define STORM_LOGIC_TOTALREWARDFORMULA_H_

#include <boost/variant.hpp>

#include "src/storm/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class TotalRewardFormula : public PathFormula {
        public:
            TotalRewardFormula();
            
            virtual ~TotalRewardFormula() {
                // Intentionally left empty.
            }

            virtual bool isTotalRewardFormula() const override;
            virtual bool isRewardPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        };
    }
}

#endif /* STORM_LOGIC_TOTALREWARDFORMULA_H_ */
