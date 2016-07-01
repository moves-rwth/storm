#include "src/storage/jani/Constant.h"

namespace storm {
    namespace jani {
        
        Constant::Constant(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> const& expression) : name(name), variable(variable), expression(expression) {
            // Intentionally left empty.
        }
        
        std::string const& Constant::getName() const {
            return name;
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
        
        storm::expressions::Variable const& Constant::getExpressionVariable() const {
            return variable;
        }
        
        storm::expressions::Expression const& Constant::getExpression() const {
            return expression.get();
        }
        
    }
}
