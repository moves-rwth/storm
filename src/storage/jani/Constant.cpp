#include "src/storage/jani/Constant.h"

namespace storm {
    namespace jani {
        
        Constant::Constant(std::string const& name, boost::optional<storm::expressions::Expression> const& expression) : name(name), expression(expression) {
            // Intentionally left empty.
        }
        
        bool Constant::isDefined() const {
            return static_cast<bool>(expression);
        }
        
        void Constant::define(storm::expressions::Expression const& expression) {
            this->expression = expression;
        }
        
        storm::expressions::Type const& Constant::getType() const {
            return variable.getType();
        }
        
        bool Constant::isBooleanConstant() const {
            return getType().isBooleanType();
        }
        
        bool Constant::isIntegerConstant() const {
            return getType().isIntegerType();
        }
        
        bool Constant::isRealConstant() const {
            return getType().isRationalType();
        }
        
    }
}
