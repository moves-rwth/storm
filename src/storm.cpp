
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
#include <climits>
#include <sstream>
#include <vector>
#include <chrono>

#include "storm-config.h"
#include "storm-version.h"
#include "src/models/Dtmc.h"
#include "src/models/MarkovAutomaton.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/TopologicalValueIterationMdpPrctlModelChecker.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/solver/GurobiLpSolver.h"
#include "src/counterexamples/MILPMinimalLabelSetGenerator.h"
// #include "src/counterexamples/SMTMinimalCommandSetGenerator.h"
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
log4cplus::Logger logger;

#include "src/parser/PrismParser.h"
#include "src/adapters/ExplicitModelAdapter.h"
// #include "src/adapters/SymbolicModelAdapter.h"

#include "src/exceptions/InvalidSettingsException.h"

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
#ifdef STORM_HAVE_CUDAFORSTORM
#	include "cudaForStorm.h"
#endif

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
* Gives the current working directory
*
* @return std::string The path of the current working directory
*/
std::string getCurrentWorkingDirectory() {
	char temp[512];
	return (_getcwd(temp, 512 - 1) ? std::string(temp) : std::string(""));
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

#ifdef STORM_HAVE_CUDAFORSTORM
	std::cout << "Compiled with Runtime Support for the StoRM CUDA Plugin." << std::endl;
	std::cout << "Detected the StoRM CUDA Plugin in Version " << getStormCudaPluginVersionMajor() << "." << getStormCudaPluginVersionMinor() << "." << getStormCudaPluginVersionPatch();
	if (getStormCudaPluginVersionCommitsAhead() != 0) {
		std::cout << " (+" << getStormCudaPluginVersionCommitsAhead() << " commits)";
	}
	std::cout << " build from revision " << getStormCudaPluginVersionHash();
	if (getStormCudaPluginVersionIsDirty()) {
		std::cout << " (DIRTY)";
	}
	std::cout << "." << std::endl;
#endif
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
	std::cout << "Command line: " << commandStream.str() << std::endl << std::endl;
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
    // Intentionally left empty.
}

/*!
 * Creates a model checker for the given DTMC that complies with the given options.
 *
 * @param dtmc A reference to the DTMC for which the model checker is to be created.
 * @return A pointer to the resulting model checker.
 */
storm::modelchecker::prctl::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Dtmc<double> const & dtmc) {
    // Create the appropriate model checker.
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	std::string const& linsolver = s->getOptionByLongName("linsolver").getArgument(0).getValueAsString();
	if (linsolver == "gmm++") {
		return new storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double>(dtmc, new storm::solver::GmmxxLinearEquationSolver<double>());
	} else if (linsolver == "native") {
		return new storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double>(dtmc, new storm::solver::NativeLinearEquationSolver<double>());
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
storm::modelchecker::prctl::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Mdp<double> const & mdp) {
    //return new storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double>(mdp);
	return new storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double>(mdp);
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
			std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
        	modelchecker.check(*formula);
            delete formula;
			std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
			std::cout << "Checking the formula took " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms." << std::endl;
        }
	}
}

