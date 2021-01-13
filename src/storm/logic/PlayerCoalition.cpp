#include "storm/logic/PlayerCoalition.h"

namespace storm {
    namespace logic {

        PlayerCoalition::PlayerCoalition(std::vector<boost::variant<std::string, storm::storage::PlayerIndex>> playerIds) : _playerIds(playerIds) {
            // Intentionally left empty.
        }

        std::ostream& operator<<(std::ostream& stream, PlayerCoalition const& coalition) {
            bool firstItem = true;
            for (auto const& id : coalition._playerIds) {
                if (firstItem) { firstItem = false; } else { stream << ","; }
                stream << id;
            }
            return stream;
        }
    }
}
