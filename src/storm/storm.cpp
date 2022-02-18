/*! \mainpage Storm - A Modern Probabilistic Model Checker
 *
 * This document contains the Doxygen documentation of the Storm source code.
 *
 * \section more_info More information
 * For more information, installation guides and tutorials on how to use Storm, visit the Storm website: http://www.stormchecker.org.
 */

#include "storm/exceptions/BaseException.h"
#include "storm/utility/macros.h"

#include "storm-cli-utilities/cli.h"

/*!
 * Main entry point of the executable storm.
 */
int main(const int argc, const char** argv) {
    try {
        return storm::cli::process(argc, argv);
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
