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
#include "src/storage/SquareSparseMatrix.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/modelChecker/EigenDtmcPrctlModelChecker.h"
#include "src/modelChecker/GmmxxDtmcPrctlModelChecker.h"
#include "src/parser/DtmcParser.h"
#include "src/parser/PrctlParser.h"
#include "src/solver/GraphAnalyzer.h"
#include "src/utility/Settings.h"
#include "src/formula/Formulas.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

#include "src/exceptions/InvalidSettingsException.h"

log4cplus::Logger logger;

/*!
 * Initializes the logging framework and sets up logging to console.
 */
void initializeLogger() {
	logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
	logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
	log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
	consoleLogAppender->setName("mainConsoleAppender");
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
	std::cout << "====" << std::endl << std::endl;

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
		storm::settings::Settings::registerModule<storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>>();
		s = storm::settings::newInstance(argc, argv, nullptr);
	} catch (storm::exceptions::InvalidSettingsException& e) {
		std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
		std::cout << std::endl << storm::settings::help;
		delete s;
		return false;
	}
	
	if (s->isSet("help")) {
		std::cout << storm::settings::help;
		delete s;
		return false;
	}
	if (s->isSet("test-prctl")) {
		storm::parser::PrctlParser parser(s->getString("test-prctl").c_str());
		delete s;
		return false;
	}
	
	if (!s->isSet("verbose") && !s->isSet("logfile")) {
		logger.setLogLevel(log4cplus::FATAL_LOG_LEVEL);
	} else if (!s->isSet("verbose")) {
		logger.removeAppender("mainConsoleAppender");
		setUpFileLogging();
	} else if (!s->isSet("logfile")) {
		LOG4CPLUS_INFO(logger, "Enable verbose mode, log output gets printed to console.");
	} else {
		setUpFileLogging();
		LOG4CPLUS_INFO(logger, "Enable verbose mode, log output gets printed to console.");
	}

	return true;
}

/*!
 * Function to perform some cleanup.
 */
void cleanUp() {
	if (storm::settings::instance() != nullptr) {
		delete storm::settings::instance();
	}
}

/*!
 * Simple testing procedure.
 */
void testChecking() {
	storm::settings::Settings* s = storm::settings::instance();
	storm::parser::DtmcParser dtmcParser(s->getString("trafile"), s->getString("labfile"), s->getString("staterew"), s->getString("transrew"));
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = dtmcParser.getDtmc();

	dtmc->printModelInformationToStream(std::cout);

	storm::formula::Ap<double>* observe0Greater1Formula = new storm::formula::Ap<double>("one");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(observe0Greater1Formula);
	storm::formula::ProbabilisticNoBoundsOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundsOperator<double>(eventuallyFormula);

	storm::formula::Ap<double>* done = new storm::formula::Ap<double>("done");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(done);
	storm::formula::RewardNoBoundsOperator<double>* rewardFormula = new storm::formula::RewardNoBoundsOperator<double>(reachabilityRewardFormula);

	storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>(*dtmc);
	mc->check(*probFormula);
	mc->check(*rewardFormula);

	delete mc;
	delete probFormula;
	delete rewardFormula;
}

/*!
 * Main entry point.
 */
int main(const int argc, const char* argv[]) {
	initializeLogger();
	if (!parseOptions(argc, argv)) {
		return 0;
	}
	printHeader(argc, argv);

	// testChecking();

	cleanUp();
	return 0;
}
