#include "src/storage/jani/Automaton.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        namespace detail {
            EdgeIterator::EdgeIterator(Automaton& automaton, outer_iter out_it, outer_iter out_ite, inner_iter in_it) : automaton(automaton), out_it(out_it), out_ite(out_ite), in_it(in_it) {
                // Intentionally left empty.
            }
            
            EdgeIterator& EdgeIterator::operator++() {
                incrementIterator();
                return *this;
            }
            
            EdgeIterator& EdgeIterator::operator++(int) {
                incrementIterator();
                return *this;
            }
            
            Edge& EdgeIterator::operator*() {
                return *in_it;
            }
            
            bool EdgeIterator::operator==(EdgeIterator const& other) const {
                return this->out_it == other.out_it && this->in_it == other.in_it;
            }
            
            bool EdgeIterator::operator!=(EdgeIterator const& other) const {
                return !(*this == other);
            }
            
            void EdgeIterator::incrementIterator() {
                ++in_it;
                
                // If the inner iterator has reached its end move it to the beginning of the next outer element.
                if (in_it == out_it->end()) {
                    ++out_it;
                    while (out_it != out_ite && out_it->empty()) {
                        ++out_it;
                        in_it = out_it->end();
                    }
                    if (out_it != out_ite) {
                        in_it = out_it->begin();
                    }
                }
            }
            
            ConstEdgeIterator::ConstEdgeIterator(Automaton const& automaton, outer_iter out_it, outer_iter out_ite, inner_iter in_it) : automaton(automaton), out_it(out_it), out_ite(out_ite), in_it(in_it) {
                // Intentionally left empty.
            }
            
            ConstEdgeIterator& ConstEdgeIterator::operator++() {
                incrementIterator();
                return *this;
            }
            
            ConstEdgeIterator& ConstEdgeIterator::operator++(int) {
                incrementIterator();
                return *this;
            }
            
            Edge const& ConstEdgeIterator::operator*() const {
                return *in_it;
            }
            
            bool ConstEdgeIterator::operator==(ConstEdgeIterator const& other) const {
                return this->out_it == other.out_it && this->in_it == other.in_it;
            }
            
            bool ConstEdgeIterator::operator!=(ConstEdgeIterator const& other) const {
                return !(*this == other);
            }
            
            void ConstEdgeIterator::incrementIterator() {
                ++in_it;
                
                // If the inner iterator has reached its end move it to the beginning of the next outer element.
                if (in_it == out_it->end()) {
                    ++out_it;
                    while (out_it != out_ite && out_it->empty()) {
                        ++out_it;
                        in_it = out_it->end();
                    }
                    if (out_it != out_ite) {
                        in_it = out_it->begin();
                    }
                }
            }
            
            Edges::Edges(Automaton& automaton) : automaton(automaton) {
                // Intentionally left empty.
            }
            
            EdgeIterator Edges::begin() {
                auto outer = automaton.edges.begin();
                while (outer != automaton.edges.end() && outer->empty()) {
                    ++outer;
                }
                if (outer == automaton.edges.end()) {
                    return end();
                } else {
                    return EdgeIterator(automaton, outer, automaton.edges.end(), outer->begin());
                }
            }
            
            EdgeIterator Edges::end() {
                return EdgeIterator(automaton, automaton.edges.end(), automaton.edges.end(), automaton.edges.back().end());
            }
            
            ConstEdges::ConstEdges(Automaton const& automaton) : automaton(automaton) {
                // Intentionally left empty.
            }
            
            ConstEdgeIterator ConstEdges::begin() const {
                auto outer = automaton.edges.begin();
                while (outer != automaton.edges.end() && outer->empty()) {
                    ++outer;
                }
                if (outer == automaton.edges.end()) {
                    return end();
                } else {
                    return ConstEdgeIterator(automaton, outer, automaton.edges.end(), outer->begin());
                }
            }
            
            ConstEdgeIterator ConstEdges::end() const {
                return ConstEdgeIterator(automaton, automaton.edges.end(), automaton.edges.end(), automaton.edges.back().end());
            }
        }
        
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
            edges.push_back(EdgeSet());
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
        
        EdgeSet const& Automaton::getEdgesFromLocation(std::string const& name) const {
            auto it = locationToIndex.find(name);
            STORM_LOG_THROW(it != locationToIndex.end(), storm::exceptions::InvalidArgumentException, "Cannot retrieve edges from unknown location '" << name << ".");
            return getEdgesFromLocation(it->second);
        }
        
        EdgeSet const& Automaton::getEdgesFromLocation(uint64_t index) const {
            return edges[index];
        }
        
        void Automaton::addEdge(Edge const& edge) {
            STORM_LOG_THROW(edge.getSourceLocationId() < locations.size(), storm::exceptions::InvalidArgumentException, "Cannot add edge with unknown source location index '" << edge.getSourceLocationId() << "'.");
            edges[edge.getSourceLocationId()].addEdge(edge);
        }
        
        Automaton::Edges Automaton::getEdges() {
            return Edges(*this);
        }
        
        Automaton::ConstEdges Automaton::getEdges() const {
            return ConstEdges(*this);
        }
        
        uint64_t Automaton::getNumberOfLocations() const {
            return edges.size();
        }

    }
}