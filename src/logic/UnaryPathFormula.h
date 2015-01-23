#ifndef STORM_LOGIC_UNARYPATHFORMULA_H_
#define STORM_LOGIC_UNARYPATHFORMULA_H_

#include <memory>

#include "src/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class UnaryPathFormula : public PathFormula {
        public:
            UnaryPathFormula(std::shared_ptr<Formula> const& subformula);
            
            virtual ~UnaryPathFormula() {
                // Intentionally left empty.
            }

            virtual bool isUnaryPathFormula() const override;
            
            Formula& getSubformula();
            Formula const& getSubformula() const;
            
        private:
            std::shared_ptr<Formula> subformula;
        };
    }
}

#endif /* STORM_LOGIC_UNARYPATHFORMULA_H_ */