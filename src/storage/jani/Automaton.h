#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>

#include "src/storage/jani/VariableSet.h"
#include "src/storage/jani/Edge.h"
#include "src/storage/jani/Location.h"

namespace storm {
    namespace jani {

        class Automaton;
        
        namespace detail {
            class Edges {
            public:
                typedef std::vector<Edge>::iterator iterator;
                typedef std::vector<Edge>::const_iterator const_iterator;
                
                Edges(iterator it, iterator ite);
                
                /*!
                 * Retrieves an iterator to the edges.
                 */
                iterator begin() const;
                
                /*!
                 * Retrieves an end iterator to the edges.
                 */
                iterator end() const;
                
                /*!
                 * Determines whether this set of edges is empty.
                 */
                bool empty() const;
                
            private:
                iterator it;
                iterator ite;
            };
            
            class ConstEdges {
            public:
                typedef std::vector<Edge>::iterator iterator;
                typedef std::vector<Edge>::const_iterator const_iterator;
                
                ConstEdges(const_iterator it, const_iterator ite);
                
                /*!
                 * Retrieves an iterator to the edges.
                 */
                const_iterator begin() const;
                
                /*!
                 * Retrieves an end iterator to the edges.
                 */
                const_iterator end() const;

                /*!
                 * Determines whether this set of edges is empty.
                 */
                bool empty() const;
                
            private:
                const_iterator it;
                const_iterator ite;
            };
        }
        
        class Automaton {
        public:
            friend class detail::Edges;
            friend class detail::ConstEdges;
            
            typedef detail::Edges Edges;
            typedef detail::ConstEdges ConstEdges;
            
            /*!
             * Creates an empty automaton.
             */
            Automaton(std::string const& name);
            
            /*!
             * Retrieves the name of the automaton.
             */
            std::string const& getName() const;
            
            /*!
             * Adds the given boolean variable to this automaton.
             */
            void addBooleanVariable(BooleanVariable const& variable);
            
            /*!
             * Adds the given bounded integer variable to this automaton.
             */
            void addBoundedIntegerVariable(BoundedIntegerVariable const& variable);
            
            /*!
             * Adds the given unbounded integer variable to this automaton.
             */
            void addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable);

            /*!
             * Retrieves the variables of this automaton.
             */
            VariableSet& getVariables();

            /*!
             * Retrieves the variables of this automaton.
             */
            VariableSet const& getVariables() const;
            
            /*!
             * Retrieves whether the automaton has a location with the given name.
             */
            bool hasLocation(std::string const& name) const;

            /*!
             * Get location id for a location with a given name.
             * Yields undefined behaviour if no such location exists;
             *
             * @name the name of the location
             */
            uint64_t getLocationId(std::string const& name) const;
            
            /*!
             * Retrieves the locations of the automaton.
             */
            std::vector<Location> const& getLocations() const;
            
            /*!
             * Retrieves the location with the given index.
             */
            Location const& getLocation(uint64_t index) const;
            
            /*!
             * Adds the given location to the automaton.
             */
            uint64_t addLocation(Location const& location);
            
            /*!
             * Uses the location with the given name as the initial location.
             */
            void setInitialLocation(std::string const& name);

            /*!
             * Uses the location with the given index as the initial location.
             */
            void setInitialLocation(uint64_t index);
            
            /*!
             * Retrieves the initial location of the automaton.
             */
            Location const& getInitialLocation() const;
            
            /*!
             * Retrieves the index of the initial location.
             */
            uint64_t getInitialLocationIndex() const;

            /*!
             * Retrieves the edges of the location with the given name.
             */
            Edges getEdgesFromLocation(std::string const& name);
            
            /*!
             * Retrieves the edges of the location with the given index.
             */
            Edges getEdgesFromLocation(uint64_t index);
            
            /*!
             * Retrieves the edges of the location with the given name.
             */
            ConstEdges getEdgesFromLocation(std::string const& name) const;

            /*!
             * Retrieves the edges of the location with the given index.
             */
            ConstEdges getEdgesFromLocation(uint64_t index) const;
            
            /*!
             * Adds an edge to the automaton.
             */
            void addEdge(Edge const& edge);
            
            /*!
             * Retrieves the edges of the automaton.
             */
            std::vector<Edge>& getEdges();

            /*!
             * Retrieves the edges of the automaton.
             */
            std::vector<Edge> const& getEdges() const;

            /*!
             * Retrieves the set of action indices that are labels of edges of this automaton.
             */
            std::set<uint64_t> getActionIndices() const;
            
            /*!
             * Retrieves the number of locations.
             */
            uint64_t getNumberOfLocations() const;

            /*!
             * Retrieves the number of edges.
             */
            uint64_t getNumberOfEdges() const;

        private:
            /// The name of the automaton.
            std::string name;

            /// The set of variables of this automaton.
            VariableSet variables;
            
            /// The locations of the automaton.
            std::vector<Location> locations;
            
            /// A mapping of location names to their indices.
            std::unordered_map<std::string, uint64_t> locationToIndex;
            
            /// All edges of the automaton
            std::vector<Edge> edges;
            
            /// A mapping from location indices to the starting indices. If l is mapped to i, it means that the edges
            /// leaving location l start at index i of the edges vector.
            std::vector<uint64_t> locationToStartingIndex;

            /// The index of the initial location.
            uint64_t initialLocationIndex;
        };
        
    }
}