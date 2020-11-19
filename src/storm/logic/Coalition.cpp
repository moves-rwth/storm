#include "storm/logic/Coalition.h"

namespace storm {
    namespace logic {

        Coalition::Coalition(std::vector<std::string> const& playerNames,
                             std::vector<uint_fast32_t> const& playerIds) : playerNames(playerNames), playerIds(playerIds) {
            // Intentionally left empty.
        }

        std::ostream& operator<<(std::ostream& stream, Coalition const& coalition) {
            stream << "<<";
            for (auto const& playerName : coalition.playerNames) {
                stream << playerName << ", ";
            }
            for (auto const& playerId : coalition.playerIds) {
                stream << playerId << ", ";
            }
            stream << ">>";
            return stream;
        }
    }
}
