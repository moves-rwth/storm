#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

/*!
 * This class represents the resource settings.
 */
class ResourceSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new set of general settings.
     */
    ResourceSettings();

    /*!
     * Retrieves whether time and memory consumption shall be printed at the end of a run.
     *
     * @return True iff the option was set.
     */
    bool isPrintTimeAndMemorySet() const;

    /*!
     * Retrieves whether the timeout option was set.
     *
     * @return True if the timeout option was set.
     */
    bool isTimeoutSet() const;

    /*!
     * Retrieves the time after which the computation has to be aborted in case the timeout option was set.
     *
     * @return The number of seconds after which to timeout.
     */
    uint_fast64_t getTimeoutInSeconds() const;

    /*!
     * Retrieves the waiting time of the program after a signal.
     * If a signal to abort is handled, the program should terminate.
     * However, it waits the given number of seconds before it is killed to allow for printing preliminary results.
     *
     * @return The number of seconds after which to exit the program.
     */
    uint_fast64_t getSignalWaitingTimeInSeconds() const;

    // The name of the module.
    static const std::string moduleName;

   private:
    // Define the string names of the options as constants.
    static const std::string timeoutOptionName;
    static const std::string timeoutOptionShortName;
    static const std::string printTimeAndMemoryOptionName;
    static const std::string printTimeAndMemoryOptionShortName;
    static const std::string signalWaitingTimeOptionName;
};
}  // namespace modules
}  // namespace settings
}  // namespace storm
