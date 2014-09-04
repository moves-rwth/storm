#include "src/storage/prism/Constant.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace prism {
        Constant::Constant(storm::expressions::ExpressionReturnType type, std::string const& name, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), type(type), name(name), defined(true), expression(expression) {
            // Intentionally left empty.
        }
        
        Constant::Constant(storm::expressions::ExpressionReturnType type, std::string const& name, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), type(type), name(name), defined(false), expression() {
            // Intentionally left empty.
        }
        
        std::string const& Constant::getName() const {
            return this->name;
        }
        
        storm::expressions::ExpressionReturnType Constant::getType() const {
            return this->type;
        }
        
        bool Constant::isDefined() const {
            return this->defined;
        }
        
        storm::expressions::Expression const& Constant::getExpression() const {
            assert(this->isDefined());
            LOG_THROW(this->isDefined(), storm::exceptions::IllegalFunctionCallException, "Unable to retrieve defining expression for undefined constant: " + this->getName());
            return this->expression;
        }
        
        Constant Constant::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return Constant(this->getType(), this->getName(), this->getExpression().substitute(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, Constant const& constant) {
            stream << "const ";
            switch (constant.getType()) {
                case storm::expressions::ExpressionReturnType::Undefined: stream << "undefined "; break;
                case storm::expressions::ExpressionReturnType::Bool: stream << "bool "; break;
                case storm::expressions::ExpressionReturnType::Int: stream << "int "; break;
                case storm::expressions::ExpressionReturnType::Double: stream << "double "; break;
            }
            stream << constant.getName();
            if (constant.isDefined()) {
                stream << " = " << constant.getExpression();
            }
            stream << ";";
            return stream;
        }
    } // namespace prism
} // namespace storm