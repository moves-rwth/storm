#include "storm/storage/jani/Automaton.h"

#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/TemplateEdge.h"
#include "storm/storage/jani/Location.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidTypeException.h"

namespace storm {
    namespace jani {
        

        
        Automaton::Automaton(std::string const& name, storm::expressions::Variable const& locationExpressionVariable) : name(name), locationExpressionVariable(locationExpressionVariable) {
            // Add a sentinel element to the mapping from locations to starting indices.
            locationToStartingIndex.push_back(0);
        }
        
        std::string const& Automaton::getName() const {
            return name;
        }

        Variable const& Automaton::addVariable(Variable const &variable) {
            if (variable.isBooleanVariable()) {
                return addVariable(variable.asBooleanVariable());
            } else if (variable.isBoundedIntegerVariable()) {
                return addVariable(variable.asBoundedIntegerVariable());
            } else if (variable.isUnboundedIntegerVariable()) {
                return addVariable(variable.asUnboundedIntegerVariable());
            } else if (variable.isRealVariable()) {
                return addVariable(variable.asRealVariable());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Variable has invalid type.");
            }
        }
        
        BooleanVariable const& Automaton::addVariable(BooleanVariable const& variable) {
            return variables.addVariable(variable);
        }
        
        BoundedIntegerVariable const& Automaton::addVariable(BoundedIntegerVariable const& variable) {
            return variables.addVariable(variable);
        }

        UnboundedIntegerVariable const& Automaton::addVariable(UnboundedIntegerVariable const& variable) {
            return variables.addVariable(variable);
        }

        RealVariable const& Automaton::addVariable(RealVariable const& variable) {
            return variables.addVariable(variable);
        }

        VariableSet& Automaton::getVariables() {
            return variables;
        }

        VariableSet const& Automaton::getVariables() const {
            return variables;
        }
        
        std::set<storm::expressions::Variable> Automaton::getAllExpressionVariables() const {
            std::set<storm::expressions::Variable> result;
            for (auto const& variable : this->getVariables()) {
                result.insert(variable.getExpressionVariable());
            }
            return result;
        }
        
        bool Automaton::hasTransientVariable() const {
            return variables.hasTransientVariable();
        }
        
        bool Automaton::hasLocation(std::string const& name) const {
            return locationToIndex.find(name) != locationToIndex.end();
        }
        
        std::vector<Location> const& Automaton::getLocations() const {
            return locations;
        }

        std::vector<Location>& Automaton::getLocations() {
            return locations;
        }

        Location const& Automaton::getLocation(uint64_t index) const {
            return locations[index];
        }

        Location& Automaton::getLocation(uint64_t index) {
            return locations[index];
        }

        uint64_t Automaton::addLocation(Location const& location) {
            STORM_LOG_THROW(!this->hasLocation(location.getName()), storm::exceptions::WrongFormatException, "Cannot add location with name '" << location.getName() << "', because a location with this name already exists.");
            locationToIndex.emplace(location.getName(), locations.size());
            locations.push_back(location);
            locationToStartingIndex.push_back(edges.size());
            return locations.size() - 1;
        }

        uint64_t Automaton::getLocationIndex(std::string const& name) const {
            assert(hasLocation(name));
            return locationToIndex.at(name);
        }

        void Automaton::addInitialLocation(std::string const& name) {
            auto it = locationToIndex.find(name);
            STORM_LOG_THROW(it != locationToIndex.end(), storm::exceptions::InvalidArgumentException, "Cannot make unknown location '" << name << "' the initial location.");
            return addInitialLocation(it->second);
        }
        
        void Automaton::addInitialLocation(uint64_t index) {
            STORM_LOG_THROW(index < locations.size(), storm::exceptions::InvalidArgumentException, "Cannot make location with index " << index << " initial: out of bounds.");
            initialLocationIndices.insert(index);
        }
        
        std::set<uint64_t> const& Automaton::getInitialLocationIndices() const {
            return initialLocationIndices;
        }
        
        std::map<uint64_t, std::string> Automaton::buildIdToLocationNameMap() const {
            std::map<uint64_t, std::string> mapping;
            uint64_t i = 0;
            for(auto const& loc : locations) {
                mapping[i] = loc.getName();
                ++i;
            }
            return mapping;
        }
        
