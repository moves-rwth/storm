#include "storm/logic/GameFormula.h"

#include "storm/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        GameFormula::GameFormula(Coalition coalition, std::shared_ptr<Formula const> subFormula) : coalition(coalition), subformula(subformula) {
            STORM_PRINT_AND_LOG("CTOR subf usecount:" << subformula.use_count() << std::endl);
            // Intentionally left empty.
        }

        bool GameFormula::isGameFormula() const {
            return true;
        }

        Formula const& GameFormula::getSubformula() const {
            return *subformula;
        }

        Coalition GameFormula::getCoalition() const {
            return coalition;
        }

        boost::any GameFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }

        std::ostream& GameFormula::writeToStream(std::ostream& out) const {
            out << coalition;
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}
