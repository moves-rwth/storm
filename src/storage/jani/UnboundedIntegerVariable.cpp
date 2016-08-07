#include "src/storage/jani/UnboundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        UnboundedIntegerVariable::UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, bool transient) : Variable(name, variable, transient) {
            // Intentionally left empty.
        }
        
        bool UnboundedIntegerVariable::isUnboundedIntegerVariable() const {
            return true;
        }
        
    }
}