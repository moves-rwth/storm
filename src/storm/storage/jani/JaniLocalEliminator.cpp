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
            // makeVariablesLocal("multiplex");
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
            if (!newModel.getSystemComposition().isAutomatonComposition())
            {
                STORM_LOG_ERROR("Only automaton compositions are currently supported for state reduction");
                return;
            }
            auto comp = newModel.getSystemComposition().asAutomatonComposition();
            localizedModel.setSystemComposition(std::make_shared<AutomatonComposition>(comp)); //TODO: Is this shared_ptr counstruction correct?

            Automaton originalAutomaton = newModel.getAutomaton(automatonName);
            Automaton copyAutomaton = Automaton(originalAutomaton);
            for (auto const& var: newModel.getGlobalVariables()) {
                copyAutomaton.addVariable(var);
                // localizedModel.addVariable(var);
            }

            for (auto const& c : newModel.getConstants()){
                localizedModel.addConstant(c);
            }
            localizedModel.addAutomaton(copyAutomaton);

            newModel = localizedModel;
        }

        void JaniLocalEliminator::makeVariablesGlobal(const std::string &automatonName) {
            Automaton originalAutomaton = newModel.getAutomaton(automatonName);
            for (auto const& var : originalAutomaton.getVariables()){
                newModel.addVariable(var);
            }
        }
    }
}
