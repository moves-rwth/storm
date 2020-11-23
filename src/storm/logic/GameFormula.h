#ifndef STORM_LOGIC_GAMEFORMULA_H_
#define STORM_LOGIC_GAMEFORMULA_H_

#include "storm/logic/Formula.h"
#include "storm/logic/Coalition.h"
#include <memory>

namespace storm {
    namespace logic {
        class GameFormula : public Formula {
        public:
            GameFormula(Coalition coalition, std::shared_ptr<Formula const> const& subFormula);

            ~GameFormula() {
                // Intentionally left empty.
            }

            bool isGameFormula() const;

            Formula const& getSubformula() const;
            Coalition getCoalition() const;

            boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const;

            std::ostream& writeToStream(std::ostream& out) const;

        private:
            Coalition coalition;
            std::shared_ptr<Formula const> subformula;

        };
    }
}

#endif /* STORM_LOGIC_GAMEFORMULA_H_ */
