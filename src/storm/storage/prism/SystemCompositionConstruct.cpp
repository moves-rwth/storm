#include "storm/storage/prism/SystemCompositionConstruct.h"

namespace storm {
    namespace prism {
        
        SystemCompositionConstruct::SystemCompositionConstruct(std::shared_ptr<Composition> const& composition, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), composition(composition) {
            // Intentionlly left empty.
        }
        
        Composition const& SystemCompositionConstruct::getSystemComposition() const {
            return *composition;
        }
        
        std::ostream& operator<<(std::ostream& stream, SystemCompositionConstruct const& systemCompositionConstruct) {
            stream << "system" << std::endl;
            stream << "\t" << systemCompositionConstruct.getSystemComposition() << std::endl;
            stream << "endsystem" << std::endl;
            return stream;
        }
        
    }
}
