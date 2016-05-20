#include "src/storage/jani/Automaton.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        Automaton::Automaton(std::string const& name) : name(name) {
            // Intentionally left empty.
        }
        
        std::string const& Automaton::getName() const {
            return name;
        }
        
        void Automaton::addBooleanVariable(BooleanVariable const& variable) {
            variables.addBooleanVariable(variable);
        }
        
        void Automaton::addBoundedIntegerVariable(BoundedIntegerVariable const& variable) {
            variables.addBoundedIntegerVariable(variable);
        }

        void Automaton::addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable) {
            variables.addUnboundedIntegerVariable(variable);
        }
        
        VariableSet const& Automaton::getVariableSet() const {
            return variables;
        }
        
        bool Automaton::hasLocation(std::string const& name) const {
            return locationToIndex.find(name) != locationToIndex.end();
        }
        
        std::vector<Location> const& Automaton::getLocations() const {
            return locations;
        }
        
        void Automaton::addLocation(Location const& location) {
            STORM_LOG_THROW(!this->hasLocation(location.getName()), storm::exceptions::WrongFormatException, "Cannot add location with name '" << location.getName() << "', because a location with this name already exists.");
            locationToIndex.emplace(location.getName(), locations.size());
            locations.push_back(location);
        }
        
        void Automaton::setInitialLocation(std::string const& name) {
            auto it = locationToIndex.find(name);
            STORM_LOG_THROW(it != locationToIndex.end(), storm::exceptions::InvalidArgumentException, "Cannot make unknown location '" << name << "' the initial location.");
            return setInitialLocation(it->second);
        }
        
        void Automaton::setInitialLocation(uint64_t index) {
            STORM_LOG_THROW(index < locations.size(), storm::exceptions::InvalidArgumentException, "Cannot make location with index " << index << " initial: out of bounds.");
            initialLocationIndex = index;
        }
        
        Location const& Automaton::getInitialLocation() const {
            return locations[getInitialLocationIndex()];
        }
        
        uint64_t Automaton::getInitialLocationIndex() const {
            return initialLocationIndex;
        }
        
        EdgeSet const& Automaton::getEdgesFromLocation(std::string const& name) const {
            auto it = locationToIndex.find(name);
            STORM_LOG_THROW(it != locationToIndex.end(), storm::exceptions::InvalidArgumentException, "Cannot retrieve edges from unknown location '" << name << ".");
            return getEdgesFromLocation(it->second);
        }
        
        EdgeSet const& Automaton::getEdgesFromLocation(uint64_t index) const {
            return edges[index];
        }
        
    }
}