#include "src/storage/prism/ParallelComposition.h"

#include <boost/algorithm/string/join.hpp>

namespace storm {
    namespace prism {
        
        ParallelComposition::ParallelComposition(std::shared_ptr<Composition> const& left, std::set<std::string> const& synchronizingActions, std::shared_ptr<Composition> const& right) : left(left), synchronizingActions(synchronizingActions), right(right) {
            // Intentionally left empty.
        }
        
        void ParallelComposition::writeToStream(std::ostream& stream) const {
            stream << "(" << *left << " |[" << boost::algorithm::join(synchronizingActions, ", ") << "]| " << *right << ")";
        }

    }
}