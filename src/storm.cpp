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
#include "src/formula/Prctl.h"

#include "src/parser/PrctlFileParser.h"

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

	storm::property::prctl::Ap<double>* oneFormula = new storm::property::prctl::Ap<double>("one");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(oneFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	oneFormula = new storm::property::prctl::Ap<double>("two");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(oneFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	oneFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(oneFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	storm::property::prctl::Ap<double>* done = new storm::property::prctl::Ap<double>("done");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(done);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	mc->check(*rewardFormula);
	delete rewardFormula;
	delete mc;
}

void testCheckingCrowds(storm::models::Dtmc<double>& dtmc) {
	storm::property::prctl::Ap<double>* observe0Greater1Formula = new storm::property::prctl::Ap<double>("observe0Greater1");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(observe0Greater1Formula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
	mc->check(*probFormula);
	delete probFormula;

	storm::property::prctl::Ap<double>* observeIGreater1Formula = new storm::property::prctl::Ap<double>("observeIGreater1");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(observeIGreater1Formula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	storm::property::prctl::Ap<double>* observeOnlyTrueSenderFormula = new storm::property::prctl::Ap<double>("observeOnlyTrueSender");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(observeOnlyTrueSenderFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	mc->check(*probFormula);
	delete probFormula;

	delete mc;
}

void testCheckingSynchronousLeader(storm::models::Dtmc<double>& dtmc, uint_fast64_t n) {
	storm::property::prctl::Ap<double>* electedFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(electedFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
	mc->check(*probFormula);
	delete probFormula;

	electedFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedUntil<double>* boundedUntilFormula = new storm::property::prctl::BoundedUntil<double>(new storm::property::prctl::Ap<double>("true"), electedFormula, n * 4);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedUntilFormula);

	mc->check(*probFormula);
	delete probFormula;

	electedFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(electedFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
}

void testCheckingDice(storm::models::Mdp<double>& mdp) {
	storm::property::prctl::Ap<double>* twoFormula = new storm::property::prctl::Ap<double>("two");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxMdpPrctlModelChecker<double>(mdp);

	mc->check(*probFormula);
	delete probFormula;


	twoFormula = new storm::property::prctl::Ap<double>("two");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::property::prctl::Ap<double>("four");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::property::prctl::Ap<double>("four");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::property::prctl::Ap<double>("five");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::property::prctl::Ap<double>("five");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::property::prctl::Ap<double>("six");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	twoFormula = new storm::property::prctl::Ap<double>("six");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(twoFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	storm::property::prctl::Ap<double>* doneFormula = new storm::property::prctl::Ap<double>("done");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(doneFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	mc->check(*rewardFormula);
	delete rewardFormula;

	doneFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(doneFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
}

void testCheckingAsynchLeader(storm::models::Mdp<double>& mdp) {
	storm::property::prctl::Ap<double>* electedFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(electedFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probMinFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxMdpPrctlModelChecker<double>(mdp);

	mc->check(*probMinFormula);
	delete probMinFormula;

	electedFormula = new storm::property::prctl::Ap<double>("elected");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(electedFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probMaxFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probMaxFormula);
	delete probMaxFormula;

	electedFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(electedFormula, 25);
	probMinFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	mc->check(*probMinFormula);
	delete probMinFormula;

	electedFormula = new storm::property::prctl::Ap<double>("elected");
	boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(electedFormula, 25);
	probMaxFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	mc->check(*probMaxFormula);
	delete probMaxFormula;

	electedFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(electedFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	mc->check(*rewardFormula);
	delete rewardFormula;

	electedFormula = new storm::property::prctl::Ap<double>("elected");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(electedFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
}

void testCheckingConsensus(storm::models::Mdp<double>& mdp) {
	storm::property::prctl::Ap<double>* finishedFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(finishedFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double>* mc = new storm::modelchecker::GmmxxMdpPrctlModelChecker<double>(mdp);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::Ap<double>* allCoinsEqual0Formula = new storm::property::prctl::Ap<double>("all_coins_equal_0");
	storm::property::prctl::And<double>* conjunctionFormula = new storm::property::prctl::And<double>(finishedFormula, allCoinsEqual0Formula);
	eventuallyFormula = new storm::property::prctl::Eventually<double>(conjunctionFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::Ap<double>* allCoinsEqual1Formula = new storm::property::prctl::Ap<double>("all_coins_equal_1");
	conjunctionFormula = new storm::property::prctl::And<double>(finishedFormula, allCoinsEqual1Formula);
	eventuallyFormula = new storm::property::prctl::Eventually<double>(conjunctionFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::Ap<double>* agree = new storm::property::prctl::Ap<double>("agree");
	storm::property::prctl::Not<double>* notAgree = new storm::property::prctl::Not<double>(agree);
	conjunctionFormula = new storm::property::prctl::And<double>(finishedFormula, notAgree);
	eventuallyFormula = new storm::property::prctl::Eventually<double>(conjunctionFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(finishedFormula, 50);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::property::prctl::Ap<double>("finished");
	boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(finishedFormula, 50);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	mc->check(*probFormula);
	delete probFormula;

	finishedFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(finishedFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	mc->check(*rewardFormula);
	delete rewardFormula;

	finishedFormula = new storm::property::prctl::Ap<double>("finished");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(finishedFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	mc->check(*rewardFormula);
	delete rewardFormula;

	delete mc;
}

/*!
 * Check method for DTMCs
 * @param dtmc Reference to the DTMC to check
 */
void checkDtmc(std::shared_ptr<storm::models::Dtmc<double>> dtmc) {
	dtmc->printModelInformationToStream(std::cout);
}

void checkMdp(std::shared_ptr<storm::models::Mdp<double>> mdp) {
	mdp->printModelInformationToStream(std::cout);
}

/*!
 * Simple testing procedure.
 */
void testChecking() {
	storm::settings::Settings* s = storm::settings::instance();
	storm::parser::AutoParser<double> parser(s->getString("trafile"), s->getString("labfile"), s->getString("staterew"), s->getString("transrew"));

	if (s->isSet("prctl")) {
		LOG4CPLUS_INFO(logger, "Parsing prctl file"+ s->getString("prctl"));
		storm::parser::PrctlFileParser fileParser;
		std::list<storm::property::prctl::AbstractPrctlFormula<double>*> formulaList = fileParser.parseFormulas(s->getString("prctl"));

	}


	switch (parser.getType()) {
	case storm::models::DTMC:
		LOG4CPLUS_INFO(logger, "Model was detected as DTMC");
		checkDtmc(parser.getModel<storm::models::Dtmc<double>>());
		break;
	case storm::models::MDP:
		LOG4CPLUS_INFO(logger, "Model was detected as MDP");
		checkMdp(parser.getModel<storm::models::Mdp<double>>());
		break;
	default:
		LOG4CPLUS_ERROR(logger, "The model type could not be determined correctly.");
		break;
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
