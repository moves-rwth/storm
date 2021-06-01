#ifndef STORM_LOGIC_GAMEFORMULA_H_
#define STORM_LOGIC_GAMEFORMULA_H_

#include "storm/logic/UnaryStateFormula.h"
#include "storm/logic/PlayerCoalition.h"
#include <memory>

namespace storm {
    namespace logic {
        class GameFormula : public UnaryStateFormula {
        public:
            GameFormula(PlayerCoalition const& coalition, std::shared_ptr<Formula const> subFormula);

            virtual ~GameFormula() {
                // Intentionally left empty.
            }

            PlayerCoalition const& getCoalition() const;
            virtual bool isGameFormula() const override;
            virtual bool hasQualitativeResult() const override;
            virtual bool hasQuantitativeResult() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;

        private:
            PlayerCoalition coalition;
        };
    }
}

#endif /* STORM_LOGIC_GAMEFORMULA_H_ */
