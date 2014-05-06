#include "src/storage/expressions/ConstantExpression.h"

namespace storm {
    namespace expressions {
        ConstantExpression::ConstantExpression(ExpressionReturnType returnType, std::string const& constantName) : BaseExpression(returnType), constantName(constantName) {
            // Intentionally left empty.
        }
        
        std::set<std::string> ConstantExpression::getVariables() const {
            return std::set<std::string>();
        }
        
        std::set<std::string> ConstantExpression::getConstants() const {
            return {this->getConstantName()};
        }
        
        std::string const& ConstantExpression::getConstantName() const {
            return this->constantName;
        }
        
        bool ConstantExpression::isConstant() const {
            return true;
        }
        
        bool ConstantExpression::hasConstantValue() const {
            return false;
        }
        
        std::string const& ConstantExpression::getIdentifier() const {
            return this->getConstantName();
        }
        
        void ConstantExpression::printToStream(std::ostream& stream) const {
            stream << this->getConstantName();
        }
    }
}