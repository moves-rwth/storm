#pragma once
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>

#include "storm-config.h"
#include "storm/utility/OsDetection.h"
#include "storm/utility/macros.h"

namespace storm {
namespace utility {
namespace resources {

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
    (void)megabytes;  // To silence "unused" warning
    STORM_LOG_WARN("Setting a memory limit is not supported for your operating system.");
#endif
}

}  // namespace resources
}  // namespace utility
}  // namespace storm
