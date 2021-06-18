#pragma

#include <memory>

namespace storm {

    namespace logic {
        // fwd
        class Formula;
    }

    namespace automata {
        // fwd
        class DeterministicAutomaton;

        class LTL2DeterministicAutomaton {
        public:

            /*!
             * TODO
             * @param f
             * @param dnf
             * @return
             */
            static std::shared_ptr<DeterministicAutomaton> ltl2daSpot(storm::logic::Formula const& f, bool dnf);

            /*!
             * TODO
             * @param f
             * @param ltl2daTool
             * @return
             */
            static std::shared_ptr<DeterministicAutomaton> ltl2daExternalTool(storm::logic::Formula const& f, std::string ltl2daTool);

        };

    }
}
