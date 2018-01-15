#include "GspnJsonExporter.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"

#include <algorithm>
#include <string>

namespace storm {
    namespace gspn {

        void GspnJsonExporter::toStream(storm::gspn::GSPN const& gspn, std::ostream& os) {
            os << translate(gspn).dump(4) << std::endl;
        }

        modernjson::json GspnJsonExporter::translate(storm::gspn::GSPN const& gspn) {
            modernjson::json jsonGspn;

            // Export places
            for (const auto &place : gspn.getPlaces()) {
                modernjson::json jsonPlace = translatePlace(place);
                jsonGspn.push_back(jsonPlace);
            }

            // Export immediate transitions
            for (const auto &transition : gspn.getImmediateTransitions()) {
                modernjson::json jsonImmediateTransition = translateImmediateTransition(transition);
                jsonGspn.push_back(jsonImmediateTransition);
            }

            // Export timed transitions
            for (const auto &transition : gspn.getTimedTransitions()) {
                modernjson::json jsonTimedTransition = translateTimedTransition(transition);
                jsonGspn.push_back(jsonTimedTransition);
            }

            // Export arcs
            std::vector<storm::gspn::Place> places =  gspn.getPlaces();
            // Export arcs for immediate transitions
            for (const auto &transition : gspn.getImmediateTransitions()) {
                // Export input arcs
                for (auto const& entry : transition.getInputPlaces()) {
                    storm::gspn::Place place = places.at(entry.first);
                    modernjson::json jsonInputArc = translateArc(transition, place, entry.second, true, ArcType::INPUT);
                    jsonGspn.push_back(jsonInputArc);
                }

                // Export inhibitor arcs
                for (auto const& entry : transition.getInhibitionPlaces()) {
                    storm::gspn::Place place = places.at(entry.first);
                    modernjson::json jsonInputArc = translateArc(transition, place, entry.second, true, ArcType::INHIBITOR);
                    jsonGspn.push_back(jsonInputArc);
                }

                // Export output arcs
                for (auto const& entry : transition.getOutputPlaces()) {
                    storm::gspn::Place place = places.at(entry.first);
                    modernjson::json jsonInputArc = translateArc(transition, place, entry.second, true, ArcType::OUTPUT);
                    jsonGspn.push_back(jsonInputArc);
                }
            }
            // Export arcs for timed transitions
            for (const auto &transition : gspn.getTimedTransitions()) {
                // Export input arcs
                for (auto const& entry : transition.getInputPlaces()) {
                    storm::gspn::Place place = places.at(entry.first);
                    modernjson::json jsonInputArc = translateArc(transition, place, entry.second, false, ArcType::INPUT);
                    jsonGspn.push_back(jsonInputArc);
                }

                // Export inhibitor arcs
                for (auto const& entry : transition.getInhibitionPlaces()) {
                    storm::gspn::Place place = places.at(entry.first);
                    modernjson::json jsonInputArc = translateArc(transition, place, entry.second, false, ArcType::INHIBITOR);
                    jsonGspn.push_back(jsonInputArc);
                }

                // Export output arcs
                for (auto const& entry : transition.getOutputPlaces()) {
                    storm::gspn::Place place = places.at(entry.first);
                    modernjson::json jsonInputArc = translateArc(transition, place, entry.second, false, ArcType::OUTPUT);
                    jsonGspn.push_back(jsonInputArc);
                }
            }
            return jsonGspn;
        }


        modernjson::json GspnJsonExporter::translatePlace(storm::gspn::Place const& place) {
            modernjson::json data;
            data["id"] = toJsonString(place);
            data["name"] = place.getName();
            data["marking"] = place.getNumberOfInitialTokens();

            modernjson::json jsonPlace;
            jsonPlace["data"] = data;
            jsonPlace["group"] = "nodes";
            jsonPlace["classes"] = "place";
            return jsonPlace;
        }

        modernjson::json GspnJsonExporter::translateImmediateTransition(storm::gspn::ImmediateTransition<double> const& transition) {
            modernjson::json data;
            data["id"] = toJsonString(transition, true);
            data["name"] = transition.getName();
            data["priority"] = transition.getPriority();
            data["weight"] = transition.getWeight();

            modernjson::json jsonTrans;
            jsonTrans["data"] = data;
            jsonTrans["group"] = "nodes";
            jsonTrans["classes"] = "trans_im";
            return jsonTrans;
        }

         modernjson::json GspnJsonExporter::translateTimedTransition(storm::gspn::TimedTransition<double> const& transition) {
             modernjson::json data;
             data["id"] = toJsonString(transition, false);
             data["name"] = transition.getName();
             data["rate"] = transition.getRate();
             data["priority"] = transition.getPriority();

             modernjson::json jsonTrans;
             jsonTrans["data"] = data;
             jsonTrans["group"] = "nodes";
             jsonTrans["classes"] = "trans_time";
             return jsonTrans;
         }

        modernjson::json GspnJsonExporter::translateArc(storm::gspn::Transition const& transition, storm::gspn::Place const& place, uint64_t multiplicity, bool immediate, ArcType arctype) {
            modernjson::json data;
            data["id"] = toJsonString(transition, place, arctype);
            data["source"] = toJsonString(place);
            data["target"] = toJsonString(transition, immediate);
            data["mult"] = multiplicity;

            modernjson::json jsonArc;
            jsonArc["data"] = data;
            //jsonTrans["group"] = "nodes";
            switch (arctype) {
                case INPUT:
                    jsonArc["classes"] = "input";
                    break;
                case OUTPUT:
                    jsonArc["classes"] = "output";
                    break;
                case INHIBITOR:
                    jsonArc["classes"] = "inhibit";
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Unknown type " << arctype << " used.");
            }
            return jsonArc;
        }

    }
}
