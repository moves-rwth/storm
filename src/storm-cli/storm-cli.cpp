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
#include "storm-cli-utilities/model-handling.h"

void processOptions() {
    // Parse symbolic input (PRISM, JANI, properties, etc.)
    storm::cli::SymbolicInput symbolicInput = storm::cli::parseSymbolicInput();

    // Obtain settings for model processing
    storm::cli::ModelProcessingInformation mpi;

    // Preprocess the symbolic input
    std::tie(symbolicInput, mpi) = storm::cli::preprocessSymbolicInput(symbolicInput);

    STORM_LOG_WARN_COND(mpi.isCompatible,
                        "The model checking query does not seem to be supported for the selected engine. Storm will try to solve the query, but you will most "
                        "likely get an error for at least one of the provided properties.");

    // Export symbolic input (if requested)
    storm::cli::exportSymbolicInput(symbolicInput);

#ifdef STORM_HAVE_CARL
    switch (mpi.verificationValueType) {
        case storm::cli::ModelProcessingInformation::ValueType::Parametric:
            storm::cli::processInputWithValueType<storm::RationalFunction>(symbolicInput, mpi);
            break;
        case storm::cli::ModelProcessingInformation::ValueType::Exact:
            storm::cli::processInputWithValueType<storm::RationalNumber>(symbolicInput, mpi);
            break;
        case storm::cli::ModelProcessingInformation::ValueType::FinitePrecision:
            storm::cli::processInputWithValueType<double>(symbolicInput, mpi);
            break;
    }
#else
    STORM_LOG_THROW(mpi.verificationValueType == storm::cli::ModelProcessingInformation::ValueType::FinitePrecision, storm::exceptions::NotSupportedException,
                    "No exact numbers or parameters are supported in this build.");
    storm::cli::processInputWithValueType<double>(symbolicInput, mpi);
#endif
}

void initSettings(std::string const& name, std::string const& executableName) {
    storm::settings::initializeAll(name, executableName);
    storm::settings::addModule<storm::settings::modules::CounterexampleGeneratorSettings>();
}

/*!
 * Main entry point of the executable storm.
 */
int main(const int argc, const char** argv) {
    try {
        return storm::cli::process("Storm", "storm", initSettings, processOptions, argc, argv);
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
