#include <src/exceptions/IllegalFunctionCallException.h>
#include "GspnBuilder.h"

#include "src/utility/macros.h"
#include "src/exceptions/IllegalFunctionCallException.h"
#include "Place.h"

namespace storm {
    namespace gspn {
        uint_fast64_t GspnBuilder::addPlace(int_fast64_t const& capacity, uint_fast64_t const& initialTokens) {
            auto place = storm::gspn::Place();
            place.setCapacity(capacity);
            place.setID(nextStateID);
            place.setNumberOfInitialTokens(initialTokens);
            gspn.addPlace(place);
            return nextStateID++;
        }

        uint_fast64_t GspnBuilder::addImmediateTransition(uint_fast64_t const& priority, double const& weight) {
            auto trans = storm::gspn::ImmediateTransition<double>();
            trans.setName(std::to_string(nextTransitionID));
            trans.setPriority(priority);
            trans.setWeight(weight);
            trans.setID(nextTransitionID);
            gspn.addImmediateTransition(trans);
            return nextTransitionID++;
        }

        uint_fast64_t GspnBuilder::addTimedTransition(uint_fast64_t const &priority, double const &rate) {
            auto trans = storm::gspn::TimedTransition<double>();
            trans.setName(std::to_string(nextTransitionID));
            trans.setPriority(priority);
            trans.setRate(rate);
            trans.setID(nextTransitionID);
            gspn.addTimedTransition(trans);
            return nextTransitionID++;
        }

        void GspnBuilder::addInputArc(uint_fast64_t const &from, uint_fast64_t const &to,
                                      uint_fast64_t const &multiplicity) {
            auto transPair = gspn.getTransition(std::to_string(to));
            if (!std::get<0>(transPair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The transition with the name \"" + std::to_string(to) + "\" does not exist.");
            }

            auto placePair = gspn.getPlace(from);
            if (!std::get<0>(placePair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The place with the id \"" + std::to_string(from) + "\" does not exist.");
            }

            std::get<1>(transPair)->setInputArcMultiplicity(*std::get<1>(placePair), multiplicity);
        }

        void GspnBuilder::addInhibitionArc(uint_fast64_t const& from, uint_fast64_t const& to, uint_fast64_t const& multiplicity) {
            auto transPair = gspn.getTransition(std::to_string(to));
            if (!std::get<0>(transPair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The transition with the name \"" + std::to_string(to) + "\" does not exist.");
            }

            auto placePair = gspn.getPlace(from);
            if (!std::get<0>(placePair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The place with the id \"" + std::to_string(from) + "\" does not exist.");
            }

            std::get<1>(transPair)->setInhibitionArcMultiplicity(*std::get<1>(placePair), multiplicity);
        }

        void GspnBuilder::addOutputArc(uint_fast64_t const& from, uint_fast64_t const& to, uint_fast64_t const& multiplicity) {
            auto transPair = gspn.getTransition(std::to_string(from));
            if (!std::get<0>(transPair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The transition with the name \"" + std::to_string(to) + "\" does not exist.");
            }

            auto placePair = gspn.getPlace(to);
            if (!std::get<0>(placePair)) {
                STORM_LOG_THROW(false, storm::exceptions::IllegalFunctionCallException, "The place with the id \"" + std::to_string(from) + "\" does not exist.");
            }

            std::get<1>(transPair)->setOutputArcMultiplicity(*std::get<1>(placePair), multiplicity);
        }

        storm::gspn::GSPN const& GspnBuilder::buildGspn() const {
            return gspn;
        }
    }
}