/*!
 * Handles the counterexample generation control.
 *
 * @param parser An AutoParser to get the model from.
 */
 void generateCounterExample(std::shared_ptr<storm::models::AbstractModel<double>> model) {
	LOG4CPLUS_INFO(logger, "Starting counterexample generation.");
	LOG4CPLUS_INFO(logger, "Testing inputs...");

	storm::settings::Settings* s  = storm::settings::Settings::getInstance();

	// First test output directory.
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

 	// Differentiate between model types.
	if(model->getType() != storm::models::DTMC) {
		LOG4CPLUS_ERROR(logger, "Counterexample generation for the selected model type is not supported.");
		return;
	}

	// Get the Dtmc back from the AbstractModel
	// Note that the ownership of the object referenced by dtmc lies at the main function.
	// Thus, it must not be deleted.
	storm::models::Dtmc<double> dtmc = *(model->as<storm::models::Dtmc<double>>());
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
		storm::storage::BitVector result = stateForm.check(*createPrctlModelChecker(dtmc));
		logger.getAppender("mainFileAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);

		if((result & dtmc.getInitialStates()).getNumberOfSetBits() == dtmc.getInitialStates().getNumberOfSetBits()) {
			std::cout << "Formula is satisfied. Can not generate counterexample.\n\n" << std::endl;
			LOG4CPLUS_INFO(logger, "Formula is satisfied. Can not generate counterexample.");
			delete formula;
			continue;
		}

		// Generate counterexample
		storm::models::Dtmc<double> counterExample = storm::counterexamples::PathBasedSubsystemGenerator<double>::computeCriticalSubsystem(dtmc, stateForm);

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
        
        // If requested by the user, we install a timeout signal to abort computation.
		storm::settings::Settings* s = storm::settings::Settings::getInstance();
        uint_fast64_t timeout = s->getOptionByLongName("timeout").getArgument(0).getValueAsUnsignedInteger();
        if (timeout != 0) {
			stormSetAlarm(timeout);
        }
        
		// Execution Time measurement, start
		std::chrono::high_resolution_clock::time_point executionStart = std::chrono::high_resolution_clock::now();

		// Now, the settings are received and the specified model is parsed. The actual actions taken depend on whether
        // the model was provided in explicit or symbolic format.
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

			std::shared_ptr<storm::models::AbstractModel<double>> model = storm::parser::AutoParser::parseModel(chosenTransitionSystemFile, chosenLabelingFile, chosenStateRewardsFile, chosenTransitionRewardsFile);

			// Model Parsing Time Measurement, End
			std::chrono::high_resolution_clock::time_point parsingEnd = std::chrono::high_resolution_clock::now();
			std::cout << "Parsing the given model took " << std::chrono::duration_cast<std::chrono::milliseconds>(parsingEnd - executionStart).count() << " milliseconds." << std::endl;

            if (s->isSet("exportdot")) {
                std::ofstream outputFileStream;
                outputFileStream.open(s->getOptionByLongName("exportdot").getArgument(0).getValueAsString(), std::ofstream::out);
                model->writeDotToStream(outputFileStream);
                outputFileStream.close();
            }
            
			// Should there be a counterexample generated in case the formula is not satisfied?
			if(s->isSet("counterexample")) {
				// Counterexample Time Measurement, Start
				std::chrono::high_resolution_clock::time_point counterexampleStart = std::chrono::high_resolution_clock::now();

				generateCounterExample(model);
			
				// Counterexample Time Measurement, End
				std::chrono::high_resolution_clock::time_point counterexampleEnd = std::chrono::high_resolution_clock::now();
				std::cout << "Generating the counterexample took " << std::chrono::duration_cast<std::chrono::milliseconds>(counterexampleEnd - counterexampleStart).count() << " milliseconds." << std::endl;
			} else {
				// Depending on the model type, the appropriate model checking procedure is chosen.
				storm::modelchecker::prctl::AbstractModelChecker<double>* modelchecker = nullptr;
				model->printModelInformationToStream(std::cout);
                
				// Modelchecking Time Measurement, Start
				std::chrono::high_resolution_clock::time_point modelcheckingStart = std::chrono::high_resolution_clock::now();

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
                case storm::models::MA: {
                    LOG4CPLUS_INFO(logger, "Model is a Markov automaton.");
                    storm::models::MarkovAutomaton<double> markovAutomaton = *model->as<storm::models::MarkovAutomaton<double>>();
                    markovAutomaton.close();
                    storm::modelchecker::csl::SparseMarkovAutomatonCslModelChecker<double> mc(markovAutomaton);
//                    std::cout << mc.checkExpectedTime(true, markovAutomaton->getLabeledStates("goal")) << std::endl;
//                    std::cout << mc.checkExpectedTime(false, markovAutomaton->getLabeledStates("goal")) << std::endl;
                    std::cout << mc.checkLongRunAverage(true, markovAutomaton.getLabeledStates("goal")) << std::endl;
                    std::cout << mc.checkLongRunAverage(false, markovAutomaton.getLabeledStates("goal")) << std::endl;
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

				// Modelchecking Time Measurement, End
				std::chrono::high_resolution_clock::time_point modelcheckingEnd = std::chrono::high_resolution_clock::now();
				std::cout << "Running the ModelChecker took " << std::chrono::duration_cast<std::chrono::milliseconds>(modelcheckingEnd - modelcheckingStart).count() << " milliseconds." << std::endl;
			}
		} else if (s->isSet("symbolic")) {
			// Program Translation Time Measurement, Start
			std::chrono::high_resolution_clock::time_point programTranslationStart = std::chrono::high_resolution_clock::now();

            // First, we build the model using the given symbolic model description and constant definitions.
            std::string const& programFile = s->getOptionByLongName("symbolic").getArgument(0).getValueAsString();
            std::string const& constants = s->getOptionByLongName("constants").getArgument(0).getValueAsString();
            storm::prism::Program program = storm::parser::PrismParser::parse(programFile);
            std::shared_ptr<storm::models::AbstractModel<double>> model = storm::adapters::ExplicitModelAdapter<double>::translateProgram(program, constants);
            model->printModelInformationToStream(std::cout);
            
			// Program Translation Time Measurement, End
			std::chrono::high_resolution_clock::time_point programTranslationEnd = std::chrono::high_resolution_clock::now();
			std::cout << "Parsing and translating the Symbolic Input took " << std::chrono::duration_cast<std::chrono::milliseconds>(programTranslationEnd - programTranslationStart).count() << " milliseconds." << std::endl;

            if (s->isSet("mincmd")) {
                if (model->getType() != storm::models::MDP) {
                    LOG4CPLUS_ERROR(logger, "Minimal command counterexample generation is only supported for models of type MDP.");
                    throw storm::exceptions::InternalTypeErrorException() << "Minimal command counterexample generation is only supported for models of type MDP.";
                }
                
                std::shared_ptr<storm::models::Mdp<double>> mdp = model->as<storm::models::Mdp<double>>();
                
                // Determine whether we are required to use the MILP-version or the SAT-version.
                bool useMILP = s->getOptionByLongName("mincmd").getArgumentByName("method").getValueAsString() == "milp";
                
				// MinCMD Time Measurement, Start
				std::chrono::high_resolution_clock::time_point minCmdStart = std::chrono::high_resolution_clock::now();

                // Now parse the property file and receive the list of parsed formulas.
                std::string const& propertyFile = s->getOptionByLongName("mincmd").getArgumentByName("propertyFile").getValueAsString();
                std::list<storm::property::prctl::AbstractPrctlFormula<double>*> formulaList = storm::parser::PrctlFileParser(propertyFile);

                // Now generate the counterexamples for each formula.
                for (storm::property::prctl::AbstractPrctlFormula<double>* formulaPtr : formulaList) {
                    if (useMILP) {
                        storm::counterexamples::MILPMinimalLabelSetGenerator<double>::computeCounterexample(program, *mdp, formulaPtr);
                    } else {
                        // storm::counterexamples::SMTMinimalCommandSetGenerator<double>::computeCounterexample(program, constants, *mdp, formulaPtr);
                    }
                    
                    // Once we are done with the formula, delete it.
                    delete formulaPtr;
                }

				// MinCMD Time Measurement, End
				std::chrono::high_resolution_clock::time_point minCmdEnd = std::chrono::high_resolution_clock::now();
				std::cout << "Minimal command Counterexample generation took " << std::chrono::duration_cast<std::chrono::milliseconds>(minCmdStart - minCmdEnd).count() << " milliseconds." << std::endl;
            } else if (s->isSet("prctl")) {
				// Depending on the model type, the appropriate model checking procedure is chosen.
				storm::modelchecker::prctl::AbstractModelChecker<double>* modelchecker = nullptr;
                
				// Modelchecking Time Measurement, Start
				std::chrono::high_resolution_clock::time_point modelcheckingStart = std::chrono::high_resolution_clock::now();

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

				// Modelchecking Time Measurement, End
				std::chrono::high_resolution_clock::time_point modelcheckingEnd = std::chrono::high_resolution_clock::now();
				std::cout << "Running the PRCTL ModelChecker took " << std::chrono::duration_cast<std::chrono::milliseconds>(modelcheckingEnd - modelcheckingStart).count() << " milliseconds." << std::endl;
            }
        }

		// Execution Time Measurement, End
		std::chrono::high_resolution_clock::time_point executionEnd = std::chrono::high_resolution_clock::now();
		std::cout << "Complete execution took " << std::chrono::duration_cast<std::chrono::milliseconds>(executionEnd - executionStart).count() << " milliseconds." << std::endl;
        
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
