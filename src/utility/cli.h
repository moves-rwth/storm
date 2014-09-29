#ifndef STORM_UTILITY_CLI_H_
#define STORM_UTILITY_CLI_H_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <sstream>

#include "src/utility/OsDetection.h"
#include "src/settings/SettingsManager.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
log4cplus::Logger logger;

// Includes for the linked libraries and versions header
#ifdef STORM_HAVE_INTELTBB
#	include "tbb/tbb_stddef.h"
#endif
#ifdef STORM_HAVE_GLPK
#	include "glpk.h"
#endif
#ifdef STORM_HAVE_GUROBI
#	include "gurobi_c.h"
#endif
#ifdef STORM_HAVE_Z3
#	include "z3.h"
#endif

namespace storm {
    namespace utility {
        namespace cli {
            
            /*!
             * Initializes the logging framework and sets up logging to console.
             */
            void initializeLogger() {
                logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
                logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
                log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
                consoleLogAppender->setName("mainConsoleAppender");
                consoleLogAppender->setThreshold(log4cplus::WARN_LOG_LEVEL);
                consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %b:%L: %m%n")));
                logger.addAppender(consoleLogAppender);
            }
            
            /*!
             * Performs some necessary initializations.
             */
            void setUp() {
                initializeLogger();
                std::cout.precision(10);
            }
            
            /*!
             * Performs some necessary clean-up.
             */
            void cleanUp() {
                // Intentionally left empty.
            }
            
            /*!
             * Sets up the logging to file.
             */
            void setUpFileLogging() {
                log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender(storm::settings::debugSettings().getLogfilename()));
                fileLogAppender->setName("mainFileAppender");
                fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %F:%L: %m%n")));
                logger.addAppender(fileLogAppender);
            }
            
            /*!
             * Gives the current working directory
             *
             * @return std::string The path of the current working directory
             */
            std::string getCurrentWorkingDirectory() {
                char temp[512];
                return (GetCurrentDir(temp, 512 - 1) ? std::string(temp) : std::string(""));
            }
            
            
            void printUsage() {
#ifndef WINDOWS
                struct rusage ru;
                getrusage(RUSAGE_SELF, &ru);
                
                std::cout << "===== Statistics ==============================" << std::endl;
                std::cout << "peak memory usage: " << ru.ru_maxrss/1024/1024 << "MB" << std::endl;
                std::cout << "CPU time: " << ru.ru_utime.tv_sec << "." << std::setw(3) << std::setfill('0') << ru.ru_utime.tv_usec/1000 << " seconds" << std::endl;
                std::cout << "===============================================" << std::endl;
#else
                HANDLE hProcess = GetCurrentProcess ();
                FILETIME ftCreation, ftExit, ftUser, ftKernel;
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc))) {
                    std::cout << "Memory Usage: " << std::endl;
                    std::cout << "\tPageFaultCount: " << pmc.PageFaultCount << std::endl;
                    std::cout << "\tPeakWorkingSetSize: " << pmc.PeakWorkingSetSize << std::endl;
                    std::cout << "\tWorkingSetSize: " << pmc.WorkingSetSize << std::endl;
                    std::cout << "\tQuotaPeakPagedPoolUsage: " << pmc.QuotaPeakPagedPoolUsage << std::endl;
                    std::cout << "\tQuotaPagedPoolUsage: " << pmc.QuotaPagedPoolUsage << std::endl;
                    std::cout << "\tQuotaPeakNonPagedPoolUsage: " << pmc.QuotaPeakNonPagedPoolUsage << std::endl;
                    std::cout << "\tQuotaNonPagedPoolUsage: " << pmc.QuotaNonPagedPoolUsage << std::endl;
                    std::cout << "\tPagefileUsage:" << pmc.PagefileUsage << std::endl;
                    std::cout << "\tPeakPagefileUsage: " << pmc.PeakPagefileUsage << std::endl;
                }
                
                GetProcessTimes (hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser);
                
                ULARGE_INTEGER uLargeInteger;
                uLargeInteger.LowPart = ftKernel.dwLowDateTime;
                uLargeInteger.HighPart = ftKernel.dwHighDateTime;
                double kernelTime = static_cast<double>(uLargeInteger.QuadPart) / 10000.0; // 100 ns Resolution to milliseconds
                uLargeInteger.LowPart = ftUser.dwLowDateTime;
                uLargeInteger.HighPart = ftUser.dwHighDateTime;
                double userTime = static_cast<double>(uLargeInteger.QuadPart) / 10000.0;
                
                std::cout << "CPU Time: " << std::endl;
                std::cout << "\tKernel Time: " << std::setprecision(5) << kernelTime << "ms" << std::endl;
                std::cout << "\tUser Time: " << std::setprecision(5) << userTime << "ms" << std::endl;
