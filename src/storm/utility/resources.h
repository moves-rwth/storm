#ifndef STORM_UTILITY_RESOURCES_H_
#define STORM_UTILITY_RESOURCES_H_

#include <cstdlib>
#include <csignal>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>

#include "storm-config.h"
#include "storm/utility/OsDetection.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace utility {
        namespace resources {

            // Maximal waiting time after abort signal before terminating
            static const int maxWaitTime = 3;

            // Flag whether the program should terminate
            static bool terminate = false;
            // Store last signal code
            static int lastSignal = 0;

            /*!
             * Check whether the program should terminate (due to some abort signal).
             * @return True iff program should terminate.
             */
            inline bool isTerminate() {
                return terminate;
            }

            /*! Get the error code.
             * Default is 0. If a signal was handled, the corresponding signal code is returned.
             * @return Error code.
             */
            inline int getErrorCode() {
                return lastSignal;
            }

            /*!
             * Get CPU limit.
             * @return CPU limit in seconds.
             */
            inline std::size_t getCPULimit() {
                rlimit rl;
                getrlimit(RLIMIT_CPU, &rl);
                return rl.rlim_cur;
            }

            /*!
             * Set CPU limit.
             * @param seconds CPU limit in seconds.
             */
            inline void setCPULimit(std::size_t seconds) {
                rlimit rl;
                getrlimit(RLIMIT_CPU, &rl);
                rl.rlim_cur = seconds;
                setrlimit(RLIMIT_CPU, &rl);
            }

            /*!
             * Get used CPU time.
             * @return CPU time in seconds.
             */
            inline std::size_t usedCPU() {
                return std::size_t(clock()) / CLOCKS_PER_SEC;
            }

            /*!
             * Get memory limit.
             * @return Memory limit in MB.
             */
            inline std::size_t getMemoryLimit() {
#if defined LINUX
                rlimit rl;
                getrlimit(RLIMIT_AS, &rl);
                return rl.rlim_cur;
#else
                STORM_LOG_WARN("Retrieving the memory limit is not supported for your operating system.");
                return 0;
#endif
            }

            /*!
             * Set memory limit.
             * @param megabytes Memory limit in MB.
             */
            inline void setMemoryLimit(std::size_t megabytes) {
#if defined LINUX
                rlimit rl;
                getrlimit(RLIMIT_AS, &rl);
                rl.rlim_cur = megabytes * 1024 * 1024;
                setrlimit(RLIMIT_AS, &rl);
#else
                (void) megabytes; // To silence "unused" warning
                STORM_LOG_WARN("Setting a memory limit is not supported for your operating system.");
#endif
            }

            /*!
             * Exit without cleanup.
             * @param errorCode Error code to return.
             */
            inline void quickest_exit(int errorCode) {
#if defined LINUX
                std::quick_exit(errorCode);
#elif defined MACOS
                std::_Exit(errorCode);
#else
                std::abort();
#endif
            }

            /*!
             * Set timeout by raising an alarm after timeout seconds.
             * @param timeout Timeout in seconds.
             */
            inline void setTimeoutAlarm(uint_fast64_t timeout) {
                alarm(timeout);
            }

            /*!
             * Signal handler for aborts, etc.
             * After the first signal the handler waits a number of seconds to let the program print preliminary results
             * which were computed up this point. If the waiting time is exceeded or if a second signal was raised
             * in the mean time, the program immediately terminates.
             * @param signal Exit code of signal.
             */
            inline void signalHandler(int signal) {
                if (!isTerminate()) {
                    // First time we get an abort signal
                    // We give the program a number of seconds to print results obtained so far before termination
                    std::cerr << "ERROR: The program received signal " << signal << " and will be aborted in " << maxWaitTime << "s." << std::endl;
                    terminate = true;
                    // Remember original signal such that the program returns the correct original signal
                    lastSignal = signal;
                    // Trigger a new signal after waitTime
                    setTimeoutAlarm(maxWaitTime);
                } else {
                    // Second time we get a signal
                    // We now definitely have to terminate as fast as possible
                    if (getErrorCode() == SIGXCPU) {
                        std::cerr << "TIMEOUT." << std::endl;
                    } else if (getErrorCode() == ENOMEM) {
                        std::cerr << "OUT OF MEMORY." << std::endl;
                    } else if (getErrorCode() == SIGABRT || getErrorCode() == SIGINT) {
                        std::cerr << "ABORT." << std::endl;
                    } else {
                        std::cerr << "Received signal " << signal << std::endl;
                    }
                    quickest_exit(getErrorCode());
                }
            }

            /*!
             * Register some signal handlers to detect and correctly handle abortion (due to timeout for example).
             */
            inline void installSignalHandler() {
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

#endif /* STORM_UTILITY_RESOURCES_H_ */
