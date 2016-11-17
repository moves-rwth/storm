/* 
 * File:   SimpleStatement.h
 * Author: Lukas Westhofen
 *
 * Created on 11. April 2015, 17:41
 */

#ifndef SIMPLESTATEMENT_H
#define	SIMPLESTATEMENT_H

#include "src/storm/storage/pgcl/Statement.h"
#include "src/storm/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace pgcl {
        /*
         * Simple statements are statements not containing other PGCL programs.
         * Exactly one variable can be part of that statement.
         */
        class SimpleStatement : public Statement {
        public:
            SimpleStatement() = default;
            SimpleStatement(const SimpleStatement& orig) = default;
            virtual ~SimpleStatement() = default;
            virtual void accept(class AbstractStatementVisitor& visitor) = 0;
        };
    }
}
#endif	/* SIMPLESTATEMENT_H */

