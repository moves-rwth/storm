#include "cli.h"
#include "entrypoints.h"

#include "../utility/storm.h"

#include "storm/storage/SymbolicModelDescription.h"

#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/exceptions/OptionParserException.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/JaniExportSettings.h"

#include "storm/utility/resources.h"
#include "storm/utility/storm-version.h"


// Includes for the linked libraries and versions header.
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
#ifdef STORM_HAVE_MSAT
#   include "mathsat.h"
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
        std::string getCurrentWorkingDirectory() {
            char temp[512];
            return (GetCurrentDir(temp, 512 - 1) ? std::string(temp) : std::string(""));
        }

        void printHeader(const std::string name, const int argc, const char* argv[]) {
            std::cout << name << std::endl;
            std::cout << "---------------" << std::endl << std::endl;


            std::cout << storm::utility::StormVersion::longVersionString() << std::endl;
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
#ifdef STORM_HAVE_MSAT
            char* msatVersion = msat_get_version();
            std::cout << "Linked with " << msatVersion << "." << std::endl;
            msat_free(msatVersion);
#endif
#ifdef STORM_HAVE_SMTRAT
            std::cout << "Linked with SMT-RAT " << SMTRAT_VERSION << "." << std::endl;
#endif
#ifdef STORM_HAVE_CARL
            std::cout << "Linked with CARL." << std::endl;
            // TODO get version string
#endif

#ifdef STORM_HAVE_CUDA
            int deviceCount = 0;
            cudaError_t error_id = cudaGetDeviceCount(&deviceCount);

            if (error_id == cudaSuccess)
            {
                std::cout << "Compiled with CUDA support, ";
                // This function call returns 0 if there are no CUDA capable devices.
                if (deviceCount == 0)
                {
                    std::cout<< "but there are no available device(s) that support CUDA." << std::endl;
                } else
                {
                    std::cout << "detected " << deviceCount << " CUDA Capable device(s):" << std::endl;
                }

                int dev, driverVersion = 0, runtimeVersion = 0;

                for (dev = 0; dev < deviceCount; ++dev)
                {
                    cudaSetDevice(dev);
                    cudaDeviceProp deviceProp;
                    cudaGetDeviceProperties(&deviceProp, dev);

                    std::cout << "CUDA Device " << dev << ": \"" << deviceProp.name << "\"" << std::endl;

                    // Console log
                    cudaDriverGetVersion(&driverVersion);
                    cudaRuntimeGetVersion(&runtimeVersion);
                    std::cout << "  CUDA Driver Version / Runtime Version          " << driverVersion / 1000 << "." << (driverVersion % 100) / 10 << " / " << runtimeVersion / 1000 << "." << (runtimeVersion % 100) / 10 << std::endl;
                    std::cout << "  CUDA Capability Major/Minor version number:    " << deviceProp.major<<"."<<deviceProp.minor <<std::endl;
                }
                std::cout << std::endl;
            }
            else {
                std::cout << "Compiled with CUDA support, but an error occured trying to find CUDA devices." << std::endl;
            }
#endif

            // "Compute" the command line argument string with which STORM was invoked.
            std::stringstream commandStream;
            for (int i = 1; i < argc; ++i) {
                commandStream << argv[i] << " ";
            }
            std::cout << "Command line arguments: " << commandStream.str() << std::endl;
            std::cout << "Current working directory: " << getCurrentWorkingDirectory() << std::endl << std::endl;
        }

        void showTimeAndMemoryStatistics(uint64_t wallclockMilliseconds) {
#ifndef WINDOWS
            struct rusage ru;
            getrusage(RUSAGE_SELF, &ru);

            std::cout << "Performance statistics:" << std::endl;
            std::cout << "  * peak memory usage: " << ru.ru_maxrss/1024 << " mb" << std::endl;
            std::cout << "  * CPU time: " << ru.ru_utime.tv_sec << "." << std::setw(3) << std::setfill('0') << ru.ru_utime.tv_usec/1000 << " seconds" << std::endl;
            if (wallclockMilliseconds != 0) {
                std::cout << "  * wallclock time: " << (wallclockMilliseconds/1000) << "." << std::setw(3) << std::setfill('0') << (wallclockMilliseconds % 1000) << " seconds" << std::endl;
            }
            std::cout << "STATISTICS_OVERALL_HEADERS;" << "memory;CPU time;wallclock time;" << std::endl;
            std::cout << "STATISTICS_OVERALL_DATA;"
                      << ru.ru_maxrss/1024 << ";"
                      << ru.ru_utime.tv_sec << "." << std::setw(3) << std::setfill('0') << ru.ru_utime.tv_usec/1000  << ";"
                      <<  (wallclockMilliseconds/1000) << "." << std::setw(3) << std::setfill('0') << (wallclockMilliseconds % 1000) << ";" << std::endl;
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

        bool parseOptions(const int argc, const char* argv[]) {
            try {
                storm::settings::mutableManager().setFromCommandLine(argc, argv);
            } catch (storm::exceptions::OptionParserException& e) {
                storm::settings::manager().printHelp();
                throw e;
                return false;
            }
            
            storm::settings::modules::GeneralSettings const& general = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            storm::settings::modules::ResourceSettings const& resources = storm::settings::getModule<storm::settings::modules::ResourceSettings>();
            storm::settings::modules::DebugSettings const& debug = storm::settings::getModule<storm::settings::modules::DebugSettings>();
            
            if (general.isHelpSet()) {
                storm::settings::manager().printHelp(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getHelpModuleName());
                return false;
            }
            // If we were given a time limit, we put it in place now.
            if (resources.isTimeoutSet()) {
                storm::utility::resources::setCPULimit(resources.getTimeoutInSeconds());
            }
            
            if (general.isVersionSet()) {
                storm::settings::manager().printVersion();
                return false;
            }
            
            if (general.isVerboseSet()) {
                storm::utility::setLogLevel(l3pp::LogLevel::INFO);
            }
            if (debug.isDebugSet()) {
                storm::utility::setLogLevel(l3pp::LogLevel::DEBUG);
            }
            if (debug.isTraceSet()) {
                 storm::utility::setLogLevel(l3pp::LogLevel::TRACE);
            }
            if (debug.isLogfileSet()) {
                storm::utility::initializeFileLogging();
            }
            return true;
        }

        void processOptions() {
            STORM_LOG_TRACE("Processing options.");
            if (storm::settings::getModule<storm::settings::modules::DebugSettings>().isLogfileSet()) {
                storm::utility::initializeFileLogging();
            }

            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            if (ioSettings.isPrismOrJaniInputSet()) {
                storm::storage::SymbolicModelDescription model;
                std::vector<storm::jani::Property> properties;
                
                STORM_LOG_TRACE("Parsing symbolic input.");
                if (ioSettings.isPrismInputSet()) {
                    model = storm::parseProgram(ioSettings.getPrismInputFilename());
                    if (ioSettings.isPrismToJaniSet()) {
                        model = model.toJani(true);
                    }
                } else if (ioSettings.isJaniInputSet()) {
                    auto input = storm::parseJaniModel(ioSettings.getJaniInputFilename());
                    model = input.first;
                    if (ioSettings.isJaniPropertiesSet()) {
                        for (auto const& propName : ioSettings.getJaniProperties()) {
                            STORM_LOG_THROW(input.second.count(propName) == 1, storm::exceptions::InvalidArgumentException, "No property with name " << propName << " known.");
                            properties.push_back(input.second.at(propName));
                        }
                    }
                    
                }
                
                // Then proceed to parsing the properties (if given), since the model we are building may depend on the property.
                STORM_LOG_TRACE("Parsing properties.");
                uint64_t i = 0;
                if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isPropertySet()) {
                    if (model.isJaniModel()) {
                        for(auto const& formula : storm::parseFormulasForJaniModel(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getProperty(), model.asJaniModel())) {
                            properties.emplace_back(std::to_string(i), formula);
                            ++i;
                        }
                    } else {
                        for(auto const& formula :storm::parseFormulasForPrismProgram(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getProperty(), model.asPrismProgram())) {
                            properties.emplace_back(std::to_string(i), formula);
                            ++i;
                        }
                    }
                }
                
                if (model.isJaniModel() && storm::settings::getModule<storm::settings::modules::JaniExportSettings>().isJaniFileSet()) {
                    exportJaniModel(model.asJaniModel(), properties, storm::settings::getModule<storm::settings::modules::JaniExportSettings>().getJaniFilename());
                }
                
                if (ioSettings.isNoBuildModelSet()) {
                    return;
                }
                
                // Get the string that assigns values to the unknown currently undefined constants in the model.
                std::string constantDefinitionString = ioSettings.getConstantDefinitionString();
                model = model.preprocess(constantDefinitionString);

                STORM_LOG_TRACE("Building and checking symbolic model.");
                if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isParametricSet()) {
#ifdef STORM_HAVE_CARL
                    buildAndCheckSymbolicModel<storm::RationalFunction>(model, properties, true);
#else
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No parameters are supported in this build.");
#endif
                } else if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isExactSet()) {
#ifdef STORM_HAVE_CARL
                    buildAndCheckSymbolicModel<storm::RationalNumber>(model, properties, true);
#else
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No exact numbers are supported in this build.");
#endif
                } else {
                    buildAndCheckSymbolicModel<double>(model, properties, true);
                }
            } else if (storm::settings::getModule<storm::settings::modules::IOSettings>().isExplicitSet()) {
                STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::Sparse, storm::exceptions::InvalidSettingsException, "Only the sparse engine supports explicit model input.");

                // If the model is given in an explicit format, we parse the properties without allowing expressions
                // in formulas.
                std::vector<storm::jani::Property> properties;
                if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isPropertySet()) {
                    uint64_t i = 0;
                    for(auto const& formula : storm::parseFormulasForExplicit(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getProperty())) {
                        properties.emplace_back(std::to_string(i), formula);
                        ++i;
                        
                    }
                }

                buildAndCheckExplicitModel<double>(properties, true);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model.");
            }
        }

    }
}
