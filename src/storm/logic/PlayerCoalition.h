#pragma once

#include <string>
#include <vector>

#include <boost/variant.hpp>
#include "storm/storage/PlayerIndex.h"

namespace storm {
namespace logic {

class PlayerCoalition {
   public:
    PlayerCoalition() = default;
    PlayerCoalition(std::vector<boost::variant<std::string, storm::storage::PlayerIndex>> playerIds);
    PlayerCoalition(PlayerCoalition const& other) = default;

    std::vector<boost::variant<std::string, storm::storage::PlayerIndex>> const& getPlayers() const;

    friend std::ostream& operator<<(std::ostream& stream, PlayerCoalition const& playerCoalition);

   private:
    std::vector<boost::variant<std::string, storm::storage::PlayerIndex>> _playerIds;
};
}  // namespace logic
}  // namespace storm
