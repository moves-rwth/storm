#ifndef STORM_STORAGE_EXPRESSIONS_INTEGERLITERALEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_INTEGERLITERALEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace expressions {
        class IntegerLiteralExpression : public BaseExpression {
        public:
            /*!
             * Creates an integer literal expression with the given value.
             *
             * @param value The value of the integer literal.
             */
            IntegerLiteralExpression(int_fast64_t value);
            
            // Instantiate constructors and assignments with their default implementations.
            IntegerLiteralExpression(IntegerLiteralExpression const& other) = default;
            IntegerLiteralExpression& operator=(IntegerLiteralExpression const& other) = default;
#ifndef WINDOWS
            IntegerLiteralExpression(IntegerLiteralExpression&&) = default;
            IntegerLiteralExpression& operator=(IntegerLiteralExpression&&) = default;
#endif
            virtual ~IntegerLiteralExpression() = default;
            
            // Override base class methods.
            virtual int_fast64_t evaluateAsInt(Valuation const* valuation = nullptr) const override;
            virtual double evaluateAsDouble(Valuation const* valuation = nullptr) const override;
            virtual bool isLiteral() const override;
			virtual std::set<std::string> getVariables() const override;
			virtual std::map<std::string, ExpressionReturnType> getVariablesAndTypes() const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            
            /*!
             * Retrieves the value of the integer literal.
             *
             * @return The value of the integer literal.
             */
            int_fast64_t getValue() const;
            
        protected:
            // Override base class method.
            virtual void printToStream(std::ostream& stream) const override;

        private:
            // The value of the integer literal.
            int_fast64_t value;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_INTEGERLITERALEXPRESSION_H_ */