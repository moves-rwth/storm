#include "src/storm/storage/jani/RealVariable.h"

namespace storm {
    namespace jani {
        
        RealVariable::RealVariable(std::string const& name, storm::expressions::Variable const& variable) : storm::jani::Variable(name, variable) {
            // Intentionally left empty.
        }
        
        RealVariable::RealVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient) : storm::jani::Variable(name, variable, initValue, transient) {
            // Intentionally left empty.
        }
        
        bool RealVariable::isRealVariable() const {
            return true;
        }
        
    }
}
