#include "src/storage/jani/Variable.h"

namespace storm {
    namespace jani {
        
        Variable::Variable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue) : name(name), variable(variable), initialValue(initialValue) {
            // Intentionally left empty.
        }
        
        storm::expressions::Variable const& Variable::getExpressionVariable() const {
            return variable;
        }

        std::string const& Variable::getName() const {
            return name;
        }
        
    }
}