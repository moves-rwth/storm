// Include other headers.
#include <chrono>
#include "storm/exceptions/BaseException.h"
#include "storm/utility/macros.h"
#include "storm/cli/cli.h"
#include "storm/utility/initialize.h"
#include "storm/utility/Stopwatch.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/ResourceSettings.h"

/*!
 * Main entry point of the executable storm.
 */
int main(const int argc, const char** argv) {

    try {
        storm::utility::Stopwatch totalTimer(true);
        storm::utility::setUp();
        storm::cli::printHeader("Storm", argc, argv);
        storm::settings::initializeAll("Storm", "storm");
        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }
        
        // From this point on we are ready to carry out the actual computations.
        storm::cli::processOptions();
        
        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        totalTimer.stop();

        if (storm::settings::getModule<storm::settings::modules::ResourceSettings>().isPrintTimeAndMemorySet()) {
            storm::cli::showTimeAndMemoryStatistics(totalTimer.getTimeInMilliseconds());
        }
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
