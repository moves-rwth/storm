#pragma once

#include <cstdint>

#include "src/storage/expressions/Expression.h"

#include "src/storage/jani/OrderedAssignments.h"

namespace storm {
    namespace jani {
        
        class EdgeDestination {
        public:
            /*!
             * Creates a new edge destination.
             */
            EdgeDestination(uint64_t locationIndex, storm::expressions::Expression const& probability, std::vector<Assignment> const& assignments = {});
            
            /*!
             * Additionally performs the given assignment when choosing this destination.
             */
            void addAssignment(Assignment const& assignment);
            
            /*!
             * Retrieves the id of the destination location.
             */
            uint64_t getLocationIndex() const;
            
            /*!
             * Retrieves the probability of choosing this destination.
             */
            storm::expressions::Expression const& getProbability() const;
            
            /*!
             * Sets a new probability for this edge destination.
             */
            void setProbability(storm::expressions::Expression const& probability);

            /*!
             * Retrieves the assignments to make when choosing this destination.
             */
            storm::jani::detail::ConstAssignments getAssignments() const;
            
            /*!
             * Retrieves the transient assignments to make when choosing this destination.
             */
            storm::jani::detail::ConstAssignments getTransientAssignments() const;

            /*!
             * Retrieves the non-transient assignments to make when choosing this destination.
             */
            storm::jani::detail::ConstAssignments getNonTransientAssignments() const;

            /*!
             * Substitutes all variables in all expressions according to the given substitution.
             */
            void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
            
            // Convenience methods to access the assignments.
            bool hasAssignment(Assignment const& assignment) const;
            bool removeAssignment(Assignment const& assignment);
            
        private:
            // The index of the destination location.
            uint64_t locationIndex;

            // The probability to go to the destination.
            storm::expressions::Expression probability;
            
            // The (ordered) assignments to make when choosing this destination.
            OrderedAssignments assignments;
        };
        
    }
}