#include "UnfoldAction.h"
#include <boost/format.hpp>
#include "storm/storage/jani/JaniLocationExpander.h"

namespace storm {
    namespace jani {
        namespace elimination_actions {
            UnfoldAction::UnfoldAction(const std::string &automatonName, const std::string &variableName) {
                this->automatonName = automatonName;
                this->variableName = variableName;
            }

            std::string UnfoldAction::getDescription() {
                return (boost::format("UnfoldAction (Automaton %s, Variable %s)") % automatonName % variableName).str();
            }

            void UnfoldAction::doAction(JaniLocalEliminator::Session &session) {
                // In addition to doing the unfolding, we also need to update which location might satisfy the property.
                // If a location that doesn't satisfy the property is unfolded, all resulting locations also won't
                // satisfy it. We therefore first store which old locations satisfy the property:

                std::map<uint64_t, bool> partOfProp;
                auto automaton = session.getModel().getAutomaton(automatonName);
                for (uint64_t i = 0; i < automaton.getNumberOfLocations(); i++){
                    partOfProp[i] = session.isPartOfProp(automatonName, i);
                }

                // If this is still slow, there is potential for another optimisation that might speed this up
                // if many locations are potentially part of the prop. In that case, one could iterate over all possible
                // values of the variable being unfolded and check whether the property is satisfiable for this variable
                // value. If it is *not*, then it follows that for all unfolded locations corresponding to that variable
                // value, the location does not satisfy the property.

                JaniLocationExpander expander = JaniLocationExpander(session.getModel());
                expander.transform(automatonName, variableName);
                session.setModel(expander.getResult());

                // After executing the expander, we can now determine which new locations satisfy the property.

                for (std::pair<uint64_t, std::map<int64_t, uint64_t>> oldLocMapping : expander.locationVariableValueMap) {
                    bool oldSatisfied = partOfProp[oldLocMapping.first];
                    for (std::pair<uint64_t, uint64_t> valueIndexPair : oldLocMapping.second){
                        if (oldSatisfied){
                            bool isPartOfProp = session.computeIsPartOfProp(automatonName, valueIndexPair.second);
                            session.setPartOfProp(automatonName, valueIndexPair.second, isPartOfProp);
                        } else {
                            session.setPartOfProp(automatonName, valueIndexPair.second, false);
                        }
                    }
                }
            }
        }
    }
}