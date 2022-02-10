#pragma once

#include "storm/utility/macros.h"

#include "storm-gspn/storage/gspn/GSPN.h"

#include "storm/adapters/JsonAdapter.h"

namespace storm {
namespace gspn {

/**
 * Exports a GSPN into the JSON format for visualizing it.
 */
class GspnJsonExporter {
   public:
    typedef typename storm::json<double> Json;
    static void toStream(storm::gspn::GSPN const& gspn, std::ostream& os);

    static Json translate(storm::gspn::GSPN const& gspn);

   private:
    enum ArcType { INPUT, OUTPUT, INHIBITOR };

    static Json translatePlace(storm::gspn::Place const& place, double x, double y);

    static Json translateImmediateTransition(storm::gspn::ImmediateTransition<double> const& transition, double x, double y);

    static Json translateTimedTransition(storm::gspn::TimedTransition<double> const& transition, double x, double y);

    static Json translateArc(storm::gspn::Transition const& transition, storm::gspn::Place const& place, uint64_t multiplicity, bool immediate,
                             ArcType arctype);

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

}  // namespace gspn
}  // namespace storm
