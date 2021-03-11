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
            static std::shared_ptr<DeterministicAutomaton> ltl2da(storm::logic::Formula const& f);
        };

    }
}
