#include "storm/storage/jani/traverser/AssignmentLevelFinder.h"


namespace storm {
    namespace jani {
        
        uint64_t AssignmentLevelFinder::getLowestAssignmentLevel(Model const& model) {
            uint64_t res = std::numeric_limits<uint64_t>::max();
            ConstJaniTraverser::traverse(model, &res);
            return res;
        }
        
        void AssignmentLevelFinder::traverse(Assignment const& assignment, boost::any const& data) {
            auto& res = *boost::any_cast<uint64_t*>(data);
            res = std::min<uint64_t>(res, assignment.getLevel());
        }
    }
}

