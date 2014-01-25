
/*
 *	STORM - a C++ Rebuild of MRMC
 *	
 *	STORM (Stochastic Reward Model Checker) is a model checker for discrete-time and continuous-time Markov
 *	reward models. It supports reward extensions of PCTL and CSL (PRCTL
 *	and CSRL), and allows for the automated verification of properties
 *	concerning long-run and instantaneous rewards as well as cumulative
 *	rewards.
 *	
 *  Authors: Philipp Berger
 *
 *  Description: Central part of the application containing the main() Method
 */

#include "src/utility/OsDetection.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <vector>

#include "storm-config.h"
#include "src/models/Dtmc.h"
#include "src/models/MarkovAutomaton.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/solver/GurobiLpSolver.h"
#include "src/counterexamples/MILPMinimalLabelSetGenerator.h"
#include "src/counterexamples/SMTMinimalCommandSetGenerator.h"
#include "src/counterexamples/PathBasedSubsystemGenerator.h"
#include "src/parser/AutoParser.h"
#include "src/parser/MarkovAutomatonParser.h"
#include "src/parser/PrctlParser.h"
#include "src/utility/ErrorHandling.h"
#include "src/formula/Prctl.h"
#include "src/utility/vector.h"

#include "src/settings/Settings.h"
// Registers all standard options
#include "src/utility/StormOptions.h" 

#include "src/parser/PrctlFileParser.h"
#include "src/parser/LtlFileParser.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

#include "src/parser/PrismParser.h"
#include "src/adapters/ExplicitModelAdapter.h"
#include "src/adapters/SymbolicModelAdapter.h"

#include "src/exceptions/InvalidSettingsException.h"

#include <iostream>
#include <iomanip>
#include <fstream>

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
	double kernelTime = uLargeInteger.QuadPart / 10000.0; // 100 ns Resolution to milliseconds
	uLargeInteger.LowPart = ftUser.dwLowDateTime;
	uLargeInteger.HighPart = ftUser.dwHighDateTime;
	double userTime = uLargeInteger.QuadPart / 10000.0;

	std::cout << "CPU Time: " << std::endl;
	std::cout << "\tKernel Time: " << std::setprecision(3) << kernelTime << std::endl;
	std::cout << "\tUser Time: " << std::setprecision(3) << userTime << std::endl;
#endif
}

log4cplus::Logger logger;

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
 * Sets up the logging to file.
 */
void setUpFileLogging() {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender(s->getOptionByLongName("logfile").getArgument(0).getValueAsString()));
	fileLogAppender->setName("mainFileAppender");
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %F:%L: %m%n")));
	logger.addAppender(fileLogAppender);
}

/*!
 * Prints the header.
 */
void printHeader(const int argc, const char* argv[]) {
	std::cout << "StoRM" << std::endl;
	std::cout << "-----" << std::endl << std::endl;

	std::cout << "Version: 1.0 Alpha" << std::endl;
    
	// "Compute" the command line argument string with which STORM was invoked.
	std::stringstream commandStream;
	for (int i = 0; i < argc; ++i) {
		commandStream << argv[i] << " ";
	}
	std::cout << "Command line: " << commandStream.str() << std::endl << std::endl;
}

/*!
 * Parses the given command line arguments.
 *
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return True iff the program should continue to run after parsing the options.
 */
bool parseOptions(const int argc, const char* argv[]) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	try {
		storm::settings::Settings::parse(argc, argv);
	} catch (storm::exceptions::OptionParserException& e) {
		std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
		std::cout << std::endl << s->getHelpText();
		return false;
	}

	if (s->isSet("help")) {
		std::cout << storm::settings::Settings::getInstance()->getHelpText();
		return false;
	}

	if (s->isSet("verbose")) {
		logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);
		LOG4CPLUS_INFO(logger, "Enabled verbose mode, log output gets printed to console.");
	}
	if (s->isSet("debug")) {
		logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
		logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::DEBUG_LOG_LEVEL);
		LOG4CPLUS_INFO(logger, "Enabled very verbose mode, log output gets printed to console.");
	}
	if (s->isSet("trace")) {
		logger.setLogLevel(log4cplus::TRACE_LOG_LEVEL);
		logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::TRACE_LOG_LEVEL);
		LOG4CPLUS_INFO(logger, "Enabled trace mode, log output gets printed to console.");
	}
	if (s->isSet("logfile")) {
		setUpFileLogging();
	}
	return true;
}

