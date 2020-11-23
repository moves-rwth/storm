#ifndef STORM_LOGIC_COALITION_H_
#define STORM_LOGIC_COALITION_H_

#include <vector>
#include <string>

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include "storm/storage/BoostTypes.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace logic {

        class Coalition {
        public:
            Coalition() = default;
            Coalition(std::vector<boost::variant<std::string, uint64_t>>);
            Coalition(Coalition const& other) = default;

            friend std::ostream& operator<<(std::ostream& stream, Coalition const& coalition);

        private:
            std::vector<boost::variant<std::string, uint64_t>> playerIds;
        };
    }
}


#endif /* STORM_LOGIC_COALITION_H_ */
