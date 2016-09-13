/* 
 * File:   BranchStatement.cpp
 * Author: Lukas Westhofen
 * 
 * Created on 11. April 2015, 17:42
 */

#include "src/storage/pgcl/BranchStatement.h"
#include "src/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace pgcl {
        std::shared_ptr<storm::pgcl::PgclBlock> const& BranchStatement::getLeftBranch() const {
            return this->leftBranch;
        }

        std::shared_ptr<storm::pgcl::PgclBlock> const& BranchStatement::getRightBranch() const {
            return this->rightBranch;
        }

    }
}