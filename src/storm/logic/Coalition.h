#ifndef STORM_LOGIC_COALITION_H_
#define STORM_LOGIC_COALITION_H_

#include <vector>
#include <string>

#include <boost/optional.hpp>
#include "storm/storage/BoostTypes.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace logic {

        class Coalition {
        public:
            Coalition() = default;
            Coalition(std::vector<std::string> const& playerNames, std::vector<uint_fast32_t> const& playerIds);
            Coalition(Coalition const& other) = default;

            friend std::ostream& operator<<(std::ostream& stream, Coalition const& coalition);

        private:
            std::vector<std::string> playerNames;
            std::vector<uint_fast32_t> playerIds;
        };
    }
}


#endif /* STORM_LOGIC_COALITION_H_ */
