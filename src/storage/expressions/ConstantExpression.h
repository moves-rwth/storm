#ifndef STORM_STORAGE_EXPRESSIONS_CONSTANTEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_CONSTANTEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        class ConstantExpression : public BaseExpression {
        public:
            /*!
             * Creates a constant expression with the given return type and constant name.
             *
             * @param returnType The return type of the expression.
             * @param constantName The name of the constant associated with this expression.
             */
            ConstantExpression(ExpressionReturnType returnType, std::string const& constantName);
            
            // Provide custom versions of copy construction and assignment.
            ConstantExpression(ConstantExpression const& other);
            ConstantExpression& operator=(ConstantExpression const& other);
            
            // Create default variants of move construction/assignment and virtual destructor.
            ConstantExpression(ConstantExpression&&) = default;
            ConstantExpression& operator=(ConstantExpression&&) = default;
            virtual ~ConstantExpression() = default;
            
            // Override base class methods.
            virtual bool isConstant() const override;
            virtual bool isTrue() const override;
            virtual bool isFalse() const override;
            virtual std::set<std::string> getVariables() const override;
            virtual std::set<std::string> getConstants() const override;
            virtual std::unique_ptr<BaseExpression> simplify() const override;
            
            /*!
             * Retrieves the name of the constant.
             *
             * @return The name of the constant.
             */
            std::string const& getConstantName() const;
            
        private:
            // The name of the constant.
            std::string constantName;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_CONSTANTEXPRESSION_H_ */
