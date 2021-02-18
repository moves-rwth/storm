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
                        std::map<std::string, bool> isInitialOrPropMap;
                        std::map<std::string, bool> alreadyEliminated;
                        for (auto loc : automaton.getLocations()) {
                            bool isInitialOrProp = (session.isPossiblyInitial(automatonName, loc.getName())
                                                    || session.isPartOfProp(automatonName, loc.getName()));
                            isInitialOrPropMap.insert(std::pair<std::string, bool>(loc.getName(), isInitialOrProp));
                            alreadyEliminated.insert(std::pair<std::string, bool>(loc.getName(), false));
                        }

                        bool done = false;
                        while (!done) {
                            int minNewEdges = transitionCountThreshold;
                            int bestLocIndex = -1;
                            for (auto loc : automaton.getLocations()) {
                                if (isInitialOrPropMap[loc.getName()] || alreadyEliminated[loc.getName()])
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
                            } else {
                                std::string locName = automaton.getLocation(bestLocIndex).getName();
                                std::cout << "Unfolding location " << locName << std::endl;
                                EliminateAction action = EliminateAction(automatonName, locName);
                                action.doAction(session);
                                automaton = session.getModel().getAutomaton(automatonName);
                                isInitialOrPropMap[locName] = true;
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