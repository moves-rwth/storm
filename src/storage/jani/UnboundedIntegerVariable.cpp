#include "src/storage/jani/UnboundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        UnboundedIntegerVariable::UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue) : Variable(name, variable, initialValue) {
            // Intentionally left empty.
        }
        
    }
}