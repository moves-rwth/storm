/* 
 * File:   NondeterministicBranch.cpp
 * Author: Lukas Westhofen
 * 
 * Created on 11. April 2015, 17:44
 */

#include "src/storage/pgcl/NondeterministicBranch.h"
#include "src/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace pgcl {
        NondeterministicBranch::NondeterministicBranch(std::shared_ptr<storm::pgcl::PgclBlock> const& left, std::shared_ptr<storm::pgcl::PgclBlock> const& right) {
            leftBranch = left;
            rightBranch = right;
        }

        void NondeterministicBranch::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
            visitor.visit(*this);
        }

        bool NondeterministicBranch::isNondet() const {
            return true;
        }
    }
}

