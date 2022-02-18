#pragma

#include <memory>

namespace storm {

namespace logic {
// fwd
class Formula;
}  // namespace logic

namespace automata {
// fwd
class DeterministicAutomaton;

class LTL2DeterministicAutomaton {
   public:
    /*!
     * Converts an LTL formula into a deterministic omega-automaton using the internal LTL2DA tool "Spot".
     * The resulting DA uses transition-based acceptance and if specified the acceptance condition is converted to DNF.
     *
     * @param f The LTL formula.
     * @param dnf A Flag indicating whether the acceptance condition is transformed into DNF.
     * @return An automaton equivalent to the formula.
     */
    static std::shared_ptr<DeterministicAutomaton> ltl2daSpot(storm::logic::Formula const& f, bool dnf);

    /*!
     * Converts an LTL formula into a deterministic omega-automaton using an external LTL2DA tool.
     * The external tool must guarantee transition-based acceptance.
     * Additionally, for MDP model checking, the acceptance condition of the resulting automaton must be in DNF.
     *
     * @param f The LTL formula.
     * @param ltl2daTool The external tool.
     * @return An automaton equivalent to the formula.
     */
    static std::shared_ptr<DeterministicAutomaton> ltl2daExternalTool(storm::logic::Formula const& f, std::string ltl2daTool);
};

}  // namespace automata
}  // namespace storm
