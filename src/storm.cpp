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
#include <cstdio>
#include <sstream>
#include <vector>

#include "storm-config.h"
#include "src/models/Dtmc.h"
#include "src/storage/SparseMatrix.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/modelchecker/EigenDtmcPrctlModelChecker.h"
#include "src/modelchecker/GmmxxDtmcPrctlModelChecker.h"
#include "src/modelchecker/GmmxxMdpPrctlModelChecker.h"
#include "src/parser/AutoParser.h"
#include "src/parser/PrctlParser.h"
//#include "src/solver/GraphAnalyzer.h"
#include "src/utility/Settings.h"
#include "src/utility/ErrorHandling.h"
#include "src/formula/Prctl.h"

#include "src/parser/PrctlFileParser.h"

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

void printUsage() {
#ifndef WINDOWS	
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);

	std::cout << "Memory Usage: " << ru.ru_maxrss << "kB" << std::endl;
	std::cout << "CPU Time: " << ru.ru_utime.tv_sec << "." << std::setw(3) << std::setfill('0') << ru.ru_utime.tv_usec/1000 << " seconds" << std::endl;
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
	storm::settings::Settings* s = storm::settings::instance();
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender(s->getString("logfile")));
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
 * Prints the footer.
 */
void printFooter() {
	std::cout << "Nothing more to do, exiting." << std::endl;
}

/*!
 * Function that parses the command line options.
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return True iff the program should continue to run after parsing the options.
 */
bool parseOptions(const int argc, const char* argv[]) {
	storm::settings::Settings* s = nullptr;
	try {
		storm::settings::Settings::registerModule<storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>>();
		s = storm::settings::newInstance(argc, argv, nullptr);
	} catch (storm::exceptions::InvalidSettingsException& e) {
		std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
		std::cout << std::endl << storm::settings::help;
		return false;
	}

	if (s->isSet("help")) {
		std::cout << storm::settings::help;
		return false;
	}

	if (s->isSet("verbose")) {
		logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);
		LOG4CPLUS_INFO(logger, "Enable verbose mode, log output gets printed to console.");
	}
	if (s->isSet("debug")) {
		logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
		logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::DEBUG_LOG_LEVEL);
		LOG4CPLUS_INFO(logger, "Enable very verbose mode, log output gets printed to console.");
	}
	if (s->isSet("trace")) {
		logger.setLogLevel(log4cplus::TRACE_LOG_LEVEL);
		logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::TRACE_LOG_LEVEL);
		LOG4CPLUS_INFO(logger, "Enable trace mode, log output gets printed to console.");
	}
	if (s->isSet("logfile")) {
		setUpFileLogging();
	}
	return true;
}

void setUp() {
	std::cout.precision(10);
}

/*!
 * Function to perform some cleanup.
 */
void cleanUp() {
	delete storm::utility::cuddUtilityInstance();
}

/*!
 * Factory style creation of new DTMC model checker
 * @param dtmc The Dtmc that the model checker will check
 * @return
 */
storm::modelchecker::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Dtmc<double>& dtmc) {
	storm::settings::Settings* s = storm::settings::instance();
	if (s->getString("matrixlib") == "gmm++") {
		return new storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
	}
	// The control flow should never reach this point, as there is a default setting for matrixlib (gmm++)
	std::string message = "No matrix library suitable for DTMC model checking has been set";
	throw storm::exceptions::InvalidSettingsException() << message;
	return nullptr;
}

/*!
 * Factory style creation of new MDP model checker
 * @param mdp The Dtmc that the model checker will check
 * @return
 */
storm::modelchecker::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Mdp<double>& mdp) {
	storm::settings::Settings* s = storm::settings::instance();
	if (s->getString("matrixlib") == "gmm++") {
		return new storm::modelchecker::GmmxxMdpPrctlModelChecker<double>(mdp);
	}
	// The control flow should never reach this point, as there is a default setting for matrixlib (gmm++)
	std::string message = "No matrix library suitable for MDP model checking has been set";
	throw storm::exceptions::InvalidSettingsException() << message;
	return nullptr;
}

/*!
 * Calls the check method of a model checker for all PRCTL formulas in a given list.
 *
 * @param formulaList The list of PRCTL formulas
 * @param mc the model checker
 */
void checkPrctlFormulasAgainstModel(std::list<storm::property::prctl::AbstractPrctlFormula<double>*>& formulaList,
									storm::modelchecker::AbstractModelChecker<double> const& mc) {
	for ( auto formula : formulaList ) {
		mc.check(*formula);

		//TODO: Should that be done here or in a second iteration through the list?
		delete formula;
	}
	formulaList.clear();
}

