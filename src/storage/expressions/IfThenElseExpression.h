#ifndef STORM_STORAGE_EXPRESSIONS_IFTHENELSEEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_IFTHENELSEEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace expressions {
        class IfThenElseExpression : public BaseExpression {
        public:
            /*!
             * Creates an if-then-else expression with the given return type, condition and operands.
             *
             * @param returnType The return type of the expression.
             * @param thenExpression The expression evaluated if the condition evaluates true.
             * @param elseExpression The expression evaluated if the condition evaluates false.
             */
            IfThenElseExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& condition, std::shared_ptr<BaseExpression const> const& thenExpression, std::shared_ptr<BaseExpression const> const& elseExpression);
            
            // Instantiate constructors and assignments with their default implementations.
            IfThenElseExpression(IfThenElseExpression const& other) = default;
            IfThenElseExpression& operator=(IfThenElseExpression const& other) = default;
#ifndef WINDOWS
            IfThenElseExpression(IfThenElseExpression&&) = default;
            IfThenElseExpression& operator=(IfThenElseExpression&&) = default;
#endif
            virtual ~IfThenElseExpression() = default;
            
            // Override base class methods.
            virtual bool evaluateAsBool(Valuation const* valuation = nullptr) const override;
            virtual int_fast64_t evaluateAsInt(Valuation const* valuation = nullptr) const override;
            virtual double evaluateAsDouble(Valuation const* valuation = nullptr) const override;
            virtual bool isConstant() const override;
            virtual std::set<std::string> getVariables() const override;
            virtual std::set<std::string> getConstants() const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            
            /*!
             * Retrieves the condition expression of the if-then-else expression.
             *
             * @return The condition expression of the if-then-else expression.
             */
            std::shared_ptr<BaseExpression const> getCondition() const;

            /*!
             * Retrieves the then expression of the if-then-else expression.
             *
             * @return The then expression of the if-then-else expression.
             */
            std::shared_ptr<BaseExpression const> getThenExpression() const;

            /*!
             * Retrieves the else expression of the if-then-else expression.
             *
             * @return The else expression of the if-then-else expression.
             */
            std::shared_ptr<BaseExpression const> getElseExpression() const;

        protected:
            // Override base class method.
            virtual void printToStream(std::ostream& stream) const override;
            
        private:
            // The condition of the if-then-else.
            std::shared_ptr<BaseExpression const> condition;
            
            // The return expression of the if-part.
            std::shared_ptr<BaseExpression const> thenExpression;

            // The return expression of the else-part.
            std::shared_ptr<BaseExpression const> elseExpression;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_IFTHENELSEEXPRESSION_H_ */