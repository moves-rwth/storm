#pragma once

#include <limits>

namespace storm {
namespace storage {

typedef uint_fast64_t PlayerIndex;
PlayerIndex const INVALID_PLAYER_INDEX = std::numeric_limits<PlayerIndex>::max();
}  // namespace storage
}  // namespace storm