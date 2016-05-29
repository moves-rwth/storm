#pragma once

#include <cstdint>

#include "src/storage/expressions/Expression.h"

#include "src/storage/jani/Assignment.h"
#include "src/storage/jani/RewardIncrement.h"

namespace storm {
    namespace jani {
        
        class EdgeDestination {
        public:
            /*!
             * Creates a new edge destination.
             */
            EdgeDestination(uint64_t locationId, storm::expressions::Expression const& probability, std::vector<Assignment> const& assignments = {}, std::vector<RewardIncrement> const& rewardIncrements = {});
            
            /*!
             * Additionally performs the given assignment when choosing this destination.
             */
            void addAssignment(Assignment const& assignment);

            /*!
             * Additionally performs the given reward increment when choosing this destination.
             */
            void addRewardIncrement(RewardIncrement const& rewardIncrement);
            
            /*!
             * Retrieves the id of the destination location.
             */
            uint64_t getLocationId() const;
            
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
             * Retrieves the reward increments to make when choosing this destination.
             */
            std::vector<RewardIncrement> const& getRewardIncrements() const;
            
        private:
            // The id of the destination location.
            uint64_t locationId;

            // The probability to go to the destination.
            storm::expressions::Expression probability;
            
            // The assignments to make when choosing this destination.
            std::vector<Assignment> assignments;
            
            // The increments to rewards to make when choosing this destination.
            std::vector<RewardIncrement> rewardIncrements;
        };
        
    }
}