#include "src/storage/jani/ParallelComposition.h"

#include <boost/algorithm/string/join.hpp>

namespace storm {
    namespace jani {
        
        ParallelComposition::ParallelComposition(std::shared_ptr<Composition> const& leftSubcomposition, std::shared_ptr<Composition> const& rightSubcomposition, std::set<std::string> const& alphabet) : leftSubcomposition(leftSubcomposition), rightSubcomposition(rightSubcomposition) {
            // Intentionally left empty.
        }
        
        Composition const& ParallelComposition::getLeftSubcomposition() const {
            return *leftSubcomposition;
        }
        
        Composition const& ParallelComposition::getRightSubcomposition() const {
            return *rightSubcomposition;
        }
        
        std::set<std::string> const& ParallelComposition::getSynchronizationAlphabet() const {
            return alphabet;
        }

        boost::any ParallelComposition::accept(CompositionVisitor& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        void ParallelComposition::write(std::ostream& stream) const {
            stream << this->getLeftSubcomposition() << " |[" << boost::algorithm::join(alphabet, ", ") << "]| " << this->getRightSubcomposition();
        }
        
    }
}