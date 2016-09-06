#pragma once

#include <cstdint>

#include "src/storage/expressions/Expression.h"

#include "src/storage/jani/Assignment.h"

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
            std::vector<Assignment>& getAssignments();
            
            /*!
             * Retrieves the assignments to make when choosing this destination.
             */
            std::vector<Assignment> const& getAssignments() const;
            
            /*!
             * Retrieves the non-transient assignments to make when choosing this destination.
             */
            std::vector<Assignment>& getNonTransientAssignments();

            /*!
             * Retrieves the non-transient assignments to make when choosing this destination.
             */
            std::vector<Assignment> const& getNonTransientAssignments() const;

            /*!
             * Retrieves the non-transient assignments to make when choosing this destination.
             */
            std::vector<Assignment>& getTransientAssignments();
            
            /*!
             * Retrieves the non-transient assignments to make when choosing this destination.
             */
            std::vector<Assignment> const& getTransientAssignments() const;

            /*!
             * Substitutes all variables in all expressions according to the given substitution.
             */
            void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
            
            /*!
             * Retrieves whether this assignment is made when choosing this destination.
             *
             * @return True iff the assignment is made.
             */
            bool hasAssignment(Assignment const& assignment) const;
            
            /*!
             * Removes the given assignment from this destination.
             *
             * @return True if the assignment was found and removed.
             */
            bool removeAssignment(Assignment const& assignment);
            
        private:
            /*!
             * Sorts the assignments to make all assignments to boolean variables precede all others and order the
             * assignments within one variable group by the expression variables.
             */
            static void sortAssignments(std::vector<Assignment>& assignments);
            
            /*!
             * Removes the given assignment from the provided list of assignments if found.
             *
             * @return True if the assignment was removed.
             */
            static bool removeAssignment(Assignment const& assignment, std::vector<Assignment>& assignments);
            
            // The index of the destination location.
            uint64_t locationIndex;

            // The probability to go to the destination.
            storm::expressions::Expression probability;
            
            // The assignments to make when choosing this destination.
            std::vector<Assignment> assignments;

            // The assignments to make when choosing this destination.
            std::vector<Assignment> nonTransientAssignments;

            // The assignments to make when choosing this destination.
            std::vector<Assignment> transientAssignments;

        };
        
    }
}