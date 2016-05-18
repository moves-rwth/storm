#include "src/storage/prism/SynchronizingParallelComposition.h"

namespace storm {
    namespace prism {
        
        SynchronizingParallelComposition::SynchronizingParallelComposition(std::shared_ptr<Composition> const& left, std::shared_ptr<Composition> const& right) : ParallelComposition(left, right) {
            // Intentionally left empty.
        }
        
        boost::any SynchronizingParallelComposition::accept(CompositionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        void SynchronizingParallelComposition::writeToStream(std::ostream& stream) const {
            stream << "(" << this->getLeftSubcomposition() << " || " << this->getRightSubcomposition() << ")";
        }
        
    }
}