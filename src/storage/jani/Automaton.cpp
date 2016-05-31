#include "src/storage/jani/Automaton.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        namespace detail {
            Edges::Edges(iterator it, iterator ite) : it(it), ite(ite) {
                // Intentionally left empty.
            }
            
            Edges::iterator Edges::begin() const {
                return it;
            }
            
            Edges::iterator Edges::end() const {
                return ite;
            }
            
            bool Edges::empty() const {
                return it == ite;
            }
            
            ConstEdges::ConstEdges(const_iterator it, const_iterator ite) : it(it), ite(ite) {
                // Intentionally left empty.
            }
            
            ConstEdges::const_iterator ConstEdges::begin() const {
                return it;
            }
            
            ConstEdges::const_iterator ConstEdges::end() const {
                return ite;
            }

            bool ConstEdges::empty() const {
                return it == ite;
            }
        }
        
        Automaton::Automaton(std::string const& name) : name(name) {
            // Add a sentinel element to the mapping from locations to starting indices.
            locationToStartingIndex.push_back(0);
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

        VariableSet& Automaton::getVariables() {
            return variables;
        }

        VariableSet const& Automaton::getVariables() const {
            return variables;
        }
        
        bool Automaton::hasLocation(std::string const& name) const {
            return locationToIndex.find(name) != locationToIndex.end();
        }
        
        std::vector<Location> const& Automaton::getLocations() const {
            return locations;
        }
        
        Location const& Automaton::getLocation(uint64_t index) const {
            return locations[index];
        }
        
        uint64_t Automaton::addLocation(Location const& location) {
            STORM_LOG_THROW(!this->hasLocation(location.getName()), storm::exceptions::WrongFormatException, "Cannot add location with name '" << location.getName() << "', because a location with this name already exists.");
            locationToIndex.emplace(location.getName(), locations.size());
            locations.push_back(location);
            locationToStartingIndex.push_back(edges.size());
            return locations.size() - 1;
        }

        uint64_t Automaton::getLocationId(std::string const& name) const {
            assert(hasLocation(name));
            return locationToIndex.at(name);
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

        Automaton::Edges Automaton::getEdgesFromLocation(std::string const& name) {
            auto it = locationToIndex.find(name);
            STORM_LOG_THROW(it != locationToIndex.end(), storm::exceptions::InvalidArgumentException, "Cannot retrieve edges from unknown location '" << name << ".");
            return getEdgesFromLocation(it->second);
        }
        
        Automaton::Edges Automaton::getEdgesFromLocation(uint64_t index) {
            auto it = edges.begin();
            std::advance(it, locationToStartingIndex[index]);
            auto ite = edges.begin();
            std::advance(ite, locationToStartingIndex[index + 1]);
            return Edges(it, ite);
        }
        
        Automaton::ConstEdges Automaton::getEdgesFromLocation(std::string const& name) const {
            auto it = locationToIndex.find(name);
            STORM_LOG_THROW(it != locationToIndex.end(), storm::exceptions::InvalidArgumentException, "Cannot retrieve edges from unknown location '" << name << ".");
            return getEdgesFromLocation(it->second);
        }
        
        Automaton::ConstEdges Automaton::getEdgesFromLocation(uint64_t index) const {
            auto it = edges.begin();
            std::advance(it, locationToStartingIndex[index]);
            auto ite = edges.begin();
            std::advance(ite, locationToStartingIndex[index + 1]);
            return ConstEdges(it, ite);
        }
        
        void Automaton::addEdge(Edge const& edge) {
            STORM_LOG_THROW(edge.getSourceLocationId() < locations.size(), storm::exceptions::InvalidArgumentException, "Cannot add edge with unknown source location index '" << edge.getSourceLocationId() << "'.");
            
            // Find the right position for the edge and insert it properly.
            auto posIt = edges.begin();
            std::advance(posIt, locationToStartingIndex[edge.getSourceLocationId() + 1]);
            edges.insert(posIt, edge);
            
            // Now update the starting indices of all subsequent locations.
            for (uint64_t locationIndex = edge.getSourceLocationId() + 1; locationIndex < locationToStartingIndex.size(); ++locationIndex) {
                ++locationToStartingIndex[locationIndex];
            }
        }
        
        std::vector<Edge>& Automaton::getEdges() {
            return edges;
        }
        
        std::vector<Edge> const& Automaton::getEdges() const {
            return edges;
        }
        
        std::set<uint64_t> Automaton::getActionIndices() const {
            std::set<uint64_t> result;
            for (auto const& edge : edges) {
                result.insert(edge.getActionId());
            }
            return result;
        }
        
        uint64_t Automaton::getNumberOfLocations() const {
            return locations.size();
        }
        
        uint64_t Automaton::getNumberOfEdges() const {
            return edges.size();
        }

    }
}