        storm::expressions::Variable const& Automaton::getLocationExpressionVariable() const {
            return locationExpressionVariable;
        }

        Edge const& Automaton::getEdge(uint64_t index) const {
            return edges.getConcreteEdges()[index];
        }
        
        Automaton::Edges Automaton::getEdgesFromLocation(std::string const& name) {
            auto it = locationToIndex.find(name);
            STORM_LOG_THROW(it != locationToIndex.end(), storm::exceptions::InvalidArgumentException, "Cannot retrieve edges from unknown location '" << name << ".");
            return getEdgesFromLocation(it->second);
        }
        
        Automaton::Edges Automaton::getEdgesFromLocation(uint64_t index) {
            auto it = edges.begin();
            std::advance(it, locationToStartingIndex[index]);
            auto ite = edges.begin();
            std::advance(ite, locationToStartingIndex[index + 1]);
            return Edges(it, ite);
        }
        
        Automaton::ConstEdges Automaton::getEdgesFromLocation(std::string const& name) const {
            auto it = locationToIndex.find(name);
            STORM_LOG_THROW(it != locationToIndex.end(), storm::exceptions::InvalidArgumentException, "Cannot retrieve edges from unknown location '" << name << ".");
            return getEdgesFromLocation(it->second);
        }
        
        Automaton::ConstEdges Automaton::getEdgesFromLocation(uint64_t index) const {
            auto it = edges.begin();
            std::advance(it, locationToStartingIndex[index]);
            auto ite = edges.begin();
            std::advance(ite, locationToStartingIndex[index + 1]);
            return ConstEdges(it, ite);
        }
        
        Automaton::Edges Automaton::getEdgesFromLocation(uint64_t locationIndex, uint64_t actionIndex) {
            typedef std::vector<Edge>::iterator ForwardIt;

            // Perform binary search for start of edges with the given action index.
            auto first = edges.begin();
            std::advance(first, locationToStartingIndex[locationIndex]);
            auto last = edges.begin();
            std::advance(last, locationToStartingIndex[locationIndex + 1]);
            typename std::iterator_traits<ForwardIt>::difference_type count, step;
            count = std::distance(first, last);
            
            ForwardIt it1;
            while (count > 0) {
                it1 = first;
                step = count / 2;
                std::advance(it1, step);
                if (it1->getActionIndex() < actionIndex) {
                    first = ++it1;
                    count -= step + 1;
                }
                else {
                    count = step;
                }
            }
            it1 = first;

            // If there is no such edge, we can return now.
            if (it1 != last && it1->getActionIndex() > actionIndex) {
                return Edges(last, last);
            }
            
            // Otherwise, perform a binary search for the end of the edges with the given action index.
            count = std::distance(it1,last);
            
            ForwardIt it2;
            while (count > 0) {
                it2 = it1;
                step = count / 2;
                std::advance(it2, step);
                if (!(actionIndex < it2->getActionIndex())) {
                    first = ++it2;
                    count -= step + 1;
                } else count = step;
            }
            it2 = first;
            
            return Edges(it1, it2);
        }
        
        Automaton::ConstEdges Automaton::getEdgesFromLocation(uint64_t locationIndex, uint64_t actionIndex) const {
            typedef std::vector<Edge>::const_iterator ForwardIt;
            
            // Perform binary search for start of edges with the given action index.
            auto first = edges.begin();
            std::advance(first, locationToStartingIndex[locationIndex]);
            auto last = edges.begin();
            std::advance(last, locationToStartingIndex[locationIndex + 1]);
            typename std::iterator_traits<ForwardIt>::difference_type count, step;
            count = std::distance(first, last);
            
            ForwardIt it1;
            while (count > 0) {
                it1 = first;
                step = count / 2;
                std::advance(it1, step);
                if (it1->getActionIndex() < actionIndex) {
                    first = ++it1;
                    count -= step + 1;
                }
                else {
                    count = step;
                }
            }
            it1 = first;
            
            // If there is no such edge, we can return now.
            if (it1 != last && it1->getActionIndex() > actionIndex) {
                return ConstEdges(last, last);
            }
            
            // Otherwise, perform a binary search for the end of the edges with the given action index.
            count = std::distance(it1,last);
            
            ForwardIt it2;
            while (count > 0) {
                it2 = first;
                step = count / 2;
                std::advance(it2, step);
                if (!(actionIndex < it2->getActionIndex())) {
                    first = ++it2;
                    count -= step + 1;
                } else count = step;
            }
            it2 = first;
            
            return ConstEdges(it1, it2);
        }
        
