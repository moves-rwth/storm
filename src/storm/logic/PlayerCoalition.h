#pragma once

#include <string>
#include <variant>
#include <vector>

#include "storm/storage/PlayerIndex.h"

namespace storm {
namespace logic {

class PlayerCoalition {
   public:
    PlayerCoalition() = default;
    PlayerCoalition(std::vector<std::variant<std::string, storm::storage::PlayerIndex>> const& playerIds);

    std::vector<std::variant<std::string, storm::storage::PlayerIndex>> const& getPlayers() const;

    friend std::ostream& operator<<(std::ostream& stream, PlayerCoalition const& playerCoalition);

   private:
    std::vector<std::variant<std::string, storm::storage::PlayerIndex>> _playerIds;
};
}  // namespace logic
}  // namespace storm
