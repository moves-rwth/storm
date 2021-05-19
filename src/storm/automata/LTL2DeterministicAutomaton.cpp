#include "storm/automata/LTL2DeterministicAutomaton.h"
#include "storm/automata/DeterministicAutomaton.h"

#include "storm/logic/Formula.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/ExpressionEvaluationException.h"
#include "storm/exceptions/WrongFormatException.h"

#ifdef STORM_HAVE_SPOT
#include "spot/tl/formula.hh"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/translate.hh"
#include "spot/twaalgos/hoa.hh"
#include "spot/twaalgos/totgba.hh"
#include "spot/twaalgos/toparity.hh"
#include "spot/twa/acc.hh"
#endif

namespace storm {
    namespace automata {

        std::shared_ptr<DeterministicAutomaton> LTL2DeterministicAutomaton::ltl2da(storm::logic::Formula const& f) {
#ifdef STORM_HAVE_SPOT

            std::string prefixLtl = f.toPrefixString();
            spot::parsed_formula spotPrefixLtl = spot::parse_prefix_ltl(prefixLtl);
            if(!spotPrefixLtl.errors.empty()){
                std::ostringstream errorMsg;
                spotPrefixLtl.format_errors(errorMsg);
                STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Spot could not parse formula: " << prefixLtl << ": " << errorMsg.str());
            }
            spot::formula spotFormula = spotPrefixLtl.f;


            // Request a deterministic, complete automaton with state-based acceptance
            spot::translator trans = spot::translator();
            trans.set_type(spot::postprocessor::Generic);
            trans.set_pref(spot::postprocessor::Deterministic | spot::postprocessor::SBAcc | spot::postprocessor::Complete);
            STORM_LOG_INFO("Construct deterministic automaton for "<< spotFormula);
            auto aut = trans.run(spotFormula);

            // TODO necessary for MDP LTL-MC
            if(!(aut->get_acceptance().is_dnf())){
                aut->set_acceptance(aut->get_acceptance().to_dnf());
            }

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
