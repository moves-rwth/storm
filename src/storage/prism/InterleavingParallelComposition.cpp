#include "src/storage/prism/InterleavingParallelComposition.h"

namespace storm {
    namespace prism {
        
        InterleavingParallelComposition::InterleavingParallelComposition(std::shared_ptr<Composition> const& left, std::shared_ptr<Composition> const& right) : ParallelComposition(left, right) {
            // Intentionally left empty.
        }
     
        boost::any InterleavingParallelComposition::accept(CompositionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        void InterleavingParallelComposition::writeToStream(std::ostream& stream) const {
            stream << "(" << *left << " ||| " << *right << ")";
        }
        
    }
}