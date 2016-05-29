#include "src/storage/jani/Variable.h"

#include <iostream>

namespace storm {
    namespace jani {
        
        Variable::Variable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValue) : name(name), variable(variable), initialValue(initialValue) {
            // Intentionally left empty.
        }
        
        storm::expressions::Variable const& Variable::getExpressionVariable() const {
            return variable;
        }

        bool Variable::hasInitialValue() const {
            return initialValue.isInitialized();
        }
        
        std::string const& Variable::getName() const {
            return name;
        }
        
        storm::expressions::Expression const& Variable::getInitialValue() const {
            return initialValue;
        }
        
        void Variable::setInitialValue(storm::expressions::Expression const& initialValue) {
            this->initialValue = initialValue;
        }
        
    }
}