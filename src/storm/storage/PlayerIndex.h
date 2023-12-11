#pragma once

#include <cstdint>
#include <limits>

namespace storm::storage {
typedef uint64_t PlayerIndex;
PlayerIndex const INVALID_PLAYER_INDEX = std::numeric_limits<PlayerIndex>::max();
}  // namespace storm::storage
