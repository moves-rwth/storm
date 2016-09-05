#include "src/storage/jani/EdgeDestination.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace jani {
        
        EdgeDestination::EdgeDestination(uint64_t locationIndex, storm::expressions::Expression const& probability, std::vector<Assignment> const& assignments) : locationIndex(locationIndex), probability(probability), assignments(assignments) {
            for (auto const& assignment : assignments) {
                if (assignment.isTransientAssignment()) {
                    transientAssignments.push_back(assignment);
                } else {
                    nonTransientAssignments.push_back(assignment);
                }
            }
            sortAssignments(this->assignments);
            sortAssignments(transientAssignments);
            sortAssignments(nonTransientAssignments);
        }
        
        void EdgeDestination::addAssignment(Assignment const& assignment) {
            // We make sure that there are no two assignments to the same variable.
            for (auto const& oldAssignment : assignments) {
                STORM_LOG_THROW(oldAssignment.getExpressionVariable() != assignment.getExpressionVariable(), storm::exceptions::WrongFormatException, "Cannot add assignment '" << assignment << "', because another assignment '" << assignment << "' writes to the same target variable.");
            }
            assignments.push_back(assignment);
            sortAssignments(assignments);
            
            if (assignment.isTransientAssignment()) {
                transientAssignments.push_back(assignment);
                sortAssignments(transientAssignments);
            } else {
                nonTransientAssignments.push_back(assignment);
                sortAssignments(nonTransientAssignments);
            }
        }
        
        uint64_t EdgeDestination::getLocationIndex() const {
            return locationIndex;
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
        
        std::vector<Assignment>& EdgeDestination::getNonTransientAssignments() {
            return nonTransientAssignments;
        }
        
        std::vector<Assignment> const& EdgeDestination::getNonTransientAssignments() const {
            return nonTransientAssignments;
        }
        
        std::vector<Assignment>& EdgeDestination::getTransientAssignments() {
            return transientAssignments;
        }
        
        std::vector<Assignment> const& EdgeDestination::getTransientAssignments() const {
            return transientAssignments;
        }
        
        void EdgeDestination::sortAssignments(std::vector<Assignment>& assignments) {
            std::sort(assignments.begin(), assignments.end(), [] (storm::jani::Assignment const& assignment1, storm::jani::Assignment const& assignment2) {
                bool smaller = assignment1.getExpressionVariable().getType().isBooleanType() && !assignment2.getExpressionVariable().getType().isBooleanType();
                if (!smaller) {
                    smaller = assignment1.getExpressionVariable() < assignment2.getExpressionVariable();
                }
                return smaller;
            });
        }
        
    }
}