#include "storm/logic/Coalition.h"

namespace storm {
    namespace logic {

        Coalition::Coalition(std::vector<boost::variant<std::string, uint64_t>> playerIds) : playerIds(playerIds) {
            // Intentionally left empty.
        }

        std::ostream& operator<<(std::ostream& stream, Coalition const& coalition) {
            bool firstItem = true;
            stream << "<<";
            for (auto const& id : coalition.playerIds) {
                if(firstItem) { firstItem = false; } else { stream << ","; }
                stream << id;
            }
            stream << ">> ";
            return stream;
        }
    }
}
