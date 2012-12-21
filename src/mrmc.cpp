/*
 *	MRMC - C++ Rebuild
 *	
 *	MRMC is a model checker for discrete-time and continuous-time Markov
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

#include "mrmc-config.h"
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
	mrmc::settings::Settings* s = mrmc::settings::instance();
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender(s->getString("logfile")));
	fileLogAppender->setName("mainFileAppender");
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %F:%L: %m%n")));
	logger.addAppender(fileLogAppender);
}

/*!
 * Prints the header.
 */
void printHeader(const int argc, const char* argv[]) {
	std::cout << "MRMC" << std::endl;
	std::cout << "====" << std::endl << std::endl;

	std::cout << "Version: 1.0" << std::endl;
	// "Compute" the command line argument string with which MRMC was invoked.
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
	mrmc::settings::Settings* s = nullptr;
	try {
		mrmc::settings::Settings::registerModule<mrmc::modelChecker::GmmxxDtmcPrctlModelChecker<double>>();
		s = mrmc::settings::newInstance(argc, argv, nullptr);
	} catch (mrmc::exceptions::InvalidSettingsException& e) {
		std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
		std::cout << std::endl << mrmc::settings::help;
		delete s;
		return 1;
	}
	
	if (s->isSet("help")) {
		std::cout << mrmc::settings::help;
		delete s;
		return false;
	}
	if (s->isSet("test-prctl")) {
		mrmc::parser::PrctlParser parser(s->getString("test-prctl").c_str());
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
	if (mrmc::settings::instance() != nullptr) {
		delete mrmc::settings::instance();
	}
}

/*!
 * Simple testing procedure.
 */
void testChecking() {
	mrmc::settings::Settings* s = mrmc::settings::instance();
	mrmc::parser::DtmcParser dtmcParser(s->getString("trafile"), s->getString("labfile"));
	std::shared_ptr<mrmc::models::Dtmc<double>> dtmc = dtmcParser.getDtmc();

	dtmc->printModelInformationToStream(std::cout);

	mrmc::formula::Ap<double>* trueFormula = new mrmc::formula::Ap<double>("true");
	mrmc::formula::Ap<double>* observe0Greater1Formula = new mrmc::formula::Ap<double>("observe0Greater1");
	mrmc::formula::Until<double>* untilFormula = new mrmc::formula::Until<double>(trueFormula, observe0Greater1Formula);
	mrmc::formula::ProbabilisticNoBoundsOperator<double>* probFormula = new mrmc::formula::ProbabilisticNoBoundsOperator<double>(untilFormula);
	mrmc::modelChecker::GmmxxDtmcPrctlModelChecker<double>* mc = new mrmc::modelChecker::GmmxxDtmcPrctlModelChecker<double>(*dtmc);
	mc->check(*probFormula);

	delete mc;
	delete probFormula;
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
