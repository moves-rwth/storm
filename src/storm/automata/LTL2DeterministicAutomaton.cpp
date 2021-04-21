#include "storm/automata/LTL2DeterministicAutomaton.h"
#include "storm/automata/DeterministicAutomaton.h"

#include "storm/logic/Formula.h"
#include "storm/utility/macros.h"

#ifdef STORM_HAVE_SPOT
#include "spot/tl/formula.hh"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/translate.hh"
#include "spot/twaalgos/hoa.hh"
#endif

namespace storm {
    namespace automata {

        std::shared_ptr<DeterministicAutomaton> LTL2DeterministicAutomaton::ltl2da(storm::logic::Formula const& f) {
#ifdef STORM_HAVE_SPOT
            spot::formula parsed_formula = spot::parse_formula(f.toPrefixString());

            STORM_LOG_INFO("Construct deterministic automaton for "<< parsed_formula);

            // Request a deterministic, complete automaton with state-based acceptance
            spot::translator trans = spot::translator();
            trans.set_type(spot::postprocessor::Generic);
            trans.set_pref(spot::postprocessor::Deterministic | spot::postprocessor::SBAcc | spot::postprocessor::Complete);
            auto aut = trans.run(parsed_formula);

            std::stringstream autStream;
            // Print reachable states in HOA format, implicit edges (i), state-based acceptance (s)
            spot::print_hoa(autStream, aut, "is");

            storm::automata::DeterministicAutomaton::ptr da = DeterministicAutomaton::parse(autStream);

            return da;

#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Storm is compiled without Spot support.");
#endif
        }
    }
}
