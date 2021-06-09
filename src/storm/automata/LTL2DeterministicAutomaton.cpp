#include "storm/automata/LTL2DeterministicAutomaton.h"
#include "storm/automata/DeterministicAutomaton.h"

#include "storm/logic/Formula.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/ExpressionEvaluationException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/FileIoException.h"

#include <sys/wait.h>

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

        bool LTL2DeterministicAutomaton::isExternalDaToolSet() {
            return std::getenv("LTL2DA");
        }

        std::shared_ptr<DeterministicAutomaton> LTL2DeterministicAutomaton::ltl2daInternalTool(std::string const& prefixLtl, bool dnf) {
#ifdef STORM_HAVE_SPOT

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

            if(!(aut->get_acceptance().is_dnf()) && dnf){
                STORM_LOG_INFO("Convert acceptance condition "<< aut->get_acceptance() << " into DNF...");
                // Transform the acceptance condition in disjunctive normal form and merge all the Fin-sets of each clause
                aut = to_generalized_rabin(aut,true);
            }


            STORM_LOG_INFO(aut->get_acceptance());

            std::stringstream autStream;
            // Print reachable states in HOA format, implicit edges (i), state-based acceptance (s)
            spot::print_hoa(autStream, aut, "is");

            storm::automata::DeterministicAutomaton::ptr da = DeterministicAutomaton::parse(autStream);

            return da;

#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Storm is compiled without Spot support.");
#endif
        }

        // TODO: this is quite hacky, improve robustness
        std::shared_ptr<DeterministicAutomaton> LTL2DeterministicAutomaton::ltl2daExternalTool(std::string const& prefixLtl) {


            std::string ltl2da_tool = std::getenv("LTL2DA");

            STORM_LOG_INFO("Calling external LTL->DA tool:   " << ltl2da_tool << " '" << prefixLtl << "' da.hoa");

            pid_t pid;

            pid = fork();
            STORM_LOG_THROW(pid >= 0, storm::exceptions::FileIoException, "Could not construct deterministic automaton, fork failed");

            if (pid == 0) {
                // we are in the child process
                if (execlp(ltl2da_tool.c_str(), ltl2da_tool.c_str(), prefixLtl.c_str(), "da.hoa", NULL) < 0) {
                    std::cerr << "ERROR: exec failed: " << strerror(errno) << std::endl;
                    std::exit(1);
                }
                // never reached
                return std::shared_ptr<DeterministicAutomaton>();
            } else { // in the parent
                int status;

                // wait for completion
                while (wait(&status) != pid);

                int rv;
                if (WIFEXITED(status)) {
                    rv = WEXITSTATUS(status);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Could not construct deterministic automaton: process aborted");
                }
                STORM_LOG_THROW(rv == 0, storm::exceptions::FileIoException, "Could not construct deterministic automaton for " << prefixLtl << ", return code = " << rv);

                // TODO for MDP-MC the acceptance condition must be in DNF, otherwise Exception is thrown during computation if accepting ECs
                STORM_LOG_INFO("Reading automaton for " << prefixLtl << " from da.hoa");

                return DeterministicAutomaton::parseFromFile("da.hoa");
            }
        }

        std::shared_ptr<DeterministicAutomaton> LTL2DeterministicAutomaton::ltl2da(storm::logic::Formula const& f, bool dnf) {
            std::string prefixLtl = f.toPrefixString();

            storm::automata::DeterministicAutomaton::ptr da;
            if(isExternalDaToolSet()){
                da = ltl2daExternalTool(prefixLtl);
            } else {
                da = ltl2daInternalTool(prefixLtl, dnf);
            }
            return da;
        }

    }
}
