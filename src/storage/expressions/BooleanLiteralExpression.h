#ifndef STORM_STORAGE_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace expressions {
        class BooleanLiteralExpression : public BaseExpression {
        public:
            /*!
             * Creates a boolean literal expression with the given value.
             *
             * @param value The value of the boolean literal.
             */
            BooleanLiteralExpression(bool value);
            
            // Instantiate constructors and assignments with their default implementations.
            BooleanLiteralExpression(BooleanLiteralExpression const& other) = default;
            BooleanLiteralExpression& operator=(BooleanLiteralExpression const& other) = default;
#ifndef WINDOWS
            BooleanLiteralExpression(BooleanLiteralExpression&&) = default;
            BooleanLiteralExpression& operator=(BooleanLiteralExpression&&) = default;
#endif
            virtual ~BooleanLiteralExpression() = default;

            // Override base class methods.
            virtual bool evaluateAsBool(Valuation const* valuation = nullptr) const override;
            virtual bool isLiteral() const override;
            virtual bool isTrue() const override;
            virtual bool isFalse() const override;
            virtual std::set<std::string> getVariables() const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            
            /*!
             * Retrieves the value of the boolean literal.
             *
             * @return The value of the boolean literal.
             */
            bool getValue() const;
            
        protected:
            // Override base class method.
            virtual void printToStream(std::ostream& stream) const override;
            
        private:
            // The value of the boolean literal.
            bool value;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_ */