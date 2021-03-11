#include "storm/automata/LTL2DeterministicAutomaton.h"
#include "storm/automata/DeterministicAutomaton.h"

#include "storm/logic/Formula.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/FileIoException.h"

#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h> // for execlp
#include <sys/wait.h>

namespace storm {
    namespace automata {

        // TODO: this is quite hacky, improve robustness
        std::shared_ptr<DeterministicAutomaton> LTL2DeterministicAutomaton::ltl2da(storm::logic::Formula const& f) {
            std::string prefixLtl = f.toPrefixString();

            std::string ltl2da_tool = "ltl2da";
            if (const char* ltl2da_env = std::getenv("LTL2DA")) {
                ltl2da_tool = ltl2da_env;
            }

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
                while (wait(&status) != pid)
                    ;

                int rv;
                if (WIFEXITED(status)) {
                    rv = WEXITSTATUS(status);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Could not construct deterministic automaton: process aborted");
                }
                STORM_LOG_THROW(rv == 0, storm::exceptions::FileIoException, "Could not construct deterministic automaton for " << prefixLtl << ", return code = " << rv);

                STORM_LOG_INFO("Reading automaton for " << prefixLtl << " from da.hoa");
                return DeterministicAutomaton::parseFromFile("da.hoa");
            }
        }
    }
}
