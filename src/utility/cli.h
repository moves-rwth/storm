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
#ifdef STORM_HAVE_CUDA
#include <cuda.h>
#include <cuda_runtime.h>
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
#include "src/models/ModelBase.h"
#include "src/models/sparse/Model.h"
#include "src/models/symbolic/Model.h"

// Headers of builders.
#include "src/builder/ExplicitPrismModelBuilder.h"
#include "src/builder/DdPrismModelBuilder.h"

// Headers for model processing.
#include "src/storage/DeterministicModelBisimulationDecomposition.h"

// Headers for model checking.
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "src/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "src/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

// Headers for counterexample generation.
#include "src/counterexamples/MILPMinimalLabelSetGenerator.h"
#include "src/counterexamples/SMTMinimalCommandSetGenerator.h"

// Headers related to exception handling.
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/NotImplementedException.h"

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
                std::cout << "--------" << std::endl << std::endl;
                
                
                //				std::cout << storm::utility::StormVersion::longVersionString() << std::endl;
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
            std::shared_ptr<storm::models::sparse::Model<ValueType>> buildExplicitModel(std::string const& transitionsFile, std::string const& labelingFile, boost::optional<std::string> const& stateRewardsFile = boost::optional<std::string>(), boost::optional<std::string> const& transitionRewardsFile = boost::optional<std::string>()) {
                return storm::parser::AutoParser::parseModel(transitionsFile, labelingFile, stateRewardsFile ? stateRewardsFile.get() : "", transitionRewardsFile ? transitionRewardsFile.get() : "");
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::ModelBase> buildSymbolicModel(storm::prism::Program const& program, boost::optional<std::shared_ptr<storm::logic::Formula>> const& formula) {
                std::shared_ptr<storm::models::ModelBase> result(nullptr);
                
                storm::settings::modules::GeneralSettings settings = storm::settings::generalSettings();
                
                // Get the string that assigns values to the unknown currently undefined constants in the model.
                std::string constants = settings.getConstantDefinitionString();
                
                bool buildRewards = false;
                if (formula) {
                    buildRewards = formula.get()->isRewardOperatorFormula() || formula.get()->isRewardPathFormula();
                }
                
                // Customize and perform model-building.
                if (settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Sparse) {
                    typename storm::builder::ExplicitPrismModelBuilder<ValueType>::Options options;
                    if (formula) {
                        options = typename storm::builder::ExplicitPrismModelBuilder<ValueType>::Options(*formula.get());
                    }
                    options.addConstantDefinitionsFromString(program, settings.getConstantDefinitionString());
                    
                    // Generate command labels if we are going to build a counterexample later.
                    if (storm::settings::counterexampleGeneratorSettings().isMinimalCommandSetGenerationSet()) {
                        options.buildCommandLabels = true;
                    }
                    
                    result = storm::builder::ExplicitPrismModelBuilder<ValueType>::translateProgram(program, options);
                } else if (settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Dd || settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Hybrid) {
                    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
                    if (formula) {
                        options = typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options(*formula.get());
                    }
                    options.addConstantDefinitionsFromString(program, settings.getConstantDefinitionString());
                    
                    result = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
                }
                
                // Then, build the model from the symbolic description.
                return result;
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::ModelBase> preprocessModel(std::shared_ptr<storm::models::ModelBase> model, boost::optional<std::shared_ptr<storm::logic::Formula>> const& formula) {
                if (storm::settings::generalSettings().isBisimulationSet()) {
                    STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for sparse models.");
                    std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->template as<storm::models::sparse::Model<ValueType>>();
                    STORM_LOG_THROW(model->getType() == storm::models::ModelType::Dtmc || model->getType() == storm::models::ModelType::Ctmc, storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for DTMCs.");
                    std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> dtmc = sparseModel->template as<storm::models::sparse::Dtmc<ValueType>>();
                    
                    if (dtmc->hasTransitionRewards()) {
                        dtmc->convertTransitionRewardsToStateRewards();
                    }
                    
                    std::cout << "Performing bisimulation minimization... ";
                    typename storm::storage::DeterministicModelBisimulationDecomposition<ValueType>::Options options;
                    if (formula) {
                        options = typename storm::storage::DeterministicModelBisimulationDecomposition<ValueType>::Options(*sparseModel, *formula.get());
                    }
                    if (storm::settings::bisimulationSettings().isWeakBisimulationSet()) {
                        options.weak = true;
                        options.bounded = false;
                    }
                    
                    storm::storage::DeterministicModelBisimulationDecomposition<ValueType> bisimulationDecomposition(*dtmc, options);
                    model = bisimulationDecomposition.getQuotient();
                    std::cout << "done." << std::endl << std::endl;
                }
                return model;
            }
            
            template<typename ValueType>
            void generateCounterexample(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::shared_ptr<storm::logic::Formula> formula) {
                if (storm::settings::counterexampleGeneratorSettings().isMinimalCommandSetGenerationSet()) {
                    STORM_LOG_THROW(model->getType() == storm::models::ModelType::Mdp, storm::exceptions::InvalidTypeException, "Minimal command set generation is only available for MDPs.");
                    STORM_LOG_THROW(storm::settings::generalSettings().isSymbolicSet(), storm::exceptions::InvalidSettingsException, "Minimal command set generation is only available for symbolic models.");
                    
                    std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = model->template as<storm::models::sparse::Mdp<ValueType>>();
                    
                    // Determine whether we are required to use the MILP-version or the SAT-version.
                    bool useMILP = storm::settings::counterexampleGeneratorSettings().isUseMilpBasedMinimalCommandSetGenerationSet();
                    
                    if (useMILP) {
                        storm::counterexamples::MILPMinimalLabelSetGenerator<ValueType>::computeCounterexample(program, *mdp, formula);
                    } else {
                        storm::counterexamples::SMTMinimalCommandSetGenerator<ValueType>::computeCounterexample(program, storm::settings::generalSettings().getConstantDefinitionString(), *mdp, formula);
                    }
                    
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No suitable counterexample representation selected.");
                }
            }
            
#ifdef STORM_HAVE_CARL
            template<>
            void generateCounterexample(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula> formula) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for parametric model.");
            }
#endif
            
            template<typename ValueType>
            void verifySparseModel(boost::optional<storm::prism::Program> const& program, std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::shared_ptr<storm::logic::Formula> formula) {
                storm::settings::modules::GeneralSettings const& settings = storm::settings::generalSettings();
                
                // If we were requested to generate a counterexample, we now do so.
                if (settings.isCounterexampleSet()) {
                    STORM_LOG_THROW(program, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for non-symbolic model.");
                    generateCounterexample<ValueType>(program.get(), model, formula);
                } else {
                    std::cout << std::endl << "Model checking property: " << *formula << " ...";
                    std::unique_ptr<storm::modelchecker::CheckResult> result;
                    if (model->getType() == storm::models::ModelType::Dtmc) {
                        std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> dtmc = model->template as<storm::models::sparse::Dtmc<ValueType>>();
                        storm::modelchecker::SparseDtmcPrctlModelChecker<ValueType> modelchecker(*dtmc);
                        if (modelchecker.canHandle(*formula.get())) {
                            result = modelchecker.check(*formula.get());
                        } else {
                            storm::modelchecker::SparseDtmcEliminationModelChecker<ValueType> modelchecker2(*dtmc);
                            if (modelchecker2.canHandle(*formula.get())) {
                                modelchecker2.check(*formula.get());
                            }
                        }
                    } else if (model->getType() == storm::models::ModelType::Mdp) {
                        std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = model->template as<storm::models::sparse::Mdp<ValueType>>();
#ifdef STORM_HAVE_CUDA
                        if (settings.isCudaSet()) {
                            storm::modelchecker::TopologicalValueIterationMdpPrctlModelChecker<ValueType> modelchecker(*mdp);
                            result = modelchecker.check(*formula.get());
                        } else {
                            storm::modelchecker::SparseMdpPrctlModelChecker<ValueType> modelchecker(*mdp);
                            result = modelchecker.check(*formula.get());
                        }
#else
                        storm::modelchecker::SparseMdpPrctlModelChecker<ValueType> modelchecker(*mdp);
                        result = modelchecker.check(*formula.get());
#endif
                    } else if (model->getType() == storm::models::ModelType::Ctmc) {
                        std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> ctmc = model->template as<storm::models::sparse::Ctmc<ValueType>>();

                        storm::modelchecker::SparseCtmcCslModelChecker<ValueType> modelchecker(*ctmc);
                        result = modelchecker.check(*formula.get());
                    }
                    
                    if (result) {
                        std::cout << " done." << std::endl;
                        std::cout << "Result (initial states): ";
                        result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                        std::cout << *result << std::endl;
                    } else {
                        std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                    }
                    
                }
            }
            
#ifdef STORM_HAVE_CARL
            template<>
            void verifySparseModel(boost::optional<storm::prism::Program> const& program, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula> formula) {
                storm::settings::modules::GeneralSettings const& settings = storm::settings::generalSettings();
                
                STORM_LOG_THROW(model->getType() == storm::models::ModelType::Dtmc, storm::exceptions::InvalidSettingsException, "Currently parametric verification is only available for DTMCs.");
                std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
                
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result;
                
                storm::modelchecker::SparseDtmcEliminationModelChecker<storm::RationalFunction> modelchecker(*dtmc);
                if (modelchecker.canHandle(*formula.get())) {
                    result = modelchecker.check(*formula.get());
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The parametric engine currently does not support this property.");
                }
                
                if (result) {
                    std::cout << " done." << std::endl;
                    std::cout << "Result (initial states): ";
                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(dtmc->getInitialStates()));
                    std::cout << *result << std::endl;
                } else {
                    std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                }
            }
#endif
            
            template<storm::dd::DdType DdType>
            void verifySymbolicModel(boost::optional<storm::prism::Program> const& program, std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::shared_ptr<storm::logic::Formula> formula) {
                storm::settings::modules::GeneralSettings const& settings = storm::settings::generalSettings();
                
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result;
                if (model->getType() == storm::models::ModelType::Dtmc) {
                    std::shared_ptr<storm::models::symbolic::Dtmc<DdType>> dtmc = model->template as<storm::models::symbolic::Dtmc<DdType>>();
                    storm::modelchecker::HybridDtmcPrctlModelChecker<DdType, double> modelchecker(*dtmc);
                    if (modelchecker.canHandle(*formula.get())) {
                        result = modelchecker.check(*formula.get());
                    }
                } else if (model->getType() == storm::models::ModelType::Ctmc) {
                    std::shared_ptr<storm::models::symbolic::Ctmc<DdType>> ctmc = model->template as<storm::models::symbolic::Ctmc<DdType>>();
                    storm::modelchecker::HybridCtmcCslModelChecker<DdType, double> modelchecker(*ctmc);
                    if (modelchecker.canHandle(*formula.get())) {
                        result = modelchecker.check(*formula.get());
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This functionality is not yet implemented.");
                }
                
                if (result) {
                    std::cout << " done." << std::endl;
                    std::cout << "Result (initial states): ";
                    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(model->getReachableStates(), model->getInitialStates()));
                    std::cout << *result << std::endl;
                } else {
                    std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                }
            }
            
            template<typename ValueType>
            void buildAndCheckSymbolicModel(boost::optional<storm::prism::Program> const& program, boost::optional<std::shared_ptr<storm::logic::Formula>> formula) {
                // Now we are ready to actually build the model.
                STORM_LOG_THROW(program, storm::exceptions::InvalidStateException, "Program has not been successfully parsed.");
                std::shared_ptr<storm::models::ModelBase> model = buildSymbolicModel<ValueType>(program.get(), formula);
                
                STORM_LOG_THROW(model != nullptr, storm::exceptions::InvalidStateException, "Model could not be constructed for an unknown reason.");
                
                // Preprocess the model if needed.
                model = preprocessModel<ValueType>(model, formula);
                
                // Print some information about the model.
                model->printModelInformationToStream(std::cout);
                
                // Verify the model, if a formula was given.
                if (formula) {
                    if (model->isSparseModel()) {
                        verifySparseModel<ValueType>(program, model->as<storm::models::sparse::Model<ValueType>>(), formula.get());
                    } else if (model->isSymbolicModel()) {
                        if (storm::settings::generalSettings().getEngine() == storm::settings::modules::GeneralSettings::Engine::Hybrid) {
                            verifySymbolicModel(program, model->as<storm::models::symbolic::Model<storm::dd::DdType::CUDD>>(), formula.get());
                        } else {
                            // Not handled yet.
                            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This functionality is not yet implemented.");
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Invalid input model type.");
                    }
                }
            }
            
            template<typename ValueType>
            void buildAndCheckExplicitModel(boost::optional<std::shared_ptr<storm::logic::Formula>> formula) {
                storm::settings::modules::GeneralSettings const& settings = storm::settings::generalSettings();
                
                STORM_LOG_THROW(settings.isExplicitSet(), storm::exceptions::InvalidStateException, "Unable to build explicit model without model files.");
                std::shared_ptr<storm::models::ModelBase> model = buildExplicitModel<ValueType>(settings.getTransitionFilename(), settings.getLabelingFilename(), settings.isStateRewardsSet() ? settings.getStateRewardsFilename() : boost::optional<std::string>(), settings.isTransitionRewardsSet() ? settings.getTransitionRewardsFilename() : boost::optional<std::string>());
                
                // Preprocess the model if needed.
                model = preprocessModel<ValueType>(model, formula);
                
                // Print some information about the model.
                model->printModelInformationToStream(std::cout);
                
                // Verify the model, if a formula was given.
                if (formula) {
                    STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidStateException, "Expected sparse model.");
                    verifySparseModel<ValueType>(boost::optional<storm::prism::Program>(), model->as<storm::models::sparse::Model<ValueType>>(), formula.get());
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
                
                if (settings.isSymbolicSet()) {
#ifdef STORM_HAVE_CARL
                    if (settings.isParametricSet()) {
                        buildAndCheckSymbolicModel<storm::RationalFunction>(program.get(), formula);
                    } else {
#endif
                        buildAndCheckSymbolicModel<double>(program.get(), formula);
#ifdef STORM_HAVE_CARL
                    }
#endif
                } else if (settings.isExplicitSet()) {
                    buildAndCheckExplicitModel<double>(formula);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model.");
                }
            }
        }
    }
}

#endif