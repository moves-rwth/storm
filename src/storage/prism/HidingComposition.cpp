#include "src/storage/prism/HidingComposition.h"

#include <boost/algorithm/string/join.hpp>

namespace storm {
    namespace prism {
        
        HidingComposition::HidingComposition(std::shared_ptr<Composition> const& sub, std::set<std::string> const& actionsToHide) : sub(sub), actionsToHide(actionsToHide) {
            // Intentionally left empty.
        }

        boost::any HidingComposition::accept(CompositionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        Composition const& HidingComposition::getSubcomposition() const {
            return *sub;
        }
        
        void HidingComposition::writeToStream(std::ostream& stream) const {
            stream << "(" << *sub << ")" << " " << "{" << boost::join(actionsToHide, ", ") << "}";
        }

    }
}