#ifndef STORM_UTILITY_CLI_H_
#define STORM_UTILITY_CLI_H_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <memory>

#include "storm-config.h"
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

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
log4cplus::Logger logger;
log4cplus::Logger printer;

// Headers that provide auxiliary functionality.
#include "src/utility/storm-version.h"
#include "src/utility/OsDetection.h"
#include "src/settings/SettingsManager.h"

// Headers related to parsing.
#include "src/parser/AutoParser.h"
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"

// Formula headers.
#include "src/logic/Formulas.h"

// Model headers.
#include "src/models/AbstractModel.h"

// Headers of builders.
#include "src/builder/ExplicitPrismModelBuilder.h"

// Headers for model processing.
#include "src/storage/NaiveDeterministicModelBisimulationDecomposition.h"
#include "src/storage/DeterministicModelBisimulationDecomposition.h"

// Headers for model checking.
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"

// Headers for counterexample generation.
#include "src/counterexamples/MILPMinimalLabelSetGenerator.h"
#include "src/counterexamples/SMTMinimalCommandSetGenerator.h"

// Headers related to exception handling.
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace utility {
        namespace cli {
            
            /*!
             * Initializes the logging framework and sets up logging to console.
             */
            void initializeLogger() {
                logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
                log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
                consoleLogAppender->setName("mainConsoleAppender");
                consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %b:%L: %m%n")));
                logger.addAppender(consoleLogAppender);
				auto loglevel = storm::settings::debugSettings().isTraceSet() ? log4cplus::TRACE_LOG_LEVEL : storm::settings::debugSettings().isDebugSet() ? log4cplus::DEBUG_LOG_LEVEL : log4cplus::WARN_LOG_LEVEL;
                logger.setLogLevel(loglevel);
				consoleLogAppender->setThreshold(loglevel);
                
                
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
            void initializeFileLogging() {
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
            
            /*!
             * Prints the header including information about the linked libraries.
             */
            void printHeader(const int argc, const char* argv[]) {
                std::cout << "StoRM" << std::endl;
                std::cout << "-----" << std::endl << std::endl;
             	
			 
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
                
                // "Compute" the command line argument string with which STORM was invoked.
                std::stringstream commandStream;
                for (int i = 0; i < argc; ++i) {
                    commandStream << argv[i] << " ";
                }
                std::cout << "Command line: " << commandStream.str() << std::endl;
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
                    manager.printHelp();
                    throw e;
                    return false;
                }
                
                if (storm::settings::generalSettings().isHelpSet()) {
                    storm::settings::manager().printHelp(storm::settings::generalSettings().getHelpModuleName());
                    return false;
                }
				
				if (storm::settings::generalSettings().isVersionSet()) {
					storm::settings::manager().printVersion();
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
                    initializeFileLogging();
                }
                return true;
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::AbstractModel<ValueType>> buildExplicitModel(std::string const& transitionsFile, std::string const& labelingFile, boost::optional<std::string> const& stateRewardsFile = boost::optional<std::string>(), boost::optional<std::string> const& transitionRewardsFile = boost::optional<std::string>()) {
                return storm::parser::AutoParser::parseModel(transitionsFile, labelingFile, stateRewardsFile ? stateRewardsFile.get() : "", transitionRewardsFile ? transitionRewardsFile.get() : "");
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::AbstractModel<ValueType>> buildSymbolicModel(storm::prism::Program const& program, boost::optional<std::shared_ptr<storm::logic::Formula>> const& formula) {
                std::shared_ptr<storm::models::AbstractModel<ValueType>> result(nullptr);
                
                storm::settings::modules::GeneralSettings settings = storm::settings::generalSettings();

                // Get the string that assigns values to the unknown currently undefined constants in the model.
                std::string constants = settings.getConstantDefinitionString();
                
                bool buildRewards = false;
                if (formula) {
                    buildRewards = formula.get()->isRewardOperatorFormula() || formula.get()->isRewardPathFormula();
                }
                
                // Customize model-building.
                typename storm::builder::ExplicitPrismModelBuilder<double>::Options options;
                if (formula) {
                    options = storm::builder::ExplicitPrismModelBuilder<double>::Options(*formula.get());
                }
                options.addConstantDefinitionsFromString(program, settings.getConstantDefinitionString());
                
                // Then, build the model from the symbolic description.
                result = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program, options);
                return result;
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::AbstractModel<ValueType>> preprocessModel(std::shared_ptr<storm::models::AbstractModel<ValueType>> model, boost::optional<std::shared_ptr<storm::logic::Formula>> const& formula) {
                if (storm::settings::generalSettings().isBisimulationSet()) {
                    STORM_LOG_THROW(model->getType() == storm::models::DTMC, storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for DTMCs.");
                    std::shared_ptr<storm::models::Dtmc<double>> dtmc = model->template as<storm::models::Dtmc<double>>();
                    
                    if (dtmc->hasTransitionRewards()) {
                        dtmc->convertTransitionRewardsToStateRewards();
                    }
                    
                    std::cout << "Performing bisimulation minimization... ";
                    typename storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options;
                    if (formula) {
                        options = storm::storage::DeterministicModelBisimulationDecomposition<double>::Options(*model, *formula.get());
                    }
                    if (storm::settings::bisimulationSettings().isWeakBisimulationSet()) {
                        options.weak = true;
                        options.bounded = false;
                    }
                    
                    storm::storage::DeterministicModelBisimulationDecomposition<double> bisimulationDecomposition(*dtmc, options);
                    model = bisimulationDecomposition.getQuotient();
                    std::cout << "done." << std::endl << std::endl;
                }
                return model;
            }
            
            void generateCounterexample(storm::prism::Program const& program, std::shared_ptr<storm::models::AbstractModel<double>> model, std::shared_ptr<storm::logic::Formula> const& formula) {
                if (storm::settings::counterexampleGeneratorSettings().isMinimalCommandSetGenerationSet()) {
                    STORM_LOG_THROW(model->getType() == storm::models::MDP, storm::exceptions::InvalidTypeException, "Minimal command set generation is only available for MDPs.");
                    STORM_LOG_THROW(storm::settings::generalSettings().isSymbolicSet(), storm::exceptions::InvalidSettingsException, "Minimal command set generation is only available for symbolic models.");
                    
                    std::shared_ptr<storm::models::Mdp<double>> mdp = model->as<storm::models::Mdp<double>>();

                    // Determine whether we are required to use the MILP-version or the SAT-version.
                    bool useMILP = storm::settings::counterexampleGeneratorSettings().isUseMilpBasedMinimalCommandSetGenerationSet();

                    if (useMILP) {
                        storm::counterexamples::MILPMinimalLabelSetGenerator<double>::computeCounterexample(program, *mdp, formula);
                    } else {
                        storm::counterexamples::SMTMinimalCommandSetGenerator<double>::computeCounterexample(program, storm::settings::generalSettings().getConstantDefinitionString(), *mdp, formula);
                    }

                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No suitable counterexample representation selected.");
                }
            }
            
            void processOptions() {
                if (storm::settings::debugSettings().isLogfileSet()) {
                    initializeFileLogging();
                }
                
                storm::settings::modules::GeneralSettings const& settings = storm::settings::generalSettings();
                
                // If we have to build the model from a symbolic representation, we need to parse the representation first.
                boost::optional<storm::prism::Program> program;
                if (settings.isSymbolicSet()) {
                    std::string const& programFile = settings.getSymbolicModelFilename();
                    program = storm::parser::PrismParser::parse(programFile);
                }
                
                // Then proceed to parsing the property (if given), since the model we are building may depend on the property.
                boost::optional<std::shared_ptr<storm::logic::Formula>> formula;
                if (settings.isPropertySet()) {
                    if (program) {
                        storm::parser::FormulaParser formulaParser(program.get().getManager().getSharedPointer());
                        formula = formulaParser.parseFromString(settings.getProperty());
                    } else {
                        storm::parser::FormulaParser formulaParser;
                        formula = formulaParser.parseFromString(settings.getProperty());
                    }
                }
                
                // Now we are ready to actually build the model.
                std::chrono::high_resolution_clock::time_point totalTimeStart = std::chrono::high_resolution_clock::now();
                std::chrono::high_resolution_clock::time_point buildingTimeStart = std::chrono::high_resolution_clock::now();
                std::shared_ptr<storm::models::AbstractModel<double>> model;
                if (settings.isExplicitSet()) {
                    model = buildExplicitModel<double>(settings.getTransitionFilename(), settings.getLabelingFilename(), settings.isStateRewardsSet() ? settings.getStateRewardsFilename() : boost::optional<std::string>(), settings.isTransitionRewardsSet() ? settings.getTransitionRewardsFilename() : boost::optional<std::string>());
                } else if (settings.isSymbolicSet()) {
                    STORM_LOG_THROW(program, storm::exceptions::InvalidStateException, "Program has not been successfully parsed.");
                    model = buildSymbolicModel<double>(program.get(), formula);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model.");
                }
                std::chrono::high_resolution_clock::time_point buildingTimeEnd = std::chrono::high_resolution_clock::now();

                // Now, we can do the preprocessing on the model, if it was requested.
                std::chrono::high_resolution_clock::time_point preprocessingTimeStart = std::chrono::high_resolution_clock::now();
                model = preprocessModel(model, formula);
                std::chrono::high_resolution_clock::time_point preprocessingTimeEnd = std::chrono::high_resolution_clock::now();
                std::chrono::high_resolution_clock::time_point totalTimeEnd = std::chrono::high_resolution_clock::now();

                // Print some information about the model.
                model->printModelInformationToStream(std::cout);
                
                if (storm::settings::generalSettings().isShowStatisticsSet()) {
                    std::chrono::high_resolution_clock::duration totalTime = totalTimeEnd - totalTimeStart;
                    std::chrono::milliseconds totalTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime);
                    std::chrono::high_resolution_clock::duration buildingTime = buildingTimeEnd - buildingTimeStart;
                    std::chrono::milliseconds buildingTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(buildingTime);
                    std::chrono::high_resolution_clock::duration preprocessingTime = preprocessingTimeEnd - preprocessingTimeStart;
                    std::chrono::milliseconds preprocessingTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(preprocessingTime);

                    STORM_PRINT_AND_LOG(std::endl);
                    STORM_PRINT_AND_LOG("Time breakdown:" << std::endl);
                    STORM_PRINT_AND_LOG("    * time for building: " << buildingTimeInMilliseconds.count() << "ms" << std::endl);
                    STORM_PRINT_AND_LOG("    * time for preprocessing: " << preprocessingTimeInMilliseconds.count() << "ms" << std::endl);
                    STORM_PRINT_AND_LOG("------------------------------------------" << std::endl);
                    STORM_PRINT_AND_LOG("    * total time: " << totalTimeInMilliseconds.count() << "ms" << std::endl);
                    STORM_PRINT_AND_LOG(std::endl);
                }
                
                // If we were requested to generate a counterexample, we now do so.
                if (settings.isCounterexampleSet()) {
                    STORM_LOG_THROW(settings.isPropertySet(), storm::exceptions::InvalidSettingsException, "Unable to generate counterexample without a property.");
                    STORM_LOG_THROW(program, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for non-symbolic model.");
                    generateCounterexample(program.get(), model, formula.get());
                } else if (formula) {
                    std::cout << std::endl << "Model checking property: " << *formula.get() << " ...";
                    std::unique_ptr<storm::modelchecker::CheckResult> result;
                    if (model->getType() == storm::models::DTMC) {
                        std::shared_ptr<storm::models::Dtmc<double>> dtmc = model->as<storm::models::Dtmc<double>>();
//                        storm::modelchecker::SparseDtmcPrctlModelChecker<double> modelchecker(*dtmc);
                        storm::modelchecker::SparseDtmcEliminationModelChecker<double> modelchecker(*dtmc);
                        result = modelchecker.check(*formula.get());
                    } else if (model->getType() == storm::models::MDP) {
                        std::shared_ptr<storm::models::Mdp<double>> mdp = model->as<storm::models::Mdp<double>>();
                        storm::modelchecker::SparseMdpPrctlModelChecker<double> modelchecker(*mdp);
                        result = modelchecker.check(*formula.get());
                    }
                    if (result) {
                        std::cout << " done." << std::endl;
                        std::cout << "Result (initial states): ";
                        result->writeToStream(std::cout, model->getInitialStates());
                        std::cout << std::endl << std::endl;
                    } else {
                        std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                    }
                }
            }
        }
    }
}

#endif