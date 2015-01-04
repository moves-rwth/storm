#ifndef STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_

#include <stack>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/storage/expressions/SimpleValuation.h"

namespace storm {
    namespace expressions {
        class LinearCoefficientVisitor : public ExpressionVisitor {
        public:
            struct VariableCoefficients {
            public:
                VariableCoefficients(double constantPart = 0);

                VariableCoefficients(VariableCoefficients const& other) = default;
                VariableCoefficients& operator=(VariableCoefficients const& other) = default;
                VariableCoefficients(VariableCoefficients&& other) = default;
                VariableCoefficients& operator=(VariableCoefficients&& other) = default;
                
                VariableCoefficients& operator+=(VariableCoefficients&& other);
                VariableCoefficients& operator-=(VariableCoefficients&& other);
                VariableCoefficients& operator*=(VariableCoefficients&& other);
                VariableCoefficients& operator/=(VariableCoefficients&& other);
                
                void negate();
                void setCoefficient(storm::expressions::Variable const& variable, double coefficient);
                double getCoefficient(storm::expressions::Variable const& variable);
                
            private:
                std::map<storm::expressions::Variable, double> variableToCoefficientMapping;
                double constantPart;
            };
            
            /*!
             * Creates a linear coefficient visitor.
             */
            LinearCoefficientVisitor() = default;
            
            /*!
             * Computes the (double) coefficients of all identifiers appearing in the expression if the expression
             * was rewritten as a sum of atoms.. If the expression is not linear, an exception is thrown.
             *
             * @param expression The expression for which to compute the coefficients.
             * @return A structure representing the coefficients of the variables and the constant part.
             */
            VariableCoefficients getLinearCoefficients(Expression const& expression);
            
            virtual boost::any visit(IfThenElseExpression const& expression) override;
            virtual boost::any visit(BinaryBooleanFunctionExpression const& expression) override;
            virtual boost::any visit(BinaryNumericalFunctionExpression const& expression) override;
            virtual boost::any visit(BinaryRelationExpression const& expression) override;
            virtual boost::any visit(VariableExpression const& expression) override;
            virtual boost::any visit(UnaryBooleanFunctionExpression const& expression) override;
            virtual boost::any visit(UnaryNumericalFunctionExpression const& expression) override;
            virtual boost::any visit(BooleanLiteralExpression const& expression) override;
            virtual boost::any visit(IntegerLiteralExpression const& expression) override;
            virtual boost::any visit(DoubleLiteralExpression const& expression) override;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_ */