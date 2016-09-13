/* 
 * File:   AssignmentStatement.cpp
 * Author: Lukas Westhofen
 * 
 * Created on 11. April 2015, 17:42
 */

#include "src/storage/pgcl/AssignmentStatement.h"

namespace storm {
    namespace pgcl {
        AssignmentStatement::AssignmentStatement(storm::expressions::Variable const& variable, boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& expression) :
            variable(variable), expression(expression) {
        }
        
        boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& AssignmentStatement::getExpression() const {
            return this->expression;
        }

        storm::expressions::Variable const& AssignmentStatement::getVariable() const {
            return this->variable;
        }

        void AssignmentStatement::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
            visitor.visit(*this);
        }
    }
}