        void Automaton::addEdge(Edge const& edge) {
            STORM_LOG_THROW(edge.getSourceLocationIndex() < locations.size(), storm::exceptions::InvalidArgumentException, "Cannot add edge with unknown source location index '" << edge.getSourceLocationIndex() << "'.");
            assert(validate());

            edges.insertEdge(edge, locationToStartingIndex[edge.getSourceLocationIndex()], locationToStartingIndex[edge.getSourceLocationIndex() + 1]);
            // Update the set of action indices of this automaton.
            actionIndices.insert(edge.getActionIndex());

            // Now update the starting indices of all subsequent locations.
            for (uint64_t locationIndex = edge.getSourceLocationIndex() + 1; locationIndex < locationToStartingIndex.size(); ++locationIndex) {
                ++locationToStartingIndex[locationIndex];
            }
            

        }
        
        std::vector<Edge>& Automaton::getEdges() {
            return edges.getConcreteEdges();
        }
        
        std::vector<Edge> const& Automaton::getEdges() const {
            return edges.getConcreteEdges();
        }
        
        std::set<uint64_t> Automaton::getActionIndices() const {
            return edges.getActionIndices();
        }
        
        uint64_t Automaton::getNumberOfLocations() const {
            return locations.size();
        }
        
        uint64_t Automaton::getNumberOfEdges() const {
            return edges.size();
        }
        
        bool Automaton::hasRestrictedInitialStates() const {
            if (!hasInitialStatesRestriction()) {
                return false;
            }
            if (getInitialStatesRestriction().containsVariables()) {
                return true;
            } else {
                return !getInitialStatesRestriction().evaluateAsBool();
            }
        }

        bool Automaton::hasInitialStatesRestriction() const {
            return initialStatesRestriction.isInitialized();
        }
        
        storm::expressions::Expression const& Automaton::getInitialStatesRestriction() const {
            return initialStatesRestriction;
        }
        
        void Automaton::setInitialStatesRestriction(storm::expressions::Expression const& initialStatesRestriction) {
            this->initialStatesRestriction = initialStatesRestriction;
        }
        
        storm::expressions::Expression Automaton::getInitialStatesExpression() const {
            storm::expressions::Expression result;
            
            // Add initial state restriction if there is one.
            if (this->hasInitialStatesRestriction()) {
                result = this->getInitialStatesRestriction();
            }
            
            // Add the expressions for all non-transient variables that have initial expressions.
            for (auto const& variable : this->getVariables()) {
                if (variable.isTransient()) {
                    continue;
                }
                
                if (variable.hasInitExpression()) {
                    storm::expressions::Expression newInitExpression = variable.isBooleanVariable() ? storm::expressions::iff(variable.getExpressionVariable(), variable.getInitExpression()) : variable.getExpressionVariable() == variable.getInitExpression();
                    if (result.isInitialized()) {
                        result = result && newInitExpression;
                    } else {
                        result = newInitExpression;
                    }
                }
            }
            
            return result;
        }
        
        bool Automaton::hasEdgeLabeledWithActionIndex(uint64_t actionIndex) const {
            return actionIndices.find(actionIndex) != actionIndices.end();
        }
        
        std::vector<storm::expressions::Expression> Automaton::getAllRangeExpressions() const {
            std::vector<storm::expressions::Expression> result;
            for (auto const& variable : this->getVariables().getBoundedIntegerVariables()) {
                result.push_back(variable.getRangeExpression());
            }
            return result;
        }
        
        void Automaton::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            for (auto& variable : this->getVariables().getBoundedIntegerVariables()) {
                variable.substitute(substitution);
            }
            
            for (auto& location : this->getLocations()) {
                location.substitute(substitution);
            }
            
            this->setInitialStatesRestriction(this->getInitialStatesRestriction().substitute(substitution));
            
            edges.substitute(substitution);
        }
        void Automaton::registerTemplateEdge(std::shared_ptr<TemplateEdge> const& te) {
            edges.insertTemplateEdge(te);
        }

