#include "storm/storage/jani/BooleanVariable.h"

namespace storm {
    namespace jani {
        
        BooleanVariable::BooleanVariable(std::string const& name, storm::expressions::Variable const& variable) : Variable(name, variable) {
            // Intentionally left empty.
        }
        
        BooleanVariable::BooleanVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient) : Variable(name, variable, initValue, transient) {
            // Intentionally left empty.
        }
        
        std::unique_ptr<Variable> BooleanVariable::clone() const {
            return std::make_unique<BooleanVariable>(*this);
        }
        
        bool BooleanVariable::isBooleanVariable() const {
            return true;
        }
        
        std::shared_ptr<BooleanVariable> makeBooleanVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient) {
            if (initValue) {
                return std::make_shared<BooleanVariable>(name, variable, initValue.get(), transient);
            } else {
                assert(!transient);
                return std::make_shared<BooleanVariable>(name, variable);
            }
        }
        
    }
}
