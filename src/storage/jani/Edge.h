#pragma once

#include <boost/optional.hpp>
#include <boost/container/flat_set.hpp>

#include "src/storage/jani/EdgeDestination.h"
#include "src/storage/jani/OrderedAssignments.h"

namespace storm {
    namespace jani {
        
        class Model;
        
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
             * Returns whether it contains the silent action.
             */
            bool hasSilentAction() const;
            
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
            
            /*!
             * Substitutes all variables in all expressions according to the given substitution.
             */
            void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
            
            /*!
             * Finalizes the building of this edge. Subsequent changes to the edge require another call to this
             * method. Note that this method is invoked by a call to <code>finalize</code> to the containing model.
             */
            void finalize(Model const& containingModel);
            
            /*!
             * Retrieves a set of (global) variables that are written by at least one of the edge's destinations. Note
             * that prior to calling this, the edge has to be finalized.
             */
            boost::container::flat_set<storm::expressions::Variable> const& getWrittenGlobalVariables() const;

            /*!
             * Adds a transient assignment to this edge.
             *
             * @param assignment The transient assignment to add.
             * @return True if the assignment was added.
             */
            bool addTransientAssignment(Assignment const& assignment);
            
            /*!
             * Retrieves the assignments of this edge.
             */
            OrderedAssignments const& getAssignments() const;

            /*!
             * Finds the transient assignments common to all destinations and lifts them to the edge. Afterwards, these
             * assignments are no longer contained in the destination.
             */
            void liftTransientDestinationAssignments();
            
            /**
             * Shifts the assingments from the edges to the destinations.
             */
            void pushAssignmentsToDestinations();
            
            /*!
             * Checks whether the provided variables appear on the right-hand side of non-transient assignments.
             */
            bool usesVariablesInNonTransientAssignments(std::set<storm::expressions::Variable> const& variables) const;
            
        private:
            /// The index of the source location.
            uint64_t sourceLocationIndex;
            
            /// The index of the action with which this edge is labeled.
            uint64_t actionIndex;
            
            /// The rate with which this edge is taken. This only applies to continuous-time models. For discrete-time
            /// models, this must be set to none.
            boost::optional<storm::expressions::Expression> rate;
            
            /// The guard that defines when this edge is enabled.
            storm::expressions::Expression guard;
            
            /// The destinations of this edge.
            std::vector<EdgeDestination> destinations;
            
            /// The assignments made when taking this edge.
            OrderedAssignments assignments;
            
            /// A set of global variables that is written by at least one of the edge's destinations. This set is
            /// initialized by the call to <code>finalize</code>.
            boost::container::flat_set<storm::expressions::Variable> writtenGlobalVariables;
        };
        
    }
}
