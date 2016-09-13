#include "src/storage/jani/EdgeDestination.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace jani {
        
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
        
        storm::jani::detail::ConstAssignments EdgeDestination::getAssignments() const {
            return assignments.getAllAssignments();
        }

        storm::jani::detail::ConstAssignments EdgeDestination::getTransientAssignments() const {
            return assignments.getTransientAssignments();
        }
        
        storm::jani::detail::ConstAssignments EdgeDestination::getNonTransientAssignments() const {
            return assignments.getNonTransientAssignments();
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
        
    }
}