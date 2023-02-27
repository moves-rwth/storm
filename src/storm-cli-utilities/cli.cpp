#include "cli.h"

#include "storm-cli-utilities/resources.h"
#include "storm-version-info/storm-version.h"
#include "storm/io/file.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/initialize.h"
#include "storm/utility/macros.h"

#include <boost/algorithm/string/replace.hpp>
#include <ctime>
#include <type_traits>

#include "storm-cli-utilities/model-handling.h"

// Includes for the linked libraries and versions header.
#include "storm/adapters/IntelTbbAdapter.h"

#ifdef STORM_HAVE_GLPK
#include "glpk.h"
#endif
#ifdef STORM_HAVE_GUROBI
#include "gurobi_c.h"
#endif
#ifdef STORM_HAVE_Z3
#include "z3.h"
#endif
#ifdef STORM_HAVE_MSAT
#include "mathsat.h"
#endif
#ifdef STORM_HAVE_CUDA
#include <cuda.h>
#include <cuda_runtime.h>
#endif

#ifdef STORM_HAVE_SMTRAT
#include "lib/smtrat.h"
#endif

namespace storm {
namespace cli {

int64_t process(const int argc, const char** argv) {
    storm::utility::setUp();
    storm::cli::printHeader("Storm", argc, argv);
    storm::settings::initializeAll("Storm", "storm");

    storm::settings::addModule<storm::settings::modules::CounterexampleGeneratorSettings>();

    storm::utility::Stopwatch totalTimer(true);
    if (!storm::cli::parseOptions(argc, argv)) {
        return -1;
    }

    processOptions();

    totalTimer.stop();
    if (storm::settings::getModule<storm::settings::modules::ResourceSettings>().isPrintTimeAndMemorySet()) {
        storm::cli::printTimeAndMemoryStatistics(totalTimer.getTimeInMilliseconds());
    }

    storm::utility::cleanUp();
    return 0;
}

std::string shellQuoteSingleIfNecessary(const std::string& arg) {
    // quote empty argument
    if (arg.empty()) {
        return "''";
    }

    if (arg.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_./=") != std::string::npos) {
        // contains potentially unsafe character, needs quoting
        if (arg.find('\'') != std::string::npos) {
            // contains ', we have to replace all ' with '\''
            std::string escaped(arg);
            boost::replace_all(escaped, "'", "'\\''");
            return "'" + escaped + "'";
        } else {
            return "'" + arg + "'";
        }
    }

    return arg;
}

void printHeader(std::string const& name, const int argc, const char** argv) {
    STORM_PRINT(name << " " << storm::StormVersion::shortVersionString() << '\n');
#ifndef NDEBUG
    STORM_PRINT("DEBUG BUILD\n");
#endif
    // "Compute" the command line argument string with which storm was invoked.
    std::stringstream commandStream;
    for (int i = 1; i < argc; ++i) {
        commandStream << " " << shellQuoteSingleIfNecessary(argv[i]);
    }

    std::string command = commandStream.str();

    if (!command.empty()) {
        STORM_PRINT('\n');
        std::time_t result = std::time(nullptr);
        STORM_PRINT("Date: " << std::ctime(&result));
        STORM_PRINT("Command line arguments:" << commandStream.str() << '\n');
        STORM_PRINT("Current working directory: " << storm::utility::cli::getCurrentWorkingDirectory() << "\n\n");
    }
}

void printVersion(std::string const& name) {
    STORM_PRINT(storm::StormVersion::longVersionString() << '\n');
    STORM_PRINT(storm::StormVersion::buildInfo() << '\n');

#ifdef STORM_HAVE_INTELTBB
    STORM_PRINT("Linked with Intel Threading Building Blocks v" << TBB_VERSION_MAJOR << "." << TBB_VERSION_MINOR << " (Interface version "
                                                                << TBB_INTERFACE_VERSION << ").\n");
#endif
#ifdef STORM_HAVE_GLPK
    STORM_PRINT("Linked with GNU Linear Programming Kit v" << GLP_MAJOR_VERSION << "." << GLP_MINOR_VERSION << ".\n");
#endif
#ifdef STORM_HAVE_GUROBI
    STORM_PRINT("Linked with Gurobi Optimizer v" << GRB_VERSION_MAJOR << "." << GRB_VERSION_MINOR << "." << GRB_VERSION_TECHNICAL << ".\n");
#endif
#ifdef STORM_HAVE_Z3
    unsigned int z3Major, z3Minor, z3BuildNumber, z3RevisionNumber;
    Z3_get_version(&z3Major, &z3Minor, &z3BuildNumber, &z3RevisionNumber);
    STORM_PRINT("Linked with Microsoft Z3 Optimizer v" << z3Major << "." << z3Minor << " Build " << z3BuildNumber << " Rev " << z3RevisionNumber << ".\n");
#endif
#ifdef STORM_HAVE_MSAT
    char* msatVersion = msat_get_version();
    STORM_PRINT("Linked with " << msatVersion << ".\n");
    msat_free(msatVersion);
#endif
#ifdef STORM_HAVE_SMTRAT
    STORM_PRINT("Linked with SMT-RAT " << SMTRAT_VERSION << ".\n");
#endif
#ifdef STORM_HAVE_CARL
    // TODO get version string
    STORM_PRINT("Linked with CArL.\n");
#endif

#ifdef STORM_HAVE_CUDA
    int deviceCount = 0;
    cudaError_t error_id = cudaGetDeviceCount(&deviceCount);

    if (error_id == cudaSuccess) {
        STORM_PRINT("Compiled with CUDA support, ");
        // This function call returns 0 if there are no CUDA capable devices.
        if (deviceCount == 0) {
            STORM_PRINT("but there are no available device(s) that support CUDA.\n");
        } else {
            STORM_PRINT("detected " << deviceCount << " CUDA capable device(s):\n");
        }

        int dev, driverVersion = 0, runtimeVersion = 0;

        for (dev = 0; dev < deviceCount; ++dev) {
            cudaSetDevice(dev);
            cudaDeviceProp deviceProp;
            cudaGetDeviceProperties(&deviceProp, dev);

            STORM_PRINT("CUDA device " << dev << ": \"" << deviceProp.name << "\"\n");

            // Console log
            cudaDriverGetVersion(&driverVersion);
            cudaRuntimeGetVersion(&runtimeVersion);
            STORM_PRINT("  CUDA Driver Version / Runtime Version          " << driverVersion / 1000 << "." << (driverVersion % 100) / 10 << " / "
                                                                            << runtimeVersion / 1000 << "." << (runtimeVersion % 100) / 10 << '\n');
            STORM_PRINT("  CUDA Capability Major/Minor version number:    " << deviceProp.major << "." << deviceProp.minor << '\n');
        }
        STORM_PRINT('\n');
    } else {
        STORM_PRINT("Compiled with CUDA support, but an error occured trying to find CUDA devices.\n");
    }
#endif
}

bool parseOptions(const int argc, const char* argv[]) {
    try {
        storm::settings::mutableManager().setFromCommandLine(argc, argv);
    } catch (storm::exceptions::OptionParserException& e) {
        STORM_LOG_ERROR("Unable to parse command line options. Type '" + std::string(argv[0]) + " --help' or '" + std::string(argv[0]) +
                        " --help all' for help.");
        return false;
    }

    storm::settings::modules::GeneralSettings const& general = storm::settings::getModule<storm::settings::modules::GeneralSettings>();

    bool result = true;
    if (general.isHelpSet()) {
        storm::settings::manager().printHelp(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getHelpFilterExpression());
        result = false;
    }

    if (general.isVersionSet()) {
        printVersion("storm");
        result = false;
        ;
    }

    return result;
}

void setResourceLimits() {
    storm::settings::modules::ResourceSettings const& resources = storm::settings::getModule<storm::settings::modules::ResourceSettings>();

    // If we were given a time limit, we put it in place now.
    if (resources.isTimeoutSet()) {
        storm::utility::resources::setTimeoutAlarm(resources.getTimeoutInSeconds());
    }

    // register signal handler to handle aborts
    storm::utility::resources::installSignalHandler(storm::settings::getModule<storm::settings::modules::ResourceSettings>().getSignalWaitingTimeInSeconds());
}

void setFileLogging() {
    storm::settings::modules::DebugSettings const& debug = storm::settings::getModule<storm::settings::modules::DebugSettings>();
    if (debug.isLogfileSet()) {
        storm::utility::initializeFileLogging(debug.getLogfilename());
    }
}

void setLogLevel() {
    storm::settings::modules::GeneralSettings const& general = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
    storm::settings::modules::DebugSettings const& debug = storm::settings::getModule<storm::settings::modules::DebugSettings>();

    if (general.isVerboseSet()) {
        storm::utility::setLogLevel(l3pp::LogLevel::INFO);
    }
    if (debug.isDebugSet()) {
        storm::utility::setLogLevel(l3pp::LogLevel::DEBUG);
    }
    if (debug.isTraceSet()) {
        storm::utility::setLogLevel(l3pp::LogLevel::TRACE);
    }
    setFileLogging();
}

void setUrgentOptions() {
    setResourceLimits();
    setLogLevel();
    setFileLogging();
    // Set output precision
    storm::utility::setOutputDigitsFromGeneralPrecision(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

void processOptions() {
    // Start by setting some urgent options (log levels, resources, etc.)
    setUrgentOptions();

    // Parse symbolic input (PRISM, JANI, properties, etc.)
    SymbolicInput symbolicInput = parseSymbolicInput();

    // Obtain settings for model processing
    ModelProcessingInformation mpi;

    // Preprocess the symbolic input
    std::tie(symbolicInput, mpi) = preprocessSymbolicInput(symbolicInput);

    STORM_LOG_WARN_COND(mpi.isCompatible,
                        "The model checking query does not seem to be supported for the selected engine. Storm will try to solve the query, but you will most "
                        "likely get an error for at least one of the provided properties.");

    // Export symbolic input (if requested)
    exportSymbolicInput(symbolicInput);

#ifdef STORM_HAVE_CARL
    switch (mpi.verificationValueType) {
        case ModelProcessingInformation::ValueType::Parametric:
            processInputWithValueType<storm::RationalFunction>(symbolicInput, mpi);
            break;
        case ModelProcessingInformation::ValueType::Exact:
            processInputWithValueType<storm::RationalNumber>(symbolicInput, mpi);
            break;
        case ModelProcessingInformation::ValueType::FinitePrecision:
            processInputWithValueType<double>(symbolicInput, mpi);
            break;
    }
#else
    STORM_LOG_THROW(mpi.verificationValueType == ModelProcessingInformation::ValueType::FinitePrecision, storm::exceptions::NotSupportedException,
                    "No exact numbers or parameters are supported in this build.");
    processInputWithValueType<double>(symbolicInput, mpi);
#endif
}

void printTimeAndMemoryStatistics(uint64_t wallclockMilliseconds) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);

    std::cout << "\nPerformance statistics:\n";
#ifdef MACOS
    // For Mac OS, this is returned in bytes.
    uint64_t maximumResidentSizeInMegabytes = ru.ru_maxrss / 1024 / 1024;
#endif
#ifdef LINUX
    // For Linux, this is returned in kilobytes.
    uint64_t maximumResidentSizeInMegabytes = ru.ru_maxrss / 1024;
#endif
    std::cout << "  * peak memory usage: " << maximumResidentSizeInMegabytes << "MB\n";
    char oldFillChar = std::cout.fill('0');
    std::cout << "  * CPU time: " << ru.ru_utime.tv_sec << "." << std::setw(3) << ru.ru_utime.tv_usec / 1000 << "s\n";
    if (wallclockMilliseconds != 0) {
        std::cout << "  * wallclock time: " << (wallclockMilliseconds / 1000) << "." << std::setw(3) << (wallclockMilliseconds % 1000) << "s\n";
    }
    std::cout.fill(oldFillChar);
}

}  // namespace cli
}  // namespace storm
