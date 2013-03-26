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
#include "src/utility/Settings.h"
#include "src/utility/ErrorHandling.h"
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
	if (s->isSet("test-prctl")) {
		storm::parser::PrctlParser parser(s->getString("test-prctl").c_str());
		return false;
	}
	
	if (s->isSet("verbose")) {
		logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);
		LOG4CPLUS_INFO(logger, "Enable verbose mode, log output gets printed to console.");
	}
	if (s->isSet("debug")) {
		logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
		logger.getAppender("mainConsoleAppender")->setThreshold(log4cplus::DEBUG_LOG_LEVEL);
		LOG4CPLUS_DEBUG(logger, "Enable very verbose mode, log output gets printed to console.");
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
	// nothing here
}

void testCheckingDie(storm::models::Dtmc<double>& dtmc) {
	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);

	storm::formula::Ap<double>* oneFormula = new storm::formula::Ap<double>("one");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(oneFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	oneFormula = new storm::formula::Ap<double>("two");
	eventuallyFormula = new storm::formula::Eventually<double>(oneFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	oneFormula = new storm::formula::Ap<double>("three");
	eventuallyFormula = new storm::formula::Eventually<double>(oneFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	storm::formula::Ap<double>* done = new storm::formula::Ap<double>("done");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(done);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	mc->check(*rewardFormula);
	delete rewardFormula;
	delete mc;
}

void testCheckingCrowds(storm::models::Dtmc<double>& dtmc) {
	storm::formula::Ap<double>* observe0Greater1Formula = new storm::formula::Ap<double>("observe0Greater1");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(observe0Greater1Formula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
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

	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
	mc->check(*probFormula);
	delete probFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	storm::formula::BoundedUntil<double>* boundedUntilFormula = new storm::formula::BoundedUntil<double>(new storm::formula::Ap<double>("true"), electedFormula, n * 4);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedUntilFormula);

	mc->check(*probFormula);
	delete probFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(electedFormula);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
}

void testCheckingDice(storm::models::Mdp<double>& mdp) {
	storm::formula::Ap<double>* twoFormula = new storm::formula::Ap<double>("two");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxMdpPrctlModelChecker<double>(mdp);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("two");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("three");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("three");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("four");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("four");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("five");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("five");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("six");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::formula::Ap<double>("six");
	eventuallyFormula = new storm::formula::Eventually<double>(twoFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	storm::formula::Ap<double>* doneFormula = new storm::formula::Ap<double>("done");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(doneFormula);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	mc->check(*rewardFormula);
	delete rewardFormula;

	doneFormula = new storm::formula::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(doneFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
}

void testCheckingAsynchLeader(storm::models::Mdp<double>& mdp) {
	storm::formula::Ap<double>* electedFormula = new storm::formula::Ap<double>("elected");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(electedFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probMinFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxMdpPrctlModelChecker<double>(mdp);

	mc->check(*probMinFormula);
	delete probMinFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	eventuallyFormula = new storm::formula::Eventually<double>(electedFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probMaxFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probMaxFormula);
	delete probMaxFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	storm::formula::BoundedEventually<double>* boundedEventuallyFormula = new storm::formula::BoundedEventually<double>(electedFormula, 25);
	probMinFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	mc->check(*probMinFormula);
	delete probMinFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	boundedEventuallyFormula = new storm::formula::BoundedEventually<double>(electedFormula, 25);
	probMaxFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	mc->check(*probMaxFormula);
	delete probMaxFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(electedFormula);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	mc->check(*rewardFormula);
	delete rewardFormula;

	electedFormula = new storm::formula::Ap<double>("elected");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(electedFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
}

void testCheckingConsensus(storm::models::Mdp<double>& mdp) {
	storm::formula::Ap<double>* finishedFormula = new storm::formula::Ap<double>("finished");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(finishedFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxMdpPrctlModelChecker<double>(mdp);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::formula::Ap<double>("finished");
	storm::formula::Ap<double>* allCoinsEqual0Formula = new storm::formula::Ap<double>("all_coins_equal_0");
	storm::formula::And<double>* conjunctionFormula = new storm::formula::And<double>(finishedFormula, allCoinsEqual0Formula);
	eventuallyFormula = new storm::formula::Eventually<double>(conjunctionFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::formula::Ap<double>("finished");
	storm::formula::Ap<double>* allCoinsEqual1Formula = new storm::formula::Ap<double>("all_coins_equal_1");
	conjunctionFormula = new storm::formula::And<double>(finishedFormula, allCoinsEqual1Formula);
	eventuallyFormula = new storm::formula::Eventually<double>(conjunctionFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::formula::Ap<double>("finished");
	storm::formula::Ap<double>* agree = new storm::formula::Ap<double>("agree");
	storm::formula::Not<double>* notAgree = new storm::formula::Not<double>(agree);
	conjunctionFormula = new storm::formula::And<double>(finishedFormula, notAgree);
	eventuallyFormula = new storm::formula::Eventually<double>(conjunctionFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::formula::Ap<double>("finished");
	storm::formula::BoundedEventually<double>* boundedEventuallyFormula = new storm::formula::BoundedEventually<double>(finishedFormula, 50);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::formula::Ap<double>("finished");
	boundedEventuallyFormula = new storm::formula::BoundedEventually<double>(finishedFormula, 50);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::formula::Ap<double>("finished");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(finishedFormula);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	mc->check(*rewardFormula);
	delete rewardFormula;

	finishedFormula = new storm::formula::Ap<double>("finished");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(finishedFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
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
		// testCheckingSynchronousLeader(*dtmc, 6);
	} else if (parser.getType() == storm::models::MDP) {
		std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
		mdp->printModelInformationToStream(std::cout);

		// testCheckingDice(*mdp);
		// testCheckingAsynchLeader(*mdp);
		// testCheckingConsensus(*mdp);
	} else {
		std::cout << "Input is neither a DTMC nor an MDP." << std::endl;
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
	if (!parseOptions(argc, argv)) {
		return 0;
	}
	setUp();

	try {
		LOG4CPLUS_INFO(logger, "StoRM was invoked.");

		testChecking();

		cleanUp();

		LOG4CPLUS_INFO(logger, "StoRM quit.");

		return 0;
	} catch (std::exception& e) {
		LOG4CPLUS_FATAL(logger, "An exception was thrown but not catched. All we can do now is show it to you and die in peace...");
		LOG4CPLUS_FATAL(logger, "\t" << e.what());
	}
	return 1;
}
