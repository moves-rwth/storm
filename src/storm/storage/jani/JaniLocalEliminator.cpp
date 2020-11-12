#include "JaniLocalEliminator.h"
#include "JaniLocationExpander.h"
#include "storm/storage/expressions/ExpressionManager.h"

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
            newModel = original; // TODO: Make copy instead?
            makeVariablesLocal("multiplex");
            if (original.getAutomata().size() != 1){
                STORM_LOG_ERROR("State Space Reduction is only supported for Jani models with a single automaton.");
                return;
            }
            unfold("s");
        }

        Model const &JaniLocalEliminator::getResult() {
            return newModel;
        }

        void JaniLocalEliminator::unfold(const std::string &variableName) {
            JaniLocationExpander expander = JaniLocationExpander(newModel);
            expander.transform("multiplex", "s");
            newModel = expander.getResult();
        }

        void JaniLocalEliminator::eliminate(const std::string &locationName) {

        }

        void JaniLocalEliminator::eliminate_all() {

        }

        void JaniLocalEliminator::makeVariablesLocal(const std::string &automatonName) {

            Model localizedModel(newModel.getName() + "_localized", newModel.getModelType(), newModel.getJaniVersion(), newModel.getManager().shared_from_this());

            localizedModel.getModelFeatures() = newModel.getModelFeatures();

            Automaton originalAutomaton = newModel.getAutomaton(automatonName);
            Automaton copyAutomaton = Automaton(originalAutomaton);
            for (auto const& var: newModel.getGlobalVariables()) {
                copyAutomaton.addVariable(var);
            }
            localizedModel.addAutomaton(copyAutomaton);

            newModel = localizedModel;
        }
    }
}
