#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>

#include "src/storage/jani/VariableSet.h"
#include "src/storage/jani/EdgeSet.h"
#include "src/storage/jani/Location.h"

namespace storm {
    namespace jani {

        class Automaton {
        public:
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
            EdgeSet const& getEdgesFromLocation(std::string const& name) const;

            /*!
             * Retrieves the edges of the location with the given index.
             */
            EdgeSet const& getEdgesFromLocation(uint64_t index) const;
            
            /*!
             * Adds an edge to the automaton.
             */
            void addEdge(Edge const& edge);
            
            /*!
             * Retrieves the number of locations.
             */
            uint64_t getNumberOfLocations() const;
            
        private:
            // The name of the automaton.
            std::string name;

            // The set of variables of this automaton.
            VariableSet variables;
            
            // The locations of the automaton.
            std::vector<Location> locations;
            
            // A mapping of location names to their indices.
            std::unordered_map<std::string, uint64_t> locationToIndex;
            
            // All edges of the automaton. The edges at index i are the edges of the location with index i.
            std::vector<EdgeSet> edges;

            // The index of the initial location.
            uint64_t initialLocationIndex;
        };
        
    }
}