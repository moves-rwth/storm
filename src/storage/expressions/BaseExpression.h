#ifndef STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_

#include "src/storage/expressions/Valuation.h"
#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {
        /*!
         * Each node in an expression tree has a uniquely defined type from this enum.
         */
        enum ExpressionReturnType {undefined, bool_, int_, double_};
        
        class BaseExpression {
        public:
            
            BaseExpression();
            BaseExpression(ExpressionReturnType returnType);
            virtual ~BaseExpression() = default;
            
            ExpressionReturnType getReturnType() const;
            
            virtual int_fast64_t evaluateAsInt(Valuation const& evaluation) const = 0;
            virtual bool evaluateAsBool(Valuation const& evaluation) const = 0;
            virtual double evaluateAsDouble(Valuation const& evaluation) const = 0;
            
            virtual std::unique_ptr<BaseExpression> operator+(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> operator-(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> operator-() const = 0;
            virtual std::unique_ptr<BaseExpression> operator*(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> operator/(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> operator&(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> operator|(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> operator~() const = 0;
            
            virtual std::unique_ptr<BaseExpression> equals(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> notEquals(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> greater(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> greaterOrEqual(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> less(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> lessOrEqual(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> minimum(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> maximum(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> mod(BaseExpression const& other) const = 0;
            virtual std::unique_ptr<BaseExpression> floor() const = 0;
            virtual std::unique_ptr<BaseExpression> ceil() const = 0;

            virtual void visit(ExpressionVisitor* visitor) const = 0;
            
        protected:
            void checkType(ExpressionReturnType actualType, ExpressionReturnType expectedType, std::string const& errorMessage) const;
            
        private:
            ExpressionReturnType returnType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_ */