#endif
            }
            
            
            
            /*!
             * Prints the header.
             */
            void printHeader(const int argc, const char* argv[]) {
                std::cout << "StoRM" << std::endl;
                std::cout << "-----" << std::endl << std::endl;
                
                std::cout << "Version: " << STORM_CPP_VERSION_MAJOR << "." << STORM_CPP_VERSION_MINOR << "." << STORM_CPP_VERSION_PATCH;
                if (STORM_CPP_VERSION_COMMITS_AHEAD != 0) {
                    std::cout << " (+" << STORM_CPP_VERSION_COMMITS_AHEAD << " commits)";
                }
                std::cout << " build from revision " << STORM_CPP_VERSION_HASH;
                if (STORM_CPP_VERSION_DIRTY == 1) {
                    std::cout << " (DIRTY)";
                }
                std::cout << "." << std::endl;
                
#ifdef STORM_HAVE_INTELTBB
                std::cout << "Linked with Intel Threading Building Blocks v" << TBB_VERSION_MAJOR << "." << TBB_VERSION_MINOR << " (Interface version " << TBB_INTERFACE_VERSION << ")." << std::endl;
#endif
#ifdef STORM_HAVE_GLPK
                std::cout << "Linked with GNU Linear Programming Kit v" << GLP_MAJOR_VERSION << "." << GLP_MINOR_VERSION << "." << std::endl;
#endif
#ifdef STORM_HAVE_GUROBI
                std::cout << "Linked with Gurobi Optimizer v" << GRB_VERSION_MAJOR << "." << GRB_VERSION_MINOR << "." << GRB_VERSION_TECHNICAL << "." << std::endl;
#endif
#ifdef STORM_HAVE_Z3
                unsigned int z3Major, z3Minor, z3BuildNumber, z3RevisionNumber;
                Z3_get_version(&z3Major, &z3Minor, &z3BuildNumber, &z3RevisionNumber);
                std::cout << "Linked with Microsoft Z3 Optimizer v" << z3Major << "." << z3Minor << " Build " << z3BuildNumber << " Rev " << z3RevisionNumber << "." << std::endl;
#endif
                
                // "Compute" the command line argument string with which STORM was invoked.
                std::stringstream commandStream;
                for (int i = 0; i < argc; ++i) {
                    commandStream << argv[i] << " ";
                }
                std::cout << "Command line: " << commandStream.str() << std::endl;
                std::cout << "Current working directory: " << getCurrentWorkingDirectory() << std::endl << std::endl;
            }
            
            /*!
             * Parses the given command line arguments.
             *
             * @param argc The argc argument of main().
             * @param argv The argv argument of main().
             * @return True iff the program should continue to run after parsing the options.
             */
            bool parseOptions(const int argc, const char* argv[]) {
                storm::settings::SettingsManager& manager = storm::settings::mutableManager();
                try {
                    manager.setFromCommandLine(argc, argv);
                } catch (storm::exceptions::OptionParserException& e) {
                    std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
                    std::cout << std::endl;
                    manager.printHelp();
                    return false;
                }
                
                if (storm::settings::generalSettings().isHelpSet()) {
                    storm::settings::manager().printHelp(storm::settings::generalSettings().getHelpModuleName());
                    return false;
                }
                
                if (storm::settings::generalSettings().isVerboseSet()) {
                    logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);
                    LOG4CPLUS_INFO(logger, "Enabled verbose mode, log output gets printed to console.");
                }
                if (storm::settings::debugSettings().isDebugSet()) {
                    logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
                    logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::DEBUG_LOG_LEVEL);
                    LOG4CPLUS_INFO(logger, "Enabled very verbose mode, log output gets printed to console.");
                }
                if (storm::settings::debugSettings().isTraceSet()) {
                    logger.setLogLevel(log4cplus::TRACE_LOG_LEVEL);
                    logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::TRACE_LOG_LEVEL);
                    LOG4CPLUS_INFO(logger, "Enabled trace mode, log output gets printed to console.");
                }
                if (storm::settings::debugSettings().isLogfileSet()) {
                    setUpFileLogging();
                }
                return true;
            }
            
            void processOptions() {
                
            }

        }
    }
}

#endif