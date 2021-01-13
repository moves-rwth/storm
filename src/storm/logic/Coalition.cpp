#include "storm/logic/Coalition.h"

namespace storm {
    namespace logic {

        Coalition::Coalition(std::vector<boost::variant<std::string, storm::storage::PlayerIndex>> playerIds) : _playerIds(playerIds) {
            // Intentionally left empty.
        }

        std::ostream& operator<<(std::ostream& stream, Coalition const& coalition) {
            bool firstItem = true;
            for (auto const& id : coalition._playerIds) {
                if (firstItem) { firstItem = false; } else { stream << ","; }
                stream << id;
            }
            return stream;
        }
    }
}
