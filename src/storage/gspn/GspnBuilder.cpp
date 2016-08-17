#include <src/exceptions/IllegalFunctionCallException.h>
#include "GspnBuilder.h"

#include "src/utility/macros.h"
#include "src/exceptions/IllegalFunctionCallException.h"
#include "Place.h"

namespace storm {
    namespace gspn {
        void GspnBuilder::addPlace(uint_fast64_t const& id, int_fast64_t const& capacity, uint_fast64_t const& initialTokens) {
            addPlace(id, "place_" + std::to_string(id), capacity, initialTokens);
        }

        void GspnBuilder::addPlace(uint_fast64_t const& id, std::string const& name, int_fast64_t const& capacity, uint_fast64_t const& initialTokens) {
            auto place = storm::gspn::Place();
            place.setCapacity(capacity);
            place.setID(id);
            place.setName(name);
            place.setNumberOfInitialTokens(initialTokens);

            idToPlaceName.insert(std::pair<uint_fast64_t const, std::string const>(id, name));
            gspn.addPlace(place);
        }

        void GspnBuilder::addImmediateTransition(std::string const& name, uint_fast64_t const& priority, double const& weight) {
            auto trans = storm::gspn::ImmediateTransition<double>();
            trans.setName(name);
            trans.setPriority(priority);
            trans.setWeight(weight);

            gspn.addImmediateTransition(trans);
        }

        void GspnBuilder::addTimedTransition(std::string const& name, uint_fast64_t const& priority, double const& rate) {
            auto trans = storm::gspn::TimedTransition<double>();
            trans.setName(name);
            trans.setPriority(priority);
            trans.setRate(rate);

            gspn.addTimedTransition(trans);
        }

        void GspnBuilder::addInputArc(uint_fast64_t const& from, std::string const& to, uint_fast64_t const& multiplicity) {
            auto transPair = gspn.getTransition(to);
            if (!std::get<0>(transPair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The transition with the name \"" + to + "\" does not exist.");
            }

            auto placePair = gspn.getPlace(idToPlaceName.at(from));
            if (!std::get<0>(placePair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The place with the id \"" + std::to_string(from) + "\" does not exist.");
            }

            std::get<1>(transPair)->setInputArcMultiplicity(std::get<1>(placePair), multiplicity);
        }

        void GspnBuilder::addInhibitionArc(uint_fast64_t const& from, std::string const& to, uint_fast64_t const& multiplicity) {
            auto transPair = gspn.getTransition(to);
            if (!std::get<0>(transPair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The transition with the name \"" + to + "\" does not exist.");
            }

            auto placePair = gspn.getPlace(idToPlaceName.at(from));
            if (!std::get<0>(placePair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The place with the id \"" + std::to_string(from) + "\" does not exist.");
            }

            std::get<1>(transPair)->setInhibitionArcMultiplicity(std::get<1>(placePair), multiplicity);
        }

        void GspnBuilder::addOutputArc(uint_fast64_t const& from, std::string const& to, uint_fast64_t const& multiplicity) {
            auto transPair = gspn.getTransition(to);
            if (!std::get<0>(transPair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The transition with the name \"" + to + "\" does not exist.");
            }

            auto placePair = gspn.getPlace(idToPlaceName.at(from));
            if (!std::get<0>(placePair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The place with the id \"" + std::to_string(from) + "\" does not exist.");
            }

            std::get<1>(transPair)->setOutputArcMultiplicity(std::get<1>(placePair), multiplicity);
        }

        storm::gspn::GSPN const& GspnBuilder::buildGspn() const {
            return gspn;
        }
    }
}