#include "src/storage/jani/Variable.h"

#include "src/storage/jani/BooleanVariable.h"
#include "src/storage/jani/BoundedIntegerVariable.h"
#include "src/storage/jani/UnboundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        Variable::Variable(std::string const& name, storm::expressions::Variable const& variable) : name(name), variable(variable) {
            // Intentionally left empty.
        }
        
        storm::expressions::Variable const& Variable::getExpressionVariable() const {
            return variable;
        }

        std::string const& Variable::getName() const {
            return name;
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