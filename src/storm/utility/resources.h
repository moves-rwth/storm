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
            
            static const int STORM_EXIT_GENERALERROR = -1;
            static const int STORM_EXIT_TIMEOUT = -2;
            static const int STORM_EXIT_MEMOUT = -3;
            
            inline std::size_t getCPULimit() {
                rlimit rl;
                getrlimit(RLIMIT_CPU, &rl);
                return rl.rlim_cur;
            }

            inline void setCPULimit(std::size_t seconds) {
                rlimit rl;
                getrlimit(RLIMIT_CPU, &rl);
                rl.rlim_cur = seconds;
                setrlimit(RLIMIT_CPU, &rl);
            }
            
            inline std::size_t usedCPU() {
                return std::size_t(clock()) / CLOCKS_PER_SEC;
            }
            
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
            
            inline void setMemoryLimit(std::size_t megabytes) {
#if defined LINUX
                rlimit rl;
                getrlimit(RLIMIT_AS, &rl);
                rl.rlim_cur = megabytes * 1024 * 1024;
                setrlimit(RLIMIT_AS, &rl);
#else
                (void)megabytes;
                STORM_LOG_WARN("Setting a memory limit is not supported for your operating system.");
#endif
            }
            
            inline void quickest_exit(int errorCode) {
#if defined LINUX
                std::quick_exit(errorCode);
#elif defined MACOS
                std::_Exit(errorCode);
#else
                std::abort();
#endif
            }
            
            inline void signalHandler(int signal) {
                if (signal == SIGXCPU) {
                    std::cerr << "Timeout." << std::endl;
                    quickest_exit(STORM_EXIT_TIMEOUT);
                } else if (signal == ENOMEM) {
                    std::cerr << "Out of memory" << std::endl;
                    quickest_exit(STORM_EXIT_MEMOUT);
                } else {
                    std::cerr << "Unknown abort in resource limitation module." << std::endl;
                    quickest_exit(STORM_EXIT_GENERALERROR);
                }
            }
            
            inline void installSignalHandler() {
                std::signal(SIGXCPU, signalHandler);
                std::signal(ENOMEM, signalHandler);
            }
            
        }
    }
}

#endif /* STORM_UTILITY_RESOURCES_H_ */
