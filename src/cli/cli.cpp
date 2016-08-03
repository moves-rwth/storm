#include "cli.h"
#include "entrypoints.h"

#include "../utility/storm.h"

#include "src/settings/modules/DebugSettings.h"
#include "src/settings/modules/IOSettings.h"
#include "src/settings/modules/CoreSettings.h"
#include "src/exceptions/OptionParserException.h"

#include "src/utility/storm-version.h"

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
        
        bool parseOptions(const int argc, const char* argv[]) {
            try {
                storm::settings::mutableManager().setFromCommandLine(argc, argv);
            } catch (storm::exceptions::OptionParserException& e) {
                storm::settings::manager().printHelp();
                throw e;
                return false;
            }
            
            if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isHelpSet()) {
                storm::settings::manager().printHelp(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getHelpModuleName());
                return false;
            }
            
            if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isVersionSet()) {
                storm::settings::manager().printVersion();
                return false;
            }
            
            if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isVerboseSet()) {
                storm::utility::setLogLevel(l3pp::LogLevel::INFO);
            }
            if (storm::settings::getModule<storm::settings::modules::DebugSettings>().isDebugSet()) {
                storm::utility::setLogLevel(l3pp::LogLevel::DEBUG);
            }
            if (storm::settings::getModule<storm::settings::modules::DebugSettings>().isTraceSet()) {
                 storm::utility::setLogLevel(l3pp::LogLevel::TRACE);
            }
            if (storm::settings::getModule<storm::settings::modules::DebugSettings>().isLogfileSet()) {
                storm::utility::initializeFileLogging();
            }
            return true;
        }
        
        void processOptions() {
            if (storm::settings::getModule<storm::settings::modules::DebugSettings>().isLogfileSet()) {
                storm::utility::initializeFileLogging();
            }
            
            if (storm::settings::getModule<storm::settings::modules::IOSettings>().isSymbolicSet()) {
                // If we have to build the model from a symbolic representation, we need to parse the representation first.
                storm::prism::Program program = storm::parseProgram(storm::settings::getModule<storm::settings::modules::IOSettings>().getSymbolicModelFilename());
                
                // Get the string that assigns values to the unknown currently undefined constants in the model.
                std::string constantDefinitionString = storm::settings::getModule<storm::settings::modules::IOSettings>().getConstantDefinitionString();
                storm::prism::Program preprocessedProgram = storm::utility::prism::preprocess(program, constantDefinitionString);
                std::map<storm::expressions::Variable, storm::expressions::Expression> constantsSubstitution = preprocessedProgram.getConstantsSubstitution();
                
                // Then proceed to parsing the properties (if given), since the model we are building may depend on the property.
                std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
                if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isPropertySet()) {
                    formulas = storm::parseFormulasForProgram(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getProperty(), preprocessedProgram);
                }
                
                // There may be constants of the model appearing in the formulas, so we replace all their occurrences
                // by their definitions in the translated program.
                std::vector<std::shared_ptr<storm::logic::Formula const>> preprocessedFormulas;
                for (auto const& formula : formulas) {
                    preprocessedFormulas.emplace_back(formula->substitute(constantsSubstitution));
                }
                
                if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isParametricSet()) {
#ifdef STORM_HAVE_CARL
                    buildAndCheckSymbolicModel<storm::RationalFunction>(preprocessedProgram, preprocessedFormulas, true);
#else
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No parameters are supported in this build.");
#endif
                } else if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isExactSet()) {
#ifdef STORM_HAVE_CARL
                    buildAndCheckSymbolicModel<storm::RationalNumber>(preprocessedProgram, preprocessedFormulas, true);
#else
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No exact numbers are supported in this build.");
#endif
                } else {
                    buildAndCheckSymbolicModel<double>(preprocessedProgram, preprocessedFormulas, true);
                }
            } else if (storm::settings::getModule<storm::settings::modules::IOSettings>().isExplicitSet()) {
                STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::Sparse, storm::exceptions::InvalidSettingsException, "Only the sparse engine supports explicit model input.");
                
                // If the model is given in an explicit format, we parse the properties without allowing expressions
                // in formulas.
                std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
                if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isPropertySet()) {
                    formulas = storm::parseFormulasForExplicit(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getProperty());
                }
                
                buildAndCheckExplicitModel<double>(formulas, true);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model.");
            }
            
        }
        
    }
}
