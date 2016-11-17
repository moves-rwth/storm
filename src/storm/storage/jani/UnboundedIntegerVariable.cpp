#include "src/storm/storage/jani/UnboundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        UnboundedIntegerVariable::UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient) : Variable(name, variable, initValue, transient) {
            // Intentionally left empty.
        }
        
        UnboundedIntegerVariable::UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, bool transient) : Variable(name, variable) {
            // Intentionally left empty.
        }
        
        bool UnboundedIntegerVariable::isUnboundedIntegerVariable() const {
            return true;
        }
        
    }
}
