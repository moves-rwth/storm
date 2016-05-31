#include "src/storage/jani/Variable.h"

#include "src/storage/jani/BooleanVariable.h"
#include "src/storage/jani/BoundedIntegerVariable.h"
#include "src/storage/jani/UnboundedIntegerVariable.h"

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
        
        bool Variable::isBooleanVariable() const {
            return false;
        }
        
        bool Variable::isBoundedIntegerVariable() const {
            return false;
        }
        
        bool Variable::isUnboundedIntegerVariable() const {
            return false;
        }
        
        BooleanVariable& Variable::asBooleanVariable() {
            return dynamic_cast<BooleanVariable&>(*this);
        }
        
        BooleanVariable const& Variable::asBooleanVariable() const {
            return dynamic_cast<BooleanVariable const&>(*this);
        }
        
        BoundedIntegerVariable& Variable::asBoundedIntegerVariable() {
            return dynamic_cast<BoundedIntegerVariable&>(*this);
        }
        
        BoundedIntegerVariable const& Variable::asBoundedIntegerVariable() const {
            return dynamic_cast<BoundedIntegerVariable const&>(*this);
        }
        
        UnboundedIntegerVariable& Variable::asUnboundedIntegerVariable() {
            return dynamic_cast<UnboundedIntegerVariable&>(*this);
        }
        
        UnboundedIntegerVariable const& Variable::asUnboundedIntegerVariable() const {
            return dynamic_cast<UnboundedIntegerVariable const&>(*this);
        }
        
    }
}