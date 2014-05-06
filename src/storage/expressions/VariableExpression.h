#ifndef STORM_STORAGE_EXPRESSIONS_VARIABLEEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_VARIABLEEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace expressions {
        class VariableExpression : public BaseExpression {
        public:
            /*!
             * Creates a variable expression with the given return type and variable name.
             *
             * @param returnType The return type of the variable expression.
             * @param variableName The name of the variable associated with this expression.
             */
            VariableExpression(ExpressionReturnType returnType, std::string const& variableName);
            
            // Instantiate constructors and assignments with their default implementations.
            VariableExpression(VariableExpression const&) = default;
            VariableExpression& operator=(VariableExpression const&) = default;
#ifndef WINDOWS
            VariableExpression(VariableExpression&&) = default;
            VariableExpression& operator=(VariableExpression&&) = default;
#endif
            virtual ~VariableExpression() = default;

            // Override base class methods.
            virtual bool evaluateAsBool(Valuation const* valuation = nullptr) const override;
            virtual int_fast64_t evaluateAsInt(Valuation const* valuation = nullptr) const override;
            virtual double evaluateAsDouble(Valuation const* valuation = nullptr) const override;
            virtual std::string const& getIdentifier() const override;
            virtual bool containsVariables() const override;
            virtual std::set<std::string> getVariables() const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;

            /*!
             * Retrieves the name of the variable associated with this expression.
             *
             * @return The name of the variable.
             */
            std::string const& getVariableName() const;

        protected:
            // Override base class method.
            virtual void printToStream(std::ostream& stream) const override;

        private:
            // The variable name associated with this expression.
            std::string variableName;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_VARIABLEEXPRESSION_H_ */