/*!
 * Check method for DTMCs
 * @param dtmc Reference to the DTMC to check
 */
void checkMdp(std::shared_ptr<storm::models::Mdp<double>> mdp) {
	mdp->printModelInformationToStream(std::cout);
	storm::settings::Settings* s = storm::settings::instance();
	if (s->isSet("prctl")) {
		LOG4CPLUS_INFO(logger, "Parsing prctl file"+ s->getString("prctl"));
		storm::parser::PrctlFileParser fileParser;
		std::list<storm::property::prctl::AbstractPrctlFormula<double>*> formulaList = fileParser.parseFormulas(s->getString("prctl"));

		storm::modelchecker::AbstractModelChecker<double>* mc = createPrctlModelChecker(*mdp);

		checkPrctlFormulasAgainstModel(formulaList, *mc);

		delete mc;
	}

	if(s->isSet("csl")) {
		LOG4CPLUS_ERROR(logger, "CSL properties cannot be checked on MDPs.");
	}
}

/*!
 * Check method for DTMCs
 * @param dtmc Reference to the DTMC to check
 */
void checkDtmc(std::shared_ptr<storm::models::Dtmc<double>> dtmc) {
	dtmc->printModelInformationToStream(std::cout);
	storm::settings::Settings* s = storm::settings::instance();
	if (s->isSet("prctl")) {
		LOG4CPLUS_INFO(logger, "Parsing prctl file"+ s->getString("prctl"));
		storm::parser::PrctlFileParser fileParser;
		std::list<storm::property::prctl::AbstractPrctlFormula<double>*> formulaList = fileParser.parseFormulas(s->getString("prctl"));

		storm::modelchecker::AbstractModelChecker<double>* mc = createPrctlModelChecker(*dtmc);

		checkPrctlFormulasAgainstModel(formulaList, *mc);

		delete mc;
	}

	if(s->isSet("csl")) {
		LOG4CPLUS_ERROR(logger, "CSL properties cannot be checked on DTMCs.");
	}
}

/*!
 * Main entry point.
 */
int main(const int argc, const char* argv[]) {
	// Catch segfaults and display a backtrace.
	installSignalHandler();

	printHeader(argc, argv);

	initializeLogger();

	setUp();

	try {
		LOG4CPLUS_INFO(logger, "StoRM was invoked.");

		// Parse options
		if (!parseOptions(argc, argv)) {
			// If false is returned, the program execution is stopped here
			// E.g. if the user asked to see the help text
			return 0;
		}

		// Now, the settings are receivd and the model is parsed.
		storm::settings::Settings* s = storm::settings::instance();
		if (s->isSet("explicit")) {
			std::vector<std::string> args = s->get<std::vector<std::string>>("explicit");
			storm::parser::AutoParser<double> parser(args[0], args[1], s->getString("staterew"), s->getString("transrew"));

			LOG4CPLUS_DEBUG(logger, s->getString("matrixlib"));


			// Depending on the model type, the respective model checking procedure is chosen.
			switch (parser.getType()) {
			case storm::models::DTMC:
				LOG4CPLUS_INFO(logger, "Model was detected as DTMC");
				checkDtmc(parser.getModel<storm::models::Dtmc<double>>());
				break;
			case storm::models::MDP:
				LOG4CPLUS_INFO(logger, "Model was detected as MDP");
				checkMdp(parser.getModel<storm::models::Mdp<double>>());
				break;
			case storm::models::CTMC:
			case storm::models::CTMDP:
				// Continuous time model checking is not implemented yet
				LOG4CPLUS_ERROR(logger, "The model type you selected is not supported in this version of storm.");
				break;
			case storm::models::Unknown:
			default:
				LOG4CPLUS_ERROR(logger, "The model type could not be determined correctly.");
				break;
			}
		}
		if (s->isSet("symbolic")) {
			std::string arg = s->getString("symbolic");
			storm::parser::PrismParser parser;
			storm::adapters::ExplicitModelAdapter adapter(parser.parseFile(arg));
			std::shared_ptr<storm::models::AbstractModel<double>> model = adapter.getModel();
			model->printModelInformationToStream(std::cout);
		}

		cleanUp();

		LOG4CPLUS_INFO(logger, "StoRM quit.");

		return 0;
	} catch (std::exception& e) {
		LOG4CPLUS_FATAL(logger, "An exception was thrown but not catched. All we can do now is show it to you and die in peace...");
		LOG4CPLUS_FATAL(logger, "\t" << e.what());
	}
	return 1;
}
