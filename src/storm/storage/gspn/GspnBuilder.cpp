#include "storm/exceptions/IllegalFunctionCallException.h"
#include "GspnBuilder.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalFunctionCallException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "Place.h"

namespace storm {
    namespace gspn {
        void GspnBuilder::setGspnName(std::string const& name) {
            gspnName = name;
        }
        
        uint_fast64_t GspnBuilder::addPlace(int_fast64_t const& capacity, uint_fast64_t const& initialTokens, std::string const& name) {
            auto newId = places.size();
            auto place = storm::gspn::Place(newId);
            place.setCapacity(capacity);
            place.setNumberOfInitialTokens(initialTokens);
            places.push_back(place);
            return newId;
        }

        uint_fast64_t GspnBuilder::addImmediateTransition(uint_fast64_t const& priority, double const& weight, std::string const& name) {
            auto trans = storm::gspn::ImmediateTransition<double>();
            auto newId = GSPN::immediateTransitionIdToTransitionId(immediateTransitions.size());
            trans.setName(std::to_string(newId));
            trans.setPriority(priority);
            trans.setWeight(weight);
            trans.setID(newId);
            immediateTransitions.push_back(trans);
            return newId;
            
        }

        uint_fast64_t GspnBuilder::addTimedTransition(uint_fast64_t const &priority, double const &rate, std::string const& name) {
            auto trans = storm::gspn::TimedTransition<double>();
            auto newId = GSPN::timedTransitionIdToTransitionId(timedTransitions.size());
            trans.setName(std::to_string(newId));
            trans.setPriority(priority);
            trans.setRate(rate);
            trans.setID(newId);
            timedTransitions.push_back(trans);
            return newId;
        }
        

        void GspnBuilder::addInputArc(uint_fast64_t const &from, uint_fast64_t const &to,
                                      uint_fast64_t const &multiplicity) {
            STORM_LOG_THROW(from < places.size(), storm::exceptions::InvalidArgumentException, "No place with id " << from << " known.");
            auto place = places.at(from);
            getTransition(to).setInputArcMultiplicity(place, multiplicity);
        }
        
        void GspnBuilder::addInputArc(std::string const& from, std::string const& to, uint64_t multiplicity) {
            STORM_LOG_THROW(placeNames.count(from) != 0, storm::exceptions::InvalidArgumentException, "Could not find a place with name '" << from << "'");
            STORM_LOG_THROW(transitionNames.count(to) != 0, storm::exceptions::InvalidArgumentException, "Could not find a transition with name << '" << to << "'");
            addInputArc(placeNames.at(from), transitionNames.at(to));
        }

        void GspnBuilder::addInhibitionArc(uint_fast64_t const& from, uint_fast64_t const& to, uint_fast64_t const& multiplicity) {
            STORM_LOG_THROW(from < places.size(), storm::exceptions::InvalidArgumentException, "No place with id " << from << " known.");
            auto place = places.at(from);

            getTransition(to).setInhibitionArcMultiplicity(place, multiplicity);
        }
        
        void GspnBuilder::addInhibitionArc(std::string const& from, std::string const& to, uint64_t multiplicity) {
            STORM_LOG_THROW(placeNames.count(from) != 0, storm::exceptions::InvalidArgumentException, "Could not find a place with name '" << from << "'");
            STORM_LOG_THROW(transitionNames.count(to) != 0, storm::exceptions::InvalidArgumentException, "Could not find a transition with name << '" << to << "'");
            addInhibitionArc(placeNames.at(from), transitionNames.at(to));
        }

        void GspnBuilder::addOutputArc(uint_fast64_t const& from, uint_fast64_t const& to, uint_fast64_t const& multiplicity) {
            STORM_LOG_THROW(to < places.size(), storm::exceptions::InvalidArgumentException, "No place with id " << to << " known.");
            auto place = places.at(to);
            getTransition(from).setOutputArcMultiplicity(place, multiplicity);
        }
        
        void GspnBuilder::addOutputArc(std::string const& from, std::string const& to, uint64_t multiplicity) {
            STORM_LOG_THROW(placeNames.count(to) != 0, storm::exceptions::InvalidArgumentException, "Could not find a place with name '" << to << "'");
            STORM_LOG_THROW(transitionNames.count(from) != 0, storm::exceptions::InvalidArgumentException, "Could not find a transition with name << '" << from << "'");
            addOutputArc(transitionNames.at(from), placeNames.at(to));
        }
        
        Transition& GspnBuilder::getTransition(uint64_t id) {
            if (isTimedTransitionId(id)) {
                return timedTransitions.at(id);
            } else if(isImmediateTransitionId(id)) {
                return immediateTransitions.at(id);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "No transitition with id '" << id << "' known.");
            }
        }
        
        void GspnBuilder::addNormalArc(std::string const& from, std::string const& to, uint64_t multiplicity) {
            if (placeNames.count(from) > 0 && transitionNames.count(to) > 0) {
                addInputArc(placeNames.at(from), transitionNames.at(to), multiplicity);
            } else if (transitionNames.count(from) > 0 && placeNames.count(to) > 0) {
                addOutputArc(transitionNames.at(from), placeNames.at(to), multiplicity);
            } else {
                // No suitable combination. Provide error message:
                if (placeNames.count(from) > 0) {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expected a transition with name " << to << " for arc from '" << from <<  "' to '" << to << "'.");
                }
                if (transitionNames.count(from) > 0) {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expected a place named " << to << " for arc from '" << from << "' to '" << to << "'.");
                }
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expected a place named " << from << " for arc from '" << from << "' to '" << to << "'.");
                
            }
        }
        
        bool GspnBuilder::isTimedTransitionId(uint64_t tid) const {
            if (tid >> 63) {
                return GSPN::transitionIdToTimedTransitionId(tid) < timedTransitions.size();
            }
            return false;
        }
        
        bool GspnBuilder::isImmediateTransitionId(uint64_t tid) const {
            if (tid >> 63) {
                return false;
            }
            return GSPN::transitionIdToImmediateTransitionId(tid) < immediateTransitions.size();
        }
        
        

        storm::gspn::GSPN* GspnBuilder::buildGspn() const {
            return new GSPN(gspnName, places, immediateTransitions, timedTransitions);
        }
    }
}
