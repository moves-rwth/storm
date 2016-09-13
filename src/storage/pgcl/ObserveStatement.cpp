/* 
 * File:   ObserveStatement.cpp
 * Author: Lukas Westhofen
 * 
 * Created on 11. April 2015, 17:42
 */

#include "src/storage/pgcl/ObserveStatement.h"
#include "src/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace pgcl {
        ObserveStatement::ObserveStatement(storm::pgcl::BooleanExpression const& condition) : condition(condition) {
        }

        void ObserveStatement::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
            visitor.visit(*this);
        }

        storm::pgcl::BooleanExpression const& ObserveStatement::getCondition() const {
            return this->condition;
        }

    }
}

