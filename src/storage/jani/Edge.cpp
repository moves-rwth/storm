#include "src/storage/jani/Edge.h"

namespace storm {
    namespace jani {
        
        Edge::Edge(uint64_t sourceLocationId, uint64_t actionId, boost::optional<storm::expressions::Expression> const& rate, storm::expressions::Expression const& guard, std::vector<EdgeDestination> destinations) : sourceLocationId(sourceLocationId), actionId(actionId), rate(rate), guard(guard), destinations(destinations) {
            // Intentionally left empty.
        }
        
        uint64_t Edge::getSourceLocationId() const {
            return sourceLocationId;
        }
        
        uint64_t Edge::getActionId() const {
            return actionId;
        }
        
        bool Edge::hasRate() const {
            return static_cast<bool>(rate);
        }
        
        storm::expressions::Expression const& Edge::getRate() const {
            return rate.get();
        }
        
        void Edge::setRate(storm::expressions::Expression const& rate) {
            this->rate = rate;
        }
        
        storm::expressions::Expression const& Edge::getGuard() const {
            return guard;
        }
        
        void Edge::setGuard(storm::expressions::Expression const& guard) {
            this->guard = guard;
        }
        
        std::vector<EdgeDestination> const& Edge::getDestinations() const {
            return destinations;
        }
        
        std::vector<EdgeDestination>& Edge::getDestinations() {
            return destinations;
        }
        
        void Edge::addDestination(EdgeDestination const& destination) {
            destinations.push_back(destination);
        }
        
    }
}