        void Automaton::changeAssignmentVariables(std::map<Variable const*, std::reference_wrapper<Variable const>> const& remapping) {
            for (auto& location : locations) {
                location.changeAssignmentVariables(remapping);
            }
            edges.changeAssignmentVariables(remapping);
        }
        
        void Automaton::finalize(Model const& containingModel) {
            //simplifyIndexedAssignments();
            edges.finalize(containingModel);
        }
        
        bool Automaton::containsVariablesOnlyInProbabilitiesOrTransientAssignments(std::set<storm::expressions::Variable> const& variables) const {
            // Check initial states restriction expression.
            if (this->hasInitialStatesRestriction()) {
                if (this->getInitialStatesRestriction().containsVariable(variables)) {
                    return false;
                }
            }
            
            // Check global variable definitions.
            if (this->getVariables().containsVariablesInBoundExpressionsOrInitialValues(variables)) {
                return false;
            }
            
            // Check edges.
            for (auto const& edge : edges) {
                if (edge.usesVariablesInNonTransientAssignments(variables)) {
                    return false;
                }
            }
            
            return true;
        }
        
        void Automaton::pushEdgeAssignmentsToDestinations() {
            edges.pushAssignmentsToDestinations();
        }
        
        bool Automaton::hasTransientEdgeDestinationAssignments() const {
            for (auto const& edge : this->getEdges()) {
                if (edge.hasTransientEdgeDestinationAssignments()) {
                    return true;
                }
            }
            return false;
        }
        
        void Automaton::liftTransientEdgeDestinationAssignments() {
            edges.liftTransientDestinationAssignments();
        }

        bool Automaton::validate() const {
            assert(locationToStartingIndex.size() == locations.size() + 1);
            for(uint64_t i = 0; i < locations.size(); i++) {
                assert(locationToStartingIndex[i] <= locationToStartingIndex[i+1]);
            }
            return true;
        }


        bool Automaton::usesAssignmentLevels() const {
            return edges.usesAssignmentLevels();
        }
        
        bool Automaton::isLinear() const {
            bool result = true;

            for (auto const& location : this->getLocations()) {
                result &= location.isLinear();
            }
            if (result) {
                return edges.isLinear();
            }
            return false;
        }

        void Automaton::restrictToEdges(boost::container::flat_set<uint_fast64_t> const& edgeIndices) {
            std::vector<Edge> oldEdges = this->edges.getConcreteEdges();
            
            this->edges.clearConcreteEdges();
            actionIndices.clear();
            for (auto& e : locationToStartingIndex) {
                e = 0;
            }

            for (auto const& index : edgeIndices) {
                this->addEdge(oldEdges[index]);
            }
        }
        
        void Automaton::writeDotToStream(std::ostream& outStream, std::vector<std::string> const& actionNames) const {
            outStream << "\tsubgraph " << name << " {" << std::endl;

            // Write all locations to the stream.
            uint64_t locIndex = 0;
            for (auto const& loc : locations) {
                outStream << "\t" << name << "_s" << locIndex  << "[ label=\"" << loc.getName() << "\"];" << std::endl;
                ++locIndex;
            }
            // Write for each edge an node to the stream;
            uint64_t edgeIndex = 0;
            for (auto const& edge : edges) {
                outStream << "\t" << name << "_e" << edgeIndex << "[ label=\"\" , shape=circle, width=.2, style=filled, fillcolor=\"black\"];" << std::endl;
                ++edgeIndex;
                
                // Silencing unused variable warning.
                (void)edge;
            }

            // Connect edges
            edgeIndex = 0;
            for (auto const& edge : edges) {
                outStream << "\t" << name << "_s" << edge.getSourceLocationIndex() << " -> " << name << "_e" << edgeIndex << " [label=\"" << actionNames.at(edge.getActionIndex()) << "\"];" << std::endl;
                for (auto const& edgeDest : edge.getDestinations()) {
                    outStream << "\t" << name << "_e" << edgeIndex << " -> " << name << "_s" << edgeDest.getLocationIndex() << ";" << std::endl;
                }
                ++edgeIndex;
            }


            outStream << "\t}" << std::endl;

        }
    }
}
