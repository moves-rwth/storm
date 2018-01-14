#pragma  once

#include "storm/utility/macros.h"

#include "storm-gspn/storage/gspn/GSPN.h"

// JSON parser
#include "json.hpp"
namespace modernjson {
    using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, double, std::allocator>;
}

namespace storm {
    namespace gspn {

        /**
         * Exports a GSPN into the JSON format for visualizing it.
         */
        class GspnJsonExporter {

        public:
            static void toStream(storm::gspn::GSPN const& gspn, std::ostream& os);

            static modernjson::json translate(storm::gspn::GSPN const& gspn);

        private:
            static modernjson::json translatePlace(storm::gspn::Place const& place);

            static modernjson::json translateImmediateTransition(storm::gspn::ImmediateTransition<double> const& transition);

            static modernjson::json translateTimedTransition(storm::gspn::TimedTransition<double> const& transition);
        };
       
    }
}
