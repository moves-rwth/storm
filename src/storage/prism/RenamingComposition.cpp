#include "src/storage/prism/RenamingComposition.h"

#include <vector>
#include <sstream>
#include <boost/algorithm/string/join.hpp>

namespace storm {
    namespace prism {
        
        RenamingComposition::RenamingComposition(std::shared_ptr<Composition> const& sub, std::map<std::string, std::string> const& actionRenaming) : sub(sub), actionRenaming(actionRenaming) {
            // Intentionally left empty.
        }
        
        boost::any RenamingComposition::accept(CompositionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        Composition const& RenamingComposition::getSubcomposition() const {
            return *sub;
        }
        
        void RenamingComposition::writeToStream(std::ostream& stream) const {
            std::vector<std::string> renamings;
            for (auto const& renaming : actionRenaming) {
                std::stringstream s;
                s << renaming.second << " <- " << renaming.first;
                renamings.push_back(s.str());
            }
            stream << *sub << "{" << boost::join(renamings, ", ") << "}";
        }
        
    }
}