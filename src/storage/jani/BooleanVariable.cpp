#include "src/storage/jani/BooleanVariable.h"

namespace storm {
    namespace jani {
        
        BooleanVariable::BooleanVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue) : Variable(name, variable, initialValue) {
            // Intentionally left empty.
        }
        
        bool BooleanVariable::isBooleanVariable() const {
            return true;
        }
        
    }
}