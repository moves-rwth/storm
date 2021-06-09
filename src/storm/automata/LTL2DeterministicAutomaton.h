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
            static std::shared_ptr<DeterministicAutomaton> ltl2da(storm::logic::Formula const&, bool dnf);

        private:
            static bool isExternalDaToolSet();

            static std::shared_ptr<DeterministicAutomaton> ltl2daInternalTool(std::string const& prefixLtl, bool dnf);
            static std::shared_ptr<DeterministicAutomaton> ltl2daExternalTool(std::string const& prefixLtl);
        };

    }
}
