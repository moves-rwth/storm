// Include generated headers.
#include "storm-config.h"
#include "storm-version.h"

// Include other headers.
#include "src/exceptions/BaseException.h"
#include "src/utility/macros.h"
#include "src/utility/cli.h"

/*!
 * Main entry point of the executable storm.
 */
int main(const int argc, const char** argv) {
    try {
        storm::utility::cli::setUp();
        storm::utility::cli::printHeader(argc, argv);
        storm::utility::cli::parseOptions(argc, argv);
        
        // From this point on we are ready to carry out the actual computations.
        storm::utility::cli::processOptions();
        
        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cli::cleanUp();
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM to terminate. The message of the exception is " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM to terminate. The message of this exception is: " << exception.what());
    }
}
