#include "storm/storage/jani/UnboundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        UnboundedIntegerVariable::UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient) : Variable(name, variable, initValue, transient) {
            // Intentionally left empty.
        }
        
        UnboundedIntegerVariable::UnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable) : Variable(name, variable) {
            // Intentionally left empty.
        }
        
        std::unique_ptr<Variable> UnboundedIntegerVariable::clone() const {
            return std::make_unique<UnboundedIntegerVariable>(*this);
        }
        
        bool UnboundedIntegerVariable::isUnboundedIntegerVariable() const {
            return true;
        }
        
        std::shared_ptr<UnboundedIntegerVariable> makeUnboundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient) {
            if (initValue) {
                return std::make_shared<UnboundedIntegerVariable>(name, variable, initValue.get(), transient);
            } else {
                assert(!transient);
                return std::make_shared<UnboundedIntegerVariable>(name, variable);
            }
        }
        
    }
}
