#include "src/storage/prism/InitialConstruct.h"

namespace storm {
    namespace prism {
        InitialConstruct::InitialConstruct(storm::expressions::Expression initialStatesExpression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), initialStatesExpression(initialStatesExpression) {
            // Intentionally left empty.
        }
        
        storm::expressions::Expression InitialConstruct::getInitialStatesExpression() const {
            return this->initialStatesExpression;
        }
        
        InitialConstruct InitialConstruct::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return InitialConstruct(this->getInitialStatesExpression().substitute(substitution));
        }
        
        std::ostream& operator<<(std::ostream& stream, InitialConstruct const& initialConstruct) {
            stream << "initial " << std::endl;
            stream << "\t" << initialConstruct.getInitialStatesExpression() << std::endl;
            stream << "endinitial" << std::endl;
            return stream;
        }
    } // namespace prism
} // namespace storm