/* 
 * File:   LoopStatement.h
 * Author: Lukas Westhofen
 *
 * Created on 11. April 2015, 17:42
 */

#ifndef LOOPSTATEMENT_H
#define	LOOPSTATEMENT_H

#include "src/storm/storage/pgcl/PgclProgram.h"
#include "src/storm/storage/pgcl/CompoundStatement.h"
#include "src/storm/storage/pgcl/BooleanExpression.h"

namespace storm {
    namespace pgcl {
        /**
         * This class represents a guarded loop statement. The guard is saved as
         * a boolean expression. The body of the loop is again a PGCL program.
         */
        class LoopStatement : public Statement {
        public:
            LoopStatement() = default;
            /**
             * Constructs a loop statement initialized with the given condition
             * and loop body program.
             * @param condition The guard of the loop.
             * @param body The body of the loop.
             */
            LoopStatement(storm::pgcl::BooleanExpression const& condition, std::shared_ptr<storm::pgcl::PgclBlock> const& body);
            LoopStatement(const LoopStatement& orig) = default;
            virtual ~LoopStatement() = default;
            void accept(class AbstractStatementVisitor&);
            /**
             * Returns the loop body program.
             * @return The loop body program.
             */
            std::shared_ptr<storm::pgcl::PgclBlock> const& getBody() const;
            /**
             * Returns the guard of the loop.
             * @return The boolean condition of the loop.
             */
            storm::pgcl::BooleanExpression const& getCondition() const;
        private:
            /// Represents the loop body.
            std::shared_ptr<storm::pgcl::PgclBlock> body;
            /// Represents the loop guard.
            storm::pgcl::BooleanExpression condition;
        };
    }
}

#endif	/* LOOPSTATEMENT_H */

