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
#include "src/parser/AutoParser.h"
#include "src/parser/PrctlParser.h"
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
		storm::settings::Settings::registerModule<storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>>();
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
	if (s->isSet("test-prctl")) {
		storm::parser::PrctlParser parser(s->getString("test-prctl").c_str());
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
	// nothing here
}

void testCheckingDie(storm::models::Dtmc<double>& dtmc) {
	storm::formula::Ap<double>* oneFormula = new storm::formula::Ap<double>("one");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(oneFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	storm::formula::Ap<double>* done = new storm::formula::Ap<double>("done");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(done);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
	mc->check(*probFormula);
	mc->check(*rewardFormula);

	delete mc;
	delete probFormula;
	delete rewardFormula;
}

void testCheckingCrowds(storm::models::Dtmc<double>& dtmc) {
	storm::formula::Ap<double>* observe0Greater1Formula = new storm::formula::Ap<double>("observe0Greater1");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(observe0Greater1Formula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
	mc->check(*probFormula);
	delete probFormula;

	storm::formula::Ap<double>* observeIGreater1Formula = new storm::formula::Ap<double>("observeIGreater1");
	eventuallyFormula = new storm::formula::Eventually<double>(observeIGreater1Formula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	storm::formula::Ap<double>* observeOnlyTrueSenderFormula = new storm::formula::Ap<double>("observeOnlyTrueSender");
	eventuallyFormula = new storm::formula::Eventually<double>(observeOnlyTrueSenderFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	delete mc;
}

void testCheckingSynchronousLeader(storm::models::Dtmc<double>& dtmc, uint_fast64_t n) {
	storm::formula::Ap<double>* electedFormula = new storm::formula::Ap<double>("elected");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(electedFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelChecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
	mc->check(*probFormula);
	delete probFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	storm::formula::BoundedUntil<double>* boundedUntilFormula = new storm::formula::BoundedUntil<double>(new storm::formula::Ap<double>("true"), electedFormula, 1);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedUntilFormula);

	for (uint_fast64_t L = 1; L < 5; ++L) {
		boundedUntilFormula->setBound(L*(n + 1));
		mc->check(*probFormula);
	}
	delete probFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(electedFormula);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
}

void testCheckingDice(storm::models::Mdp<double> mdp) {

}

/*!
 * Simple testing procedure.
 */
void testChecking() {
	storm::settings::Settings* s = storm::settings::instance();
	storm::parser::AutoParser<double> parser(s->getString("trafile"), s->getString("labfile"), s->getString("staterew"), s->getString("transrew"));

	if (parser.getType() == storm::models::DTMC) {
		std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();
		dtmc->printModelInformationToStream(std::cout);

		// testCheckingDie(*dtmc);
		// testCheckingCrowds(*dtmc);
		// testCheckingSynchronousLeader(*dtmc, 4);
	}
	else if (parser.getType() == storm::models::MDP) {
		std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
		mdp->printModelInformationToStream(std::cout);

		// testCheckingDice(*mdp);
	} else {
		std::cout << "Input is neither a DTMC nor an MDP." << std::endl;
	}
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

	testChecking();

	cleanUp();
	return 0;
}
