#include "JaniLocalEliminator.h"
#include "JaniLocationExpander.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/AutomatonComposition.h"

namespace storm {
    namespace jani {

        JaniLocalEliminator::JaniLocalEliminator(const Model &original, std::vector<storm::jani::Property>& properties) : original(original) {
            if (properties.size() != 1){
                STORM_LOG_ERROR("Local elimination only works with exactly one property");
            } else {
                property = properties[0];
            }
        }

        void JaniLocalEliminator::eliminate() {
            if (original.getAutomata().size() != 1){
                STORM_LOG_ERROR("State Space Reduction is only supported for Jani models with a single automaton.");
                return;
            }
            newModel = original; // TODO: Make copy instead?
            unfold("s");
            // eliminate()
        }

        Model const &JaniLocalEliminator::getResult() {
            return newModel;
        }

        void JaniLocalEliminator::unfold(const std::string &variableName) {
            JaniLocationExpander expander = JaniLocationExpander(newModel);
            expander.transform("multiplex", "s");
            newModel = expander.getResult();
        }

        void JaniLocalEliminator::eliminate(const std::string &automatonName, const std::string &locationName) {
            Automaton automaton = newModel.getAutomaton(automatonName);
            uint64_t locIndex = automaton.getLocationIndex(locationName);

            std::vector<std::tuple<Edge, EdgeDestination>> incomingEdges;
            for (Edge edge : automaton.getEdges()){
                for (const EdgeDestination dest : edge.getDestinations()){
                    if (dest.getLocationIndex() == locIndex){
                        incomingEdges.emplace_back(std::make_tuple(edge, dest));
                    }
                }
            }

            detail::Edges outgoingEdges = automaton.getEdgesFromLocation(locationName);
        }

        void JaniLocalEliminator::eliminate_all() {

        }
    }
}
