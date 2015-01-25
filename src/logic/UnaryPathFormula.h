#ifndef STORM_LOGIC_UNARYPATHFORMULA_H_
#define STORM_LOGIC_UNARYPATHFORMULA_H_

#include <memory>

#include "src/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class UnaryPathFormula : public PathFormula {
        public:
            UnaryPathFormula(std::shared_ptr<Formula const> const& subformula);
            
            virtual ~UnaryPathFormula() {
                // Intentionally left empty.
            }

            virtual bool isUnaryPathFormula() const override;
            
            virtual bool isPctlPathFormula() const override;
            virtual bool isLtlFormula() const override;
            virtual bool hasProbabilityOperator() const override;
            virtual bool hasNestedProbabilityOperators() const override;
            
            Formula const& getSubformula() const;
            
        private:
            std::shared_ptr<Formula const> subformula;
        };
    }
}

#endif /* STORM_LOGIC_UNARYPATHFORMULA_H_ */