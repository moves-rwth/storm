#include "src/storage/prism/Constant.h"

namespace storm {
    namespace prism {
        Constant::Constant(storm::expressions::ExpressionReturnType constantType, std::string const& constantName, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), constantType(constantType), constantName(constantName), defined(true), expression(expression) {
            // Intentionally left empty.
        }
        
        Constant::Constant(storm::expressions::ExpressionReturnType constantType, std::string const& constantName, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), constantType(constantType), constantName(constantName), defined(false), expression() {
            // Intentionally left empty.
        }
        
        std::string const& Constant::getConstantName() const {
            return this->constantName;
        }
        
        storm::expressions::ExpressionReturnType Constant::getConstantType() const {
            return this->constantType;
        }
        
        bool Constant::isDefined() const {
            return this->defined;
        }
        
        storm::expressions::Expression const& Constant::getExpression() const {
            return this->expression;
        }
        
        Constant Constant::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return Constant(this->getConstantType(), this->getConstantName(), this->getExpression().substitute<std::map>(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, Constant const& constant) {
            stream << "const ";
            switch (constant.getConstantType()) {
                case storm::expressions::ExpressionReturnType::Undefined: stream << "undefined "; break;
                case storm::expressions::ExpressionReturnType::Bool: stream << "bool "; break;
                case storm::expressions::ExpressionReturnType::Int: stream << "int "; break;
                case storm::expressions::ExpressionReturnType::Double: stream << "double "; break;
            }
            stream << constant.getConstantName();
            if (constant.isDefined()) {
                stream << " = " << constant.getExpression();
            }
            stream << ";";
            return stream;
        }
    }
}