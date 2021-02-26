#include "EliminateAutomaticallyAction.h"
#include "EliminateAction.h"

namespace storm {
    namespace jani {
        namespace elimination_actions {
            EliminateAutomaticallyAction::EliminateAutomaticallyAction(const std::string &automatonName, EliminateAutomaticallyAction::EliminationOrder order, uint32_t transitionCountThreshold)
                    : automatonName(automatonName),
                      eliminationOrder(order),
                      transitionCountThreshold(transitionCountThreshold){
            }

            std::string EliminateAutomaticallyAction::getDescription() {
                return "EliminateAutomaticallyAction";
            }

            void EliminateAutomaticallyAction::doAction(JaniLocalEliminator::Session &session) {
                Automaton &automaton = session.getModel().getAutomaton(automatonName);
                switch (eliminationOrder){
                    case EliminationOrder::Arbitrary: {
                        for (auto loc : automaton.getLocations()) {
                            if (session.isEliminable(automatonName, loc.getName())) {
                                std::cout << "Unfolding location " << loc.getName() << std::endl;
                                EliminateAction action = EliminateAction(automatonName, loc.getName());
                                action.doAction(session);
                            }
                        }
                        break;
                    }
                    case EliminationOrder::NewTransitionCount: {

                        // A location is uneliminable if
                        // - it is initial
                        // - it (potentially) satisfies the property
                        // - it has already been eliminated
                        // - it has a loop
                        // After each elimination, this list is updated to account for new loops
                        std::map<std::string, bool> uneliminable;
                        for (auto loc : automaton.getLocations()) {
                            bool isUneliminable = session.isPossiblyInitial(automatonName, loc.getName())
                                                    || session.isPartOfProp(automatonName, loc.getName())
                                                    || session.hasLoops(automatonName, loc.getName());
                            uneliminable.insert(std::pair<std::string, bool>(loc.getName(), isUneliminable));
                        }

                        bool done = false;
                        while (!done) {
                            int minNewEdges = INT_MAX;
                            int bestLocIndex = -1;
                            for (auto loc : automaton.getLocations()) {
                                if (uneliminable[loc.getName()])
                                    continue;

                                int locIndex = automaton.getLocationIndex(loc.getName());
                                int outgoing = automaton.getEdgesFromLocation(locIndex).size();
                                int incoming = 0;
                                for (auto edge : automaton.getEdges()) {
                                    int addedTransitions = 1;
                                    for (auto dest : edge.getDestinations())
                                        if (dest.getLocationIndex() == locIndex)
                                            addedTransitions *= outgoing;
                                    incoming += addedTransitions;
                                }
                                int newEdges = incoming * outgoing;
                                if (newEdges <= minNewEdges){
                                    minNewEdges = newEdges;
                                    bestLocIndex = locIndex;
                                }
                            }


                            if (bestLocIndex == -1){
                                done = true;
                                session.addToLog("Cannot eliminate more locations");
                            } else if (minNewEdges > transitionCountThreshold) {
                                done = true;
                                session.addToLog(
                                        "Cannot eliminate more locations without creating too many new transitions (best: " +
                                        std::to_string(minNewEdges) + "new transitions)");
                            } else {
                                std::string locName = automaton.getLocation(bestLocIndex).getName();
                                session.addToLog("Eliminating location " + locName);
                                EliminateAction action = EliminateAction(automatonName, locName);
                                action.doAction(session);
                                automaton = session.getModel().getAutomaton(automatonName);
                                uneliminable[locName] = true;

                                // Update "uneliminable" to account for potential new loops
                                for (const auto& loc : automaton.getLocations()) {
                                    if (!uneliminable[locName]){
                                        if (session.hasLoops(automatonName, loc.getName())){
                                            uneliminable[locName] = true;
                                        }
                                    }
                                }
                            }
                        }

                        break;
                    }
                    default: {
                        STORM_LOG_THROW(true, storm::exceptions::NotImplementedException, "This elimination order is not yet implemented");
                        break;
                    }
                }
            }

            std::string EliminateAutomaticallyAction::find_next_location(JaniLocalEliminator::Session &session) {
                return "";
            }
        }
    }
}