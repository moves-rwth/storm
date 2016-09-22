#include "src/storage/jani/Edge.h"

#include "src/storage/jani/Model.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        Edge::Edge(uint64_t sourceLocationIndex, uint64_t actionIndex, boost::optional<storm::expressions::Expression> const& rate, storm::expressions::Expression const& guard, std::vector<EdgeDestination> destinations) : sourceLocationIndex(sourceLocationIndex), actionIndex(actionIndex), rate(rate), guard(guard), destinations(destinations), assignments(), writtenGlobalVariables() {
            // Intentionally left empty.
        }
        
        uint64_t Edge::getSourceLocationIndex() const {
            return sourceLocationIndex;
        }
        
        uint64_t Edge::getActionIndex() const {
            return actionIndex;
        }
        
        bool Edge::hasRate() const {
            return static_cast<bool>(rate);
        }
        
        storm::expressions::Expression const& Edge::getRate() const {
            return rate.get();
        }
        
        void Edge::setRate(storm::expressions::Expression const& rate) {
            this->rate = rate;
        }
        
        storm::expressions::Expression const& Edge::getGuard() const {
            return guard;
        }
        
        void Edge::setGuard(storm::expressions::Expression const& guard) {
            this->guard = guard;
        }
        
        std::vector<EdgeDestination> const& Edge::getDestinations() const {
            return destinations;
        }
        
        std::vector<EdgeDestination>& Edge::getDestinations() {
            return destinations;
        }
        
        void Edge::addDestination(EdgeDestination const& destination) {
            destinations.push_back(destination);
        }
        
        OrderedAssignments const& Edge::getAssignments() const {
            return assignments;
        }
        
        void Edge::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            this->setGuard(this->getGuard().substitute(substitution));
            if (this->hasRate()) {
                this->setRate(this->getRate().substitute(substitution));
            }
            for (auto& assignment : this->getAssignments()) {
                assignment.substitute(substitution);
            }
            for (auto& destination : this->getDestinations()) {
                destination.substitute(substitution);
            }
        }
        
        void Edge::finalize(Model const& containingModel) {
            for (auto const& destination : getDestinations()) {
                for (auto const& assignment : destination.getOrderedAssignments().getAllAssignments()) {
                    if (containingModel.getGlobalVariables().hasVariable(assignment.getExpressionVariable())) {
                        writtenGlobalVariables.insert(assignment.getExpressionVariable());
                    }
                }
            }
        }
        
        bool Edge::hasSilentAction() const {
            return actionIndex == Model::SILENT_ACTION_INDEX;
        }
        
        bool Edge::addTransientAssignment(Assignment const& assignment) {
            STORM_LOG_THROW(assignment.isTransient(), storm::exceptions::InvalidArgumentException, "Must not add non-transient assignment to location.");
            return assignments.add(assignment);
        }
        
        void Edge::liftTransientDestinationAssignments() {
            if (!destinations.empty()) {
                auto const& destination = *destinations.begin();
                
                for (auto const& assignment : destination.getOrderedAssignments().getTransientAssignments()) {
                    // Check if we can lift the assignment to the edge.
                    bool canBeLifted = true;
                    for (auto const& destination : destinations) {
                        if (!destination.hasAssignment(assignment)) {
                            canBeLifted = false;
                            break;
                        }
                    }
                    
                    // If so, remove the assignment from all destinations.
                    if (canBeLifted) {
                        this->addTransientAssignment(assignment);
                        for (auto& destination : destinations) {
                            destination.removeAssignment(assignment);
                        }
                    }
                }
            }
        }
        
        void Edge::pushAssignmentsToDestinations() {
            assert(!destinations.empty());
            for (auto const& assignment : this->getAssignments()) {
                for (auto& destination : destinations) {
                    destination.addAssignment(assignment);
                }
            }
            assignments = OrderedAssignments();
        }
        
        boost::container::flat_set<storm::expressions::Variable> const& Edge::getWrittenGlobalVariables() const {
            return writtenGlobalVariables;
        }
        
        bool Edge::usesVariablesInNonTransientAssignments(std::set<storm::expressions::Variable> const& variables) const {
            for (auto const& destination : destinations) {
                for (auto const& assignment : destination.getOrderedAssignments().getNonTransientAssignments()) {
                    if (assignment.getAssignedExpression().containsVariable(variables)) {
                        return true;
                    }
                }
            }
            return false;
        }
    }
}
