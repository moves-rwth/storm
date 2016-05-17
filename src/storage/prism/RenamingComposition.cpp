#include "src/storage/prism/RenamingComposition.h"

#include <vector>
#include <sstream>
#include <boost/algorithm/string/join.hpp>

namespace storm {
    namespace prism {
        
        RenamingComposition::RenamingComposition(std::shared_ptr<Composition> const& left, std::map<std::string, boost::optional<std::string>> const& actionRenaming) : left(left), actionRenaming(actionRenaming) {
            // Intentionally left empty.
        }
        
        void RenamingComposition::writeToStream(std::ostream& stream) const {
            std::vector<std::string> renamings;
            for (auto const& renaming : actionRenaming) {
                std::stringstream s;
                if (renaming.second) {
                    s << renaming.second.get();
                }
                s << " <- " << renaming.first;
                renamings.push_back(s.str());
            }
            stream << *left << "{" << boost::join(renamings, ", ") << "}";
        }
        
    }
}