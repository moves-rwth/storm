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
            
            // Instantiate constructors and assignments with their default implementations.
            ConstantExpression(ConstantExpression const& other) = default;
            ConstantExpression& operator=(ConstantExpression const& other) = default;
            ConstantExpression(ConstantExpression&&) = default;
            ConstantExpression& operator=(ConstantExpression&&) = default;
            virtual ~ConstantExpression() = default;
            
            // Override base class methods.
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
