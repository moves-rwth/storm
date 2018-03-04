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
            enum ArcType { INPUT, OUTPUT, INHIBITOR };

            static modernjson::json translatePlace(storm::gspn::Place const& place, double x, double y);

            static modernjson::json translateImmediateTransition(storm::gspn::ImmediateTransition<double> const& transition, double x, double y);

            static modernjson::json translateTimedTransition(storm::gspn::TimedTransition<double> const& transition, double x, double y);

            static modernjson::json translateArc(storm::gspn::Transition const& transition, storm::gspn::Place const& place, uint64_t multiplicity, bool immediate, ArcType arctype);

            std::string static inline toJsonString(storm::gspn::Place const& place) {
                std::stringstream stream;
                stream << "p" << place.getID();
                return stream.str();
            }

            std::string static inline toJsonString(storm::gspn::Transition const& transition, bool immediate) {
                std::stringstream stream;
                stream << (immediate ? "i" : "t") << transition.getID();
                return stream.str();
            }

            std::string static inline toJsonString(storm::gspn::Transition const& transition, storm::gspn::Place const& place, ArcType arctype) {
                std::stringstream stream;
                stream << place.getID();
                switch (arctype) {
                    case INPUT:
                        stream << "i";
                        break;
                    case OUTPUT:
                        stream << "o";
                        break;
                    case INHIBITOR:
                        stream << "h";
                        break;
                    default:
                        STORM_LOG_ASSERT(false, "Unknown type " << arctype << " used.");
                }
                stream << transition.getID();
                return stream.str();
            }
        };
       
    }
}
