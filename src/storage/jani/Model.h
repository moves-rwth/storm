#pragma once

#include "Automaton.h"

namespace storm {
    namespace jani {


        class enum JaniModelType {UNSPECIFED = 0,
                                    DTMC = 1,
                                    CTMC = 2,
                                    MDP = 3};

        class JaniModel {
            size_t janiVersion = 0;
            JaniModelType modelType;
            std::map<std::string, JaniAutomaton> automata;


            bool checkValid(bool logdbg = true) {
                if (janiVersion == 0) {
                    if(logdbg) STORM_LOG_DEBUG("Jani version is unspecified");
                    return false;
                }

                if(modelType == JaniModelType::UNSPECIFED) {
                    if(logdbg) STORM_LOG_DEBUG("Model type is unspecified");
                    return false;
                }

                if(automata.empty()) {
                    if(logdbg) STORM_LOG_DEBUG("No automata specified");
                }

            }
        };
    }
}

