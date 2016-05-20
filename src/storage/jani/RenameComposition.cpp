#include "src/storage/jani/RenameComposition.h"

#include <vector>
#include <sstream>

#include <boost/algorithm/string/join.hpp>

namespace storm {
    namespace jani {
        
        RenameComposition::RenameComposition(std::shared_ptr<Composition> const& subcomposition, std::map<std::string, boost::optional<std::string>> const& renaming) : subcomposition(subcomposition), renaming(renaming) {
            // Intentionally left empty.
        }
        
        Composition const& RenameComposition::getSubcomposition() const {
            return *subcomposition;
        }
        
        boost::any RenameComposition::accept(CompositionVisitor& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        void RenameComposition::write(std::ostream& stream) const {
            std::vector<std::string> renamingStrings;
            
            for (auto const& entry : renaming) {
                std::stringstream stream;
                stream << entry.first << " -> ";
                if (entry.second) {
                    stream << entry.second;
                } else {
                    stream << "tau";
                }
            }
            
            stream << "(" << subcomposition << ") / {" << boost::join(renamingStrings, ", ") << "}";
        }

        
    }
}