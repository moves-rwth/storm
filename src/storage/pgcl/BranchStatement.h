/* 
 * File:   BranchStatement.h
 * Author: Lukas Westhofen
 *
 * Created on 11. April 2015, 17:42
 */

#ifndef BRANCHSTATEMENT_H
#define	BRANCHSTATEMENT_H

#include "src/storage/pgcl/PgclProgram.h"
#include "src/storage/pgcl/CompoundStatement.h"

namespace storm {
    namespace pgcl {
        /**
         * This abstract class handles the branching statements. Every branch
         * statement has a right and a left branch. Since branch statements are
         * compound statements, every branch is again a complete PGCL program
         * itself.
         */
        class BranchStatement : public CompoundStatement {
        public:
            BranchStatement() = default;
            BranchStatement(const BranchStatement& orig) = default;
            virtual ~BranchStatement() = default;
            virtual void accept(class AbstractStatementVisitor&) = 0;
            std::size_t getNumberOfOutgoingTransitions();
            /**
             * Returns the left branch of the statement.
             * @return The left branch PGCL program.
             */
            std::shared_ptr<storm::pgcl::PgclProgram> getLeftBranch();
            /**
             * Returns the right branch of the statement.
             * @return The right branch PGCL program.
             */
            std::shared_ptr<storm::pgcl::PgclProgram> getRightBranch();
        protected:
            std::shared_ptr<storm::pgcl::PgclProgram> leftBranch;
            std::shared_ptr<storm::pgcl::PgclProgram> rightBranch;
        };
    }
}

#endif	/* BRANCHSTATEMENT_H */