/*!
 * Performs some necessary initializations.
 */
void setUp() {
    // Increase the precision of output.
	std::cout.precision(10);
}

/*!
 * Performs some necessary clean-up.
 */
void cleanUp() {
	delete storm::utility::cuddUtilityInstance();
}

/*!
 * Creates a model checker for the given DTMC that complies with the given options.
 *
 * @param dtmc A reference to the DTMC for which the model checker is to be created.
 * @return A pointer to the resulting model checker.
 */
storm::modelchecker::prctl::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Dtmc<double>& dtmc) {
    // Create the appropriate model checker.
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	std::string const chosenMatrixLibrary = s->getOptionByLongName("matrixLibrary").getArgument(0).getValueAsString();
	if (chosenMatrixLibrary == "gmm++") {
		return new storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double>(dtmc, new storm::solver::GmmxxLinearEquationSolver<double>());
	}
    
	// The control flow should never reach this point, as there is a default setting for matrixlib.
	std::string message = "No matrix library suitable for DTMC model checking has been set.";
	throw storm::exceptions::InvalidSettingsException() << message;
	return nullptr;
}

/*!
 * Creates a model checker for the given MDP that complies with the given options.
 *
 * @param mdp The Dtmc that the model checker will check
 * @return
 */
storm::modelchecker::prctl::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Mdp<double>& mdp) {
    // Create the appropriate model checker.
    return new storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double>(mdp);
}

/*!
 * Checks the PRCTL formulae provided on the command line on the given model checker.
 *
 * @param modelchecker The model checker that is to be invoked on all given formulae.
 */
void checkPrctlFormulae(storm::modelchecker::prctl::AbstractModelChecker<double> const& modelchecker) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	if (s->isSet("prctl")) {
		std::string const chosenPrctlFile = s->getOptionByLongName("prctl").getArgument(0).getValueAsString();
		LOG4CPLUS_INFO(logger, "Parsing prctl file: " << chosenPrctlFile << ".");
		std::list<storm::property::prctl::AbstractPrctlFormula<double>*> formulaList = storm::parser::PrctlFileParser(chosenPrctlFile);
        
        for (auto formula : formulaList) {
        	modelchecker.check(*formula);
            delete formula;
        }
	}
}

/*!
 * Handles the counterexample generation control.
 *
 * @param parser An AutoParser to get the model from.
 */
 void generateCounterExample(storm::parser::AutoParser<double> parser) {
	LOG4CPLUS_INFO(logger, "Starting counterexample generation.");
	LOG4CPLUS_INFO(logger, "Testing inputs...");

	storm::settings::Settings* s  = storm::settings::Settings::getInstance();

	//First test output directory.
	std::string outPath = s->getOptionByLongName("counterExample").getArgument(0).getValueAsString();
	if(outPath.back() != '/' && outPath.back() != '\\') {
		LOG4CPLUS_ERROR(logger, "The output path is not valid.");
		return;
	}
	std::ofstream testFile(outPath + "test.dot");
	if(testFile.fail()) {
		LOG4CPLUS_ERROR(logger, "The output path is not valid.");
		return;
	}
	testFile.close();
	std::remove((outPath + "test.dot").c_str());

 	//Differentiate between model types.
	if(parser.getType() != storm::models::DTMC) {
		LOG4CPLUS_ERROR(logger, "Counterexample generation for the selected model type is not supported.");
		return;
	}

	storm::models::Dtmc<double> model = *parser.getModel<storm::models::Dtmc<double>>();
	LOG4CPLUS_INFO(logger, "Model is a DTMC.");

	// Get specified PRCTL formulas.
	if(!s->isSet("prctl")) {
		LOG4CPLUS_ERROR(logger, "No PRCTL formula file specified.");
		return;
	}

	std::string const chosenPrctlFile = s->getOptionByLongName("prctl").getArgument(0).getValueAsString();
	LOG4CPLUS_INFO(logger, "Parsing prctl file: " << chosenPrctlFile << ".");
	std::list<storm::property::prctl::AbstractPrctlFormula<double>*> formulaList = storm::parser::PrctlFileParser(chosenPrctlFile);

	// Test for each formula if a counterexample can be generated for it.
	if(formulaList.size() == 0) {
		LOG4CPLUS_ERROR(logger, "No PRCTL formula found.");
		return;
	}

	// Get prctl file name without the filetype
	uint_fast64_t first = 0;
	if(chosenPrctlFile.find('/') != std::string::npos) {
		first = chosenPrctlFile.find_last_of('/') + 1;
	} else if(chosenPrctlFile.find('\\') != std::string::npos) {
		first = chosenPrctlFile.find_last_of('\\') + 1;
	}

	uint_fast64_t length;
	if(chosenPrctlFile.find_last_of('.') != std::string::npos && chosenPrctlFile.find_last_of('.') >= first) {
		length = chosenPrctlFile.find_last_of('.') - first;
	} else {
		length = chosenPrctlFile.length() - first;
	}

	std::string outFileName = chosenPrctlFile.substr(first, length);

	// Test formulas and do generation
	uint_fast64_t fIndex = 0;
	for (auto formula : formulaList) {

		// First check if it is a formula type for which a counterexample can be generated.
		if (dynamic_cast<storm::property::prctl::AbstractStateFormula<double> const*>(formula) == nullptr) {
			LOG4CPLUS_ERROR(logger, "Unexpected kind of formula. Expected a state formula.");
			delete formula;
			continue;
		}

		storm::property::prctl::AbstractStateFormula<double> const& stateForm = static_cast<storm::property::prctl::AbstractStateFormula<double> const&>(*formula);

		// Do some output
		std::cout << "Generating counterexample for formula " << fIndex << ":" << std::endl;
		LOG4CPLUS_INFO(logger, "Generating counterexample for formula " + std::to_string(fIndex) + ": ");
		std::cout << "\t" << formula->toString() << "\n" << std::endl;
		LOG4CPLUS_INFO(logger, formula->toString());

		// Now check if the model does not satisfy the formula.
		// That is if there is at least one initial state of the model that does not.

		// Also raise the logger threshold for the log file, so that the model check infos aren't logged (useless and there are lots of them)
		// Lower it again after the model check.
		logger.getAppender("mainFileAppender")->setThreshold(log4cplus::WARN_LOG_LEVEL);
		storm::storage::BitVector result = stateForm.check(*createPrctlModelChecker(model));
		logger.getAppender("mainFileAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);

		if((result & model.getInitialStates()).getNumberOfSetBits() == model.getInitialStates().getNumberOfSetBits()) {
			std::cout << "Formula is satisfied. Can not generate counterexample.\n\n" << std::endl;
			LOG4CPLUS_INFO(logger, "Formula is satisfied. Can not generate counterexample.");
			delete formula;
			continue;
		}

		// Generate counterexample
		storm::models::Dtmc<double> counterExample = storm::counterexamples::PathBasedSubsystemGenerator<double>::computeCriticalSubsystem(*parser.getModel<storm::models::Dtmc<double>>(), stateForm);

		LOG4CPLUS_INFO(logger, "Found counterexample.");

		// Output counterexample
		// Do standard output
		std::cout << "Found counterexample with following properties: " << std::endl;
		counterExample.printModelInformationToStream(std::cout);
		std::cout << "For full Dtmc see " << outFileName << "_" << fIndex << ".dot at given output path.\n\n" << std::endl;

		// Write the .dot file
		std::ofstream outFile(outPath + outFileName + "_" + std::to_string(fIndex) + ".dot");
		if(outFile.good()) {
			counterExample.writeDotToStream(outFile, true, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, true);
			outFile.close();
		}

		fIndex++;
		delete formula;
	}
 }

/*!
 * Main entry point.
 */
int main(const int argc, const char* argv[]) {
    // Register a signal handler to catch signals and display a backtrace.
	installSignalHandler();
    
    // Print an information header.
	printHeader(argc, argv);

    // Initialize the logging engine and perform other initalizations.
	initializeLogger();
	setUp();

	try {
		LOG4CPLUS_INFO(logger, "StoRM was invoked.");

		// Parse options.
		if (!parseOptions(argc, argv)) {
			// If parsing failed or the option to see the usage was set, program execution stops here.
			return 0;
		}
        
		// Now, the settings are received and the specified model is parsed. The actual actions taken depend on whether
        // the model was provided in explicit or symbolic format.
		storm::settings::Settings* s = storm::settings::Settings::getInstance();
		if (s->isSet("explicit")) {
			std::string const chosenTransitionSystemFile = s->getOptionByLongName("explicit").getArgument(0).getValueAsString();
			std::string const chosenLabelingFile = s->getOptionByLongName("explicit").getArgument(1).getValueAsString();
            
			std::string chosenStateRewardsFile = "";
			if (s->isSet("stateRewards")) {
				chosenStateRewardsFile = s->getOptionByLongName("stateRewards").getArgument(0).getValueAsString();
			}
			std::string chosenTransitionRewardsFile = "";
			if (s->isSet("transitionRewards")) {
				chosenTransitionRewardsFile = s->getOptionByLongName("transitionRewards").getArgument(0).getValueAsString();
			}

			storm::parser::AutoParser<double> parser(chosenTransitionSystemFile, chosenLabelingFile, chosenStateRewardsFile, chosenTransitionRewardsFile);

            if (s->isSet("exportdot")) {
                std::ofstream outputFileStream;
                outputFileStream.open(s->getOptionByLongName("exportdot").getArgument(0).getValueAsString(), std::ofstream::out);
                parser.getModel<storm::models::AbstractModel<double>>()->writeDotToStream(outputFileStream);
                outputFileStream.close();
            }
            
			//Should there be a counterexample generated in case the formula is not satisfied?
			if(s->isSet("counterexample")) {

				generateCounterExample(parser);
			
			} else {
				// Determine which engine is to be used to choose the right model checker.
				LOG4CPLUS_DEBUG(logger, s->getOptionByLongName("matrixLibrary").getArgument(0).getValueAsString());

				// Depending on the model type, the appropriate model checking procedure is chosen.
				storm::modelchecker::prctl::AbstractModelChecker<double>* modelchecker = nullptr;
				parser.getModel<storm::models::AbstractModel<double>>()->printModelInformationToStream(std::cout);
                
				switch (parser.getType()) {
				case storm::models::DTMC:
					LOG4CPLUS_INFO(logger, "Model is a DTMC.");
					modelchecker = createPrctlModelChecker(*parser.getModel<storm::models::Dtmc<double>>());
					checkPrctlFormulae(*modelchecker);
					break;
				case storm::models::MDP:
					LOG4CPLUS_INFO(logger, "Model is an MDP.");
					modelchecker = createPrctlModelChecker(*parser.getModel<storm::models::Mdp<double>>());
					checkPrctlFormulae(*modelchecker);
					break;
				case storm::models::CTMC:
					LOG4CPLUS_INFO(logger, "Model is a CTMC.");
					LOG4CPLUS_ERROR(logger, "The selected model type is not supported.");
					break;
				case storm::models::CTMDP:
					LOG4CPLUS_INFO(logger, "Model is a CTMDP.");
					LOG4CPLUS_ERROR(logger, "The selected model type is not supported.");
					break;
                case storm::models::MA: {
                    LOG4CPLUS_INFO(logger, "Model is a Markov automaton.");
                    std::shared_ptr<storm::models::MarkovAutomaton<double>> markovAutomaton = parser.getModel<storm::models::MarkovAutomaton<double>>();
                    markovAutomaton->close();
                    storm::modelchecker::csl::SparseMarkovAutomatonCslModelChecker<double> mc(*markovAutomaton);
//                    std::cout << mc.checkExpectedTime(true, markovAutomaton->getLabeledStates("goal")) << std::endl;
//                    std::cout << mc.checkExpectedTime(false, markovAutomaton->getLabeledStates("goal")) << std::endl;
                    std::cout << mc.checkLongRunAverage(true, markovAutomaton->getLabeledStates("goal")) << std::endl;
                    std::cout << mc.checkLongRunAverage(false, markovAutomaton->getLabeledStates("goal")) << std::endl;
//                    std::cout << mc.checkTimeBoundedEventually(true, markovAutomaton->getLabeledStates("goal"), 0, 1) << std::endl;
//                    std::cout << mc.checkTimeBoundedEventually(true, markovAutomaton->getLabeledStates("goal"), 1, 2) << std::endl;
                    break;
                }
				case storm::models::Unknown:
				default:
					LOG4CPLUS_ERROR(logger, "The model type could not be determined correctly.");
					break;
				}

				if (modelchecker != nullptr) {
					delete modelchecker;
				}
			}
		} else if (s->isSet("symbolic")) {
            // First, we build the model using the given symbolic model description and constant definitions.
            std::string const& programFile = s->getOptionByLongName("symbolic").getArgument(0).getValueAsString();
            std::string const& constants = s->getOptionByLongName("constants").getArgument(0).getValueAsString();
            storm::ir::Program program = storm::parser::PrismParserFromFile(programFile);
            std::shared_ptr<storm::models::AbstractModel<double>> model = storm::adapters::ExplicitModelAdapter<double>::translateProgram(program, constants);
            model->printModelInformationToStream(std::cout);
            
            if (s->isSet("mincmd")) {
                if (model->getType() != storm::models::MDP) {
                    LOG4CPLUS_ERROR(logger, "Minimal command counterexample generation is only supported for models of type MDP.");
                    throw storm::exceptions::InternalTypeErrorException() << "Minimal command counterexample generation is only supported for models of type MDP.";
                }
                
                std::shared_ptr<storm::models::Mdp<double>> mdp = model->as<storm::models::Mdp<double>>();
                
                // Determine whether we are required to use the MILP-version or the SAT-version.
                bool useMILP = s->getOptionByLongName("mincmd").getArgumentByName("method").getValueAsString() == "milp";
                
                // Now parse the property file and receive the list of parsed formulas.
                std::string const& propertyFile = s->getOptionByLongName("mincmd").getArgumentByName("propertyFile").getValueAsString();
                std::list<storm::property::prctl::AbstractPrctlFormula<double>*> formulaList = storm::parser::PrctlFileParser(propertyFile);

                // Now generate the counterexamples for each formula.
                for (storm::property::prctl::AbstractPrctlFormula<double>* formulaPtr : formulaList) {
                    if (useMILP) {
                        storm::counterexamples::MILPMinimalLabelSetGenerator<double>::computeCounterexample(program, *mdp, formulaPtr);
                    } else {
                        storm::counterexamples::SMTMinimalCommandSetGenerator<double>::computeCounterexample(program, constants, *mdp, formulaPtr);
                    }
                    
                    // Once we are done with the formula, delete it.
                    delete formulaPtr;
                }
            } else if (s->isSet("prctl")) {
                // Determine which engine is to be used to choose the right model checker.
				LOG4CPLUS_DEBUG(logger, s->getOptionByLongName("matrixLibrary").getArgument(0).getValueAsString());
                
				// Depending on the model type, the appropriate model checking procedure is chosen.
				storm::modelchecker::prctl::AbstractModelChecker<double>* modelchecker = nullptr;
                
				switch (model->getType()) {
                    case storm::models::DTMC:
                        LOG4CPLUS_INFO(logger, "Model is a DTMC.");
                        modelchecker = createPrctlModelChecker(*model->as<storm::models::Dtmc<double>>());
                        checkPrctlFormulae(*modelchecker);
                        break;
                    case storm::models::MDP:
                        LOG4CPLUS_INFO(logger, "Model is an MDP.");
                        modelchecker = createPrctlModelChecker(*model->as<storm::models::Mdp<double>>());
                        checkPrctlFormulae(*modelchecker);
                        break;
                    case storm::models::CTMC:
                        LOG4CPLUS_INFO(logger, "Model is a CTMC.");
                        LOG4CPLUS_ERROR(logger, "The selected model type is not supported.");
                        break;
                    case storm::models::CTMDP:
                        LOG4CPLUS_INFO(logger, "Model is a CTMDP.");
                        LOG4CPLUS_ERROR(logger, "The selected model type is not supported.");
                        break;
                    case storm::models::MA:
                        LOG4CPLUS_INFO(logger, "Model is a Markov automaton.");
                        break;
                    case storm::models::Unknown:
                    default:
                        LOG4CPLUS_ERROR(logger, "The model type could not be determined correctly.");
                        break;
				}
                
				if (modelchecker != nullptr) {
					delete modelchecker;
				}
            }
        }
        
        // Perform clean-up and terminate.
		cleanUp();
        printUsage();
		LOG4CPLUS_INFO(logger, "StoRM terminating.");
		return 0;
	} catch (std::exception& e) {
		LOG4CPLUS_FATAL(logger, "An exception was thrown. Terminating.");
		LOG4CPLUS_FATAL(logger, "\t" << e.what());
	}
	return 1;
}
