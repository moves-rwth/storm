#pragma once

#include <boost/optional.hpp>

#include "src/storage/jani/EdgeDestination.h"

namespace storm {
    namespace jani {
        
        class Edge {
        public:
            Edge(uint64_t sourceLocationIndex, uint64_t actionIndex, boost::optional<storm::expressions::Expression> const& rate, storm::expressions::Expression const& guard, std::vector<EdgeDestination> destinations = {});
            
            /*!
             * Retrieves the index of the source location.
             */
            uint64_t getSourceLocationIndex() const;
            
            /*!
             * Retrieves the id of the action with which this edge is labeled.
             */
            uint64_t getActionIndex() const;
            
            /*!
             * Retrieves whether this edge has an associated rate.
             */
            bool hasRate() const;
            
            /*!
             * Retrieves the rate of this edge. Note that calling this is only valid if the edge has an associated rate.
             */
            storm::expressions::Expression const& getRate() const;

            /*!
             * Sets a new rate for this edge.
             */
            void setRate(storm::expressions::Expression const& rate);
            
            /*!
             * Retrieves the guard of this edge.
             */
            storm::expressions::Expression const& getGuard() const;
            
            /*!
             * Sets a new guard for this edge.
             */
            void setGuard(storm::expressions::Expression const& guard);
            
            /*!
             * Retrieves the destinations of this edge.
             */
            std::vector<EdgeDestination> const& getDestinations() const;

            /*!
             * Retrieves the destinations of this edge.
             */
            std::vector<EdgeDestination>& getDestinations();
            
            /*!
             * Adds the given destination to the destinations of this edge.
             */
            void addDestination(EdgeDestination const& destination);
            
        private:
            // The index of the source location.
            uint64_t sourceLocationIndex;
            
            // The index of the action with which this edge is labeled.
            uint64_t actionIndex;
            
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