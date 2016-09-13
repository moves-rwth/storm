/* 
 * File:   ObserveStatement.h
 * Author: Lukas Westhofen
 *
 * Created on 11. April 2015, 17:42
 */

#ifndef OBSERVESTATEMENT_H
#define	OBSERVESTATEMENT_H

#include "src/storage/pgcl/SimpleStatement.h"
#include "src/storage/pgcl/BooleanExpression.h"

namespace storm {
    namespace pgcl {
        /**
         * This class represents an observe statement. Observe statements
         * include a condition. If this condition doesn't hold, the program
         * stops at that point in its execution.
         */
        class ObserveStatement : public SimpleStatement {
        public:
            ObserveStatement() = default;
            /**
             * Constructs an observe statement initialized with the given
             * condition.
             * @param condition The condition of the observe statement.
             */
            ObserveStatement(storm::pgcl::BooleanExpression const& condition);
            ObserveStatement(const ObserveStatement& orig) = default;
            virtual ~ObserveStatement() = default;
            /**
             * Returns the condition of the observe statement.
             * @return The boolean expression of the observe statement.
             */
            storm::pgcl::BooleanExpression const& getCondition() const;
            void accept(class AbstractStatementVisitor&);
        private:
            /// Represents the assigned condition.
            storm::pgcl::BooleanExpression condition;
        };
    }
}

#endif	/* OBSERVESTATEMENT_H */

