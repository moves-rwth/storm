/* 
 * File:   LoopStatement.cpp
 * Author: Lukas Westhofen
 * 
 * Created on 11. April 2015, 17:42
 */

#include "src/storage/pgcl/LoopStatement.h"
#include "src/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace pgcl {
        LoopStatement::LoopStatement(storm::pgcl::BooleanExpression const& condition, std::shared_ptr<storm::pgcl::PgclProgram> const& body) :
            body(body), condition(condition) {
        }

        std::shared_ptr<storm::pgcl::PgclProgram> LoopStatement::getBody() {
            return this->body;
        }
        
        storm::pgcl::BooleanExpression& LoopStatement::getCondition() {
            return this->condition;
        }

        void LoopStatement::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
            visitor.visit(*this);
        }

        std::size_t LoopStatement::getNumberOfOutgoingTransitions() {
            return 1;
        }
    }
}

