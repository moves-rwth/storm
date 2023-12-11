#include "print.h"

#include "storm-version-info/storm-version.h"
#include "storm/utility/cli.h"
#include "storm/utility/macros.h"

#include <boost/algorithm/string/replace.hpp>
#include <ctime>

// Includes for the linked libraries and versions header.
#include "storm/adapters/IntelTbbAdapter.h"

#ifdef STORM_HAVE_GLPK
#include "glpk.h"
#endif
#ifdef STORM_HAVE_GUROBI
#include "gurobi_c.h"
#endif
#ifdef STORM_HAVE_MSAT
#include "mathsat.h"
#endif
#ifdef STORM_HAVE_SOPLEX
#include "soplex.h"
#endif
#ifdef STORM_HAVE_SMTRAT
#include "lib/smtrat.h"
#endif
#ifdef STORM_HAVE_SPOT
#include <spot/misc/version.hh>
#endif
#ifdef STORM_HAVE_XERCES
#include <xercesc/util/XercesVersion.hpp>
#endif
#ifdef STORM_HAVE_Z3
#include "z3.h"
#endif

namespace storm {
namespace cli {

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

void printVersion() {
    STORM_PRINT(storm::StormVersion::longVersionString() << '\n');
    STORM_PRINT(storm::StormVersion::buildInfo() << '\n');

    // Print configuration options
#ifdef STORM_USE_CLN_EA
    STORM_PRINT("Using CLN numbers for exact arithmetic.\n");
#else
    STORM_PRINT("Using GMP numbers for exact arithmetic.\n");
#endif
#ifdef STORM_USE_CLN_RF
    STORM_PRINT("Using CLN numbers for rational functions.\n");
#else
    STORM_PRINT("Using GMP numbers for rational functions.\n");
#endif

    // Print linked dependencies
#ifdef STORM_HAVE_CARL
    STORM_PRINT("Linked with CArL v" << STORM_CARL_VERSION << ".\n");
#endif
#ifdef STORM_HAVE_GLPK
    STORM_PRINT("Linked with GNU Linear Programming Kit v" << GLP_MAJOR_VERSION << "." << GLP_MINOR_VERSION << ".\n");
#endif
#ifdef STORM_HAVE_GUROBI
    STORM_PRINT("Linked with Gurobi Optimizer v" << GRB_VERSION_MAJOR << "." << GRB_VERSION_MINOR << "." << GRB_VERSION_TECHNICAL << ".\n");
#endif
#ifdef STORM_HAVE_INTELTBB
    STORM_PRINT("Linked with Intel Threading Building Blocks v" << TBB_VERSION_MAJOR << "." << TBB_VERSION_MINOR << " (Interface version "
                                                                << TBB_INTERFACE_VERSION << ").\n");
#endif
#ifdef STORM_HAVE_MSAT
    char* msatVersion = msat_get_version();
    STORM_PRINT("Linked with " << msatVersion << ".\n");
    msat_free(msatVersion);
#endif
#ifdef STORM_HAVE_SOPLEX
    STORM_PRINT("Linked with Soplex v" << SOPLEX_VERSION << ".\n");
#endif
#ifdef STORM_HAVE_SMTRAT
    STORM_PRINT("Linked with SMT-RAT v" << SMTRAT_VERSION << ".\n");
#endif
#ifdef STORM_HAVE_SPOT
    STORM_PRINT("Linked with Spot v" << spot::version() << ".\n");
#endif
#ifdef STORM_HAVE_XERCES
    STORM_PRINT("Linked with Xerces-C v" << gXercesMajVersion << "." << gXercesMinVersion << "." << gXercesRevision << ".\n");
#endif
#ifdef STORM_HAVE_Z3
    unsigned int z3Major, z3Minor, z3BuildNumber, z3RevisionNumber;
    Z3_get_version(&z3Major, &z3Minor, &z3BuildNumber, &z3RevisionNumber);
#ifdef STORM_HAVE_Z3_OPTIMIZE
    STORM_PRINT("Linked with Z3 Theorem Prover v" << z3Major << "." << z3Minor << " Build " << z3BuildNumber << " Rev " << z3RevisionNumber
                                                  << " (with optimization features).\n");
#else
    STORM_PRINT("Linked with Z3 Theorem Prover v" << z3Major << "." << z3Minor << " Build " << z3BuildNumber << " Rev " << z3RevisionNumber << ".\n");
#endif
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
