#include "SignalHandler.h"

#include <csignal>
#include <iostream>

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/ResourceSettings.h"

namespace storm {
    namespace utility {
        namespace resources {

            // Maximal waiting time after abort signal before terminating
            int maxWaitTime = 0;

            SignalInformation::SignalInformation() : terminate(false), lastSignal(0) {
            }

            SignalInformation::~SignalInformation() {
                // Intentionally left empty.
            }

            SignalInformation& SignalInformation::infos() {
                static SignalInformation signalInfo;
                return signalInfo;
            }

            /*!
             * Signal handler for aborts, etc.
             * After the first signal the handler waits a number of seconds to let the program print preliminary results
             * which were computed up this point. If the waiting time is exceeded or if a second signal was raised
             * in the mean time, the program immediately terminates.
             *
             * @param signal Exit code of signal.
             */
            void signalHandler(int signal) {
                if (!isTerminate()) {
                    // First time we get an abort signal
                    // We give the program a number of seconds to print results obtained so far before termination
                    std::cerr << "ERROR: The program received signal " << signal << " and will be aborted in " << maxWaitTime << "s." << std::endl;
                    SignalInformation::infos().setTerminate(true);
                    // Remember original signal such that the program returns the correct original signal
                    SignalInformation::infos().setErrorCode(signal);
                    // Trigger a new signal after waitTime
                    setTimeoutAlarm(maxWaitTime);
                } else {
                    // Second time we get a signal
                    // We now definitely have to terminate as fast as possible
                    if (SignalInformation::infos().getErrorCode() == SIGXCPU) {
                        std::cerr << "TIMEOUT." << std::endl;
                    } else if (SignalInformation::infos().getErrorCode() == ENOMEM) {
                        std::cerr << "OUT OF MEMORY." << std::endl;
                    } else if (SignalInformation::infos().getErrorCode() == SIGABRT || SignalInformation::infos().getErrorCode() == SIGINT) {
                        std::cerr << "ABORT." << std::endl;
                    } else {
                        std::cerr << "Received signal " << signal << std::endl;
                    }
                    quickest_exit(SignalInformation::infos().getErrorCode());
                }
            }

            void installSignalHandler() {
                // Set the waiting time
                maxWaitTime = storm::settings::getModule<storm::settings::modules::ResourceSettings>().getSignalWaitingTimeInSeconds();

                // We use the newer sigaction instead of signal
                struct sigaction sa;
                sa.sa_handler = signalHandler;
                sigemptyset(&sa.sa_mask);
                sa.sa_flags = SA_RESTART;

                // CPU Limit
                if (sigaction(SIGXCPU, &sa, nullptr) == -1) {
                    std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
                }
                // Memory out
                if (sigaction(ENOMEM, &sa, nullptr) == -1) {
                    std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
                }
                if (sigaction(SIGSEGV, &sa, nullptr) == -1) {
                    std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
                }
                if (sigaction(SIGABRT, &sa, nullptr) == -1) {
                    std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
                }
                if (sigaction(SIGINT, &sa, nullptr) == -1) {
                    std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
                }
                if (sigaction(SIGTERM, &sa, nullptr) == -1) {
                    std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
                }
                if (sigaction(SIGALRM, &sa, nullptr) == -1) {
                    std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
                }
            }


        }
    }
}
