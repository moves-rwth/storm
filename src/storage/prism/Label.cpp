#include "src/storage/prism/Label.h"

namespace storm {
    namespace prism {
        Label::Label(std::string const& labelName, storm::expressions::Expression const& statePredicateExpression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), labelName(labelName), statePredicateExpression(statePredicateExpression) {
            // Intentionally left empty.
        }

        std::string const& Label::getLabelName() const {
            return this->labelName;
        }
        
        storm::expressions::Expression const& Label::getStatePredicateExpression() const {
            return this->statePredicateExpression;
        }
        
        Label Label::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return Label(this->getLabelName(), this->getStatePredicateExpression().substitute<std::map>(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, Label const& label) {
            stream << "label \"" << label.getLabelName() << "\" = " << label.getStatePredicateExpression() << ";";
            return stream;
        }
    }
}