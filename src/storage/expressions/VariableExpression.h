#ifndef STORM_STORAGE_EXPRESSIONS_VARIABLEEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_VARIABLEEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        class VariableExpression : public BaseExpression {
            VariableExpression(ExpressionReturnType returnType, std::string const& variableName);
            virtual ~VariableExpression() = default;
            
            std::string const& getVariableName() const;
            
            virtual int_fast64_t evaluateAsInt(Valuation const& evaluation) const;
            virtual bool evaluateAsBool(Valuation const& evaluation) const;
            virtual double evaluateAsDouble(Valuation const& evaluation) const;
            
            virtual std::unique_ptr<BaseExpression> operator+(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> operator-(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> operator-() const;
            virtual std::unique_ptr<BaseExpression> operator*(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> operator/(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> operator&(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> operator|(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> operator~() const;
            
            virtual std::unique_ptr<BaseExpression> equals(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> notEquals(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> greater(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> greaterOrEqual(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> less(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> lessOrEqual(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> minimum(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> maximum(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> mod(BaseExpression const& other) const;
            virtual std::unique_ptr<BaseExpression> floor() const;
            virtual std::unique_ptr<BaseExpression> ceil() const;
            
            virtual void visit(ExpressionVisitor* visitor) const;
            
            virtual std::unique_ptr<BaseExpression> clonse() const;
            
        private:
            std::string variableName;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_VARIABLEEXPRESSION_H_ */