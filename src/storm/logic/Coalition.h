#pragma once

#include <vector>
#include <string>

#include <boost/variant.hpp>
#include "storm/storage/PlayerIndex.h"

namespace storm {
    namespace logic {

        class Coalition {
        public:
            Coalition() = default;
            Coalition(std::vector<boost::variant<std::string, storm::storage::PlayerIndex>> playerIds);
            Coalition(Coalition const& other) = default;

            friend std::ostream& operator<<(std::ostream& stream, Coalition const& coalition);

        private:
            std::vector<boost::variant<std::string, storm::storage::PlayerIndex>> _playerIds;
        };
    }
}
