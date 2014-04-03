#include "src/storage/expressions/ConstantExpression.h"

namespace storm {
    namespace expressions {
        ConstantExpression::ConstantExpression(ExpressionReturnType returnType, std::string const& constantName) : BaseExpression(returnType), constantName(constantName) {
            // Intentionally left empty.
        }
        
        ConstantExpression::ConstantExpression(ConstantExpression const& other) : BaseExpression(other), constantName(other.getConstantName()) {
            // Intentionally left empty.
        }
        
        ConstantExpression& ConstantExpression::operator=(ConstantExpression const& other) {
            if (this != &other) {
                BaseExpression::operator=(other);
                this->constantName = other.getConstantName();
            }
            return *this;
        }
        
        bool ConstantExpression::isConstant() const {
            return false;
        }
        
        bool ConstantExpression::isTrue() const {
            return false;
        }
        
        bool ConstantExpression::isFalse() const {
            return false;
        }
        
        std::set<std::string> ConstantExpression::getVariables() const {
            return std::set<std::string>();
        }
        
        std::set<std::string> ConstantExpression::getConstants() const {
            return {this->getConstantName()};
        }
        
        std::unique_ptr<BaseExpression> ConstantExpression::simplify() const {
            return this->clone();
        }
        
        std::string const& ConstantExpression::getConstantName() const {
            return this->constantName;
        }
    }
}