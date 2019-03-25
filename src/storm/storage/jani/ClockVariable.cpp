#include "storm/storage/jani/ClockVariable.h"

namespace storm {
    namespace jani {
        
        ClockVariable::ClockVariable(std::string const& name, storm::expressions::Variable const& variable) : Variable(name, variable) {
            // Intentionally left empty.
        }
        
        ClockVariable::ClockVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient) : Variable(name, variable, initValue, transient) {
            // Intentionally left empty.
        }
        
        std::unique_ptr<Variable> ClockVariable::clone() const {
            return std::make_unique<ClockVariable>(*this);
        }
        
        bool ClockVariable::isClockVariable() const {
            return true;
        }
        
        std::shared_ptr<ClockVariable> makeClockVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient) {
            if (initValue) {
                return std::make_shared<ClockVariable>(name, variable, initValue.get(), transient);
            } else {
                assert(!transient);
                return std::make_shared<ClockVariable>(name, variable);
            }
        }
        
    }
}
