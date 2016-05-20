#pragma once

#include <boost/optional.hpp>

#include "src/storage/jani/EdgeDestination.h"

namespace storm {
    namespace jani {
        
        class Edge {
        public:
            Edge(uint64_t sourceLocationId, uint64_t actionId, boost::optional<storm::expressions::Expression> const& rate, storm::expressions::Expression const& guard, std::vector<EdgeDestination> destinations = {});
            
            /*!
             * Retrieves the id of the source location.
             */
            uint64_t getSourceLocationId() const;
            
            /*!
             * Retrieves the id of the action with which this edge is labeled.
             */
            uint64_t getActionId() const;
            
            /*!
             * Retrieves whether this edge has an associated rate.
             */
            bool hasRate() const;
            
            /*!
             * Retrieves the rate of this edge. Note that calling this is only valid if the edge has an associated rate.
             */
            storm::expressions::Expression const& getRate() const;
            
            /*!
             * Retrieves the guard of this edge.
             */
            storm::expressions::Expression const& getGuard() const;
            
            /*!
             * Retrieves the destinations of this edge.
             */
            std::vector<EdgeDestination> const& getDestinations() const;
            
            /*!
             * Adds the given destination to the destinations of this edge.
             */
            void addDestination(EdgeDestination const& destination);
            
        private:
            // The id of the source location.
            uint64_t sourceLocationId;
            
            // The id of the action with which this edge is labeled.
            uint64_t actionId;
            
            // The rate with which this edge is taken. This only applies to continuous-time models. For discrete-time
            // models, this must be set to none.
            boost::optional<storm::expressions::Expression> rate;
            
            // The guard that defines when this edge is enabled.
            storm::expressions::Expression guard;
            
            // The destinations of this edge.
            std::vector<EdgeDestination> destinations;
        };
        
    }
}