#include "src/storage/prism/Constant.h"

namespace storm {
    namespace prism {
        Constant::Constant(ConstantType constantType, std::string const& constantName, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), constantType(constantType), constantName(constantName), defined(true), expression(expression) {
            // Intentionally left empty.
        }
        
        Constant::Constant(ConstantType constantType, std::string const& constantName, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), constantType(constantType), constantName(constantName), defined(false), expression() {
            // Intentionally left empty.
        }
        
        std::string const& Constant::getConstantName() const {
            return this->constantName;
        }
        
        Constant::ConstantType Constant::getConstantType() const {
            return this->constantType;
        }
        
        bool Constant::isDefined() const {
            return this->defined;
        }
        
        storm::expressions::Expression const& Constant::getExpression() const {
            return this->expression;
        }
        
        std::ostream& operator<<(std::ostream& stream, Constant const& constant) {
            stream << "const ";
            switch (constant.getConstantType()) {
                case Constant::ConstantType::Bool: stream << "bool "; break;
                case Constant::ConstantType::Integer: stream << "int "; break;
                case Constant::ConstantType::Double: stream << "double "; break;
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