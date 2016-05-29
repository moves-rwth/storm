#include "src/storage/jani/EdgeDestination.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace jani {
        
        EdgeDestination::EdgeDestination(uint64_t locationId, storm::expressions::Expression const& probability, std::vector<Assignment> const& assignments, std::vector<RewardIncrement> const& rewardIncrements) : locationId(locationId), probability(probability), assignments(assignments), rewardIncrements(rewardIncrements) {
            // Intentionally left empty.
        }
        
        void EdgeDestination::addAssignment(Assignment const& assignment) {
            // We make sure that there are no two assignments to the same variable.
            for (auto const& oldAssignment : assignments) {
                STORM_LOG_THROW(oldAssignment.getExpressionVariable() != assignment.getExpressionVariable(), storm::exceptions::WrongFormatException, "Cannot add assignment '" << assignment << "', because another assignment '" << assignment << "' writes to the same target variable.");
            }
            assignments.push_back(assignment);
        }
        
        void EdgeDestination::addRewardIncrement(RewardIncrement const& rewardIncrement) {
            rewardIncrements.push_back(rewardIncrement);
        }
        
        uint64_t EdgeDestination::getLocationId() const {
            return locationId;
        }
        
        storm::expressions::Expression const& EdgeDestination::getProbability() const {
            return probability;
        }
        
        void EdgeDestination::setProbability(storm::expressions::Expression const& probability) {
            this->probability = probability;
        }
        
        std::vector<Assignment>& EdgeDestination::getAssignments() {
            return assignments;
        }
        
        std::vector<Assignment> const& EdgeDestination::getAssignments() const {
            return assignments;
        }
        
        std::vector<RewardIncrement> const& EdgeDestination::getRewardIncrements() const {
            return rewardIncrements;
        }
        
    }
}