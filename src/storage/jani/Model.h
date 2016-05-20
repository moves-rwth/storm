#pragma once

#include "Automaton.h"
#include "src/utility/macros.h"


namespace storm {
    namespace jani {


         enum class JaniModelType {UNSPECIFED = 0,
                                    DTMC = 1,
                                    CTMC = 2,
                                    MDP = 3};

        class Model {
            size_t janiVersion = 0;
            JaniModelType modelType;
            std::map<std::string, Automaton> automata;

        public:
            /*!
             *  Does some simple checks to determine whether the model is supported by Prism.
             *  Mainly checks abscence of features the parser supports.
             *
             *  Throws UnsupportedModelException if something is wrong
             */
            // TODO add engine as argument to check this for several engines.
            void checkSupported() {

            }

            /*!
             *  Checks if the model is valid JANI, which should be verified before any further operations are applied to a model.
             */
            bool checkValidity(bool logdbg = true) {
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
                    return false;
                }

            }
        };
    }
}

