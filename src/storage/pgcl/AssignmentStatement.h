/* 
 * File:   AssignmentStatement.h
 * Author: Lukas Westhofen
 *
 * Created on 11. April 2015, 17:42
 */

#ifndef ASSIGNMENTSTATEMENT_H
#define	ASSIGNMENTSTATEMENT_H

#include "src/storage/pgcl/SimpleStatement.h"
#include "src/storage/pgcl/UniformExpression.h"
#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/Variable.h"
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

namespace storm {
    namespace pgcl {
        /**
         * This class represents a simple assignment statement of the form
         * identifier := expression; where the expression is either handled by
         * the expression manager or is a uniform distribution expression.
         */
        class AssignmentStatement : public SimpleStatement {
        public:
            AssignmentStatement() = default;
            /**
             * Constructs an assignment statement with the variable as the left
             * side and the expression as the right side of the assignment.
             * @param variable The left hand variable of the assignment.
             * @param expression The right hand expression of the assignment.
             */
            AssignmentStatement(storm::expressions::Variable const& variable, boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& expression);
            AssignmentStatement(const AssignmentStatement& orig) = default;
            virtual ~AssignmentStatement() = default;
            std::size_t getNumberOfOutgoingTransitions();
            void accept(class AbstractStatementVisitor&);
            /**
             * Returns the right hand expression of the assignemnt.
             * @return The expression of the assignment.
             */
            boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& getExpression();
            /**
             * Returns the left hand variable of the assignemnt.
             * @return The variable to which the expression is assigned.
             */
            storm::expressions::Variable const& getVariable();
        private:
            /// Represents the variable of our assignment statement.
            storm::expressions::Variable variable;
            /// Represents the right hand side of our assignment statement.
            boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> expression;
        };
    }
}

#endif	/* ASSIGNMENTSTATEMENT_H */

