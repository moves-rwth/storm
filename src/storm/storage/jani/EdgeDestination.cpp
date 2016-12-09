#include "storm/storage/jani/EdgeDestination.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/WrongFormatException.h"

namespace storm {
    namespace jani {
        
        EdgeDestination::EdgeDestination(uint64_t locationIndex, storm::expressions::Expression const& probability, OrderedAssignments const& assignments) : locationIndex(locationIndex), probability(probability), assignments(assignments) {
            // Intentionally left empty.
        }

        EdgeDestination::EdgeDestination(uint64_t locationIndex, storm::expressions::Expression const& probability, Assignment const& assignments) : locationIndex(locationIndex), probability(probability), assignments(assignments) {
            // Intentionally left empty.
        }

        EdgeDestination::EdgeDestination(uint64_t locationIndex, storm::expressions::Expression const& probability, std::vector<Assignment> const& assignments) : locationIndex(locationIndex), probability(probability), assignments(assignments) {
            // Intentionally left empty.
        }
        
        void EdgeDestination::addAssignment(Assignment const& assignment) {
            assignments.add(assignment);
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
        
        std::map<storm::expressions::Variable, storm::expressions::Expression> EdgeDestination::getAsVariableToExpressionMap() const {
            std::map<storm::expressions::Variable, storm::expressions::Expression> result;
            
            for (auto const& assignment : this->getOrderedAssignments()) {
                result[assignment.getExpressionVariable()] = assignment.getAssignedExpression();
            }
            
            return result;
        }
        
        OrderedAssignments const& EdgeDestination::getOrderedAssignments() const {
            return assignments;
        }
        
        void EdgeDestination::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            this->setProbability(this->getProbability().substitute(substitution));
            assignments.substitute(substitution);
        }
        
        bool EdgeDestination::hasAssignment(Assignment const& assignment) const {
            return assignments.contains(assignment);
        }
        
        bool EdgeDestination::removeAssignment(Assignment const& assignment) {
            return assignments.remove(assignment);
        }
        
        bool EdgeDestination::hasTransientAssignment() const {
            return !assignments.getTransientAssignments().empty();
        }
        
        bool EdgeDestination::usesAssignmentLevels() const {
            if (this->getOrderedAssignments().empty()) {
                return false;
            }
            return this->getOrderedAssignments().getLowestLevel() != 0 || this->getOrderedAssignments().getHighestLevel() != 0;
        }
        
    }
}
