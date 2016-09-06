#include "src/storage/jani/Edge.h"

#include "src/storage/jani/Model.h"

namespace storm {
    namespace jani {
        
        Edge::Edge(uint64_t sourceLocationIndex, uint64_t actionIndex, boost::optional<storm::expressions::Expression> const& rate, storm::expressions::Expression const& guard, std::vector<EdgeDestination> destinations) : sourceLocationIndex(sourceLocationIndex), actionIndex(actionIndex), rate(rate), guard(guard), destinations(destinations) {
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
        
        std::vector<Assignment>& Edge::getTransientAssignments() {
            return transientAssignments;
        }
        
        std::vector<Assignment> const& Edge::getTransientAssignments() const {
            return transientAssignments;
        }
        
        void Edge::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            this->setGuard(this->getGuard().substitute(substitution));
            if (this->hasRate()) {
                this->setRate(this->getRate().substitute(substitution));
            }
            for (auto& assignment : this->getTransientAssignments()) {
                assignment.substitute(substitution);
            }
            for (auto& destination : this->getDestinations()) {
                destination.substitute(substitution);
            }
        }
        
        void Edge::finalize(Model const& containingModel) {
            for (auto const& destination : getDestinations()) {
                for (auto const& assignment : destination.getAssignments()) {
                    if (containingModel.getGlobalVariables().hasVariable(assignment.getExpressionVariable())) {
                        writtenGlobalVariables.insert(assignment.getExpressionVariable());
                    }
                }
            }
        }
        
        void Edge::addTransientAssignment(Assignment const& assignment) {
            transientAssignments.push_back(assignment);
        }
        
        void Edge::liftTransientDestinationAssignments() {
            if (!destinations.empty()) {
                auto const& destination = *destinations.begin();
                
                for (auto const& assignment : destination.getTransientAssignments()) {
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
        
        boost::container::flat_set<storm::expressions::Variable> const& Edge::getWrittenGlobalVariables() const {
            return writtenGlobalVariables;
        }
    }
}