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
#include "src/parser/readLabFile.h"
#include "src/parser/readTraFile.h"
#include "src/parser/readPrctlFile.h"
#include "src/solver/GraphAnalyzer.h"
#include "src/utility/Settings.h"
#include "src/formula/Formulas.h"
#include "src/exceptions/NoConvergence.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

#include "src/exceptions/InvalidSettings.h"

log4cplus::Logger logger;

/*!
 * Initializes the logging framework.
 */
void setUpFileLogging() {
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender("mrmc.log"));
	fileLogAppender->setName("mainFileAppender");
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %F:%L : %m%n")));
	logger = log4cplus::Logger::getInstance("mainLogger");
	logger.addAppender(fileLogAppender);
}

void setUpConsoleLogging() {
	log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
	consoleLogAppender->setName("mainConsoleAppender");
	consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %b:%L : %m%n")));
	logger.addAppender(consoleLogAppender);
}

int main(const int argc, const char* argv[]) {
	setUpFileLogging();

	mrmc::settings::Settings* s = nullptr;
	
	LOG4CPLUS_INFO(logger, "This is the Markov Reward Model Checker (MRMC) by i2 of RWTH Aachen University.");

	// "Compute" the command line argument string with which MRMC was invoked and log as diagnostic information.
	std::stringstream commandStream;
	for (int i = 0; i < argc; ++i) {
		commandStream << argv[i] << " ";
	}
	LOG4CPLUS_INFO(logger, "MRMC command invoked " << commandStream.str());

	try {
		mrmc::settings::Settings::registerModule<mrmc::modelChecker::GmmxxDtmcPrctlModelChecker<double> >();
		s = mrmc::settings::newInstance(argc, argv, nullptr);
	} catch (mrmc::exceptions::InvalidSettings& e) {
		LOG4CPLUS_FATAL(logger, "InvalidSettings error: " << e.what() << ".");
		LOG4CPLUS_FATAL(logger, "Could not recover from settings error, terminating.");
		std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
		std::cout << std::endl << mrmc::settings::help;
		delete s;
		return 1;
	}
	
	if (s->isSet("help")) {
		std::cout << mrmc::settings::help;
		delete s;
		return 0;
	}
	if (s->isSet("test-prctl")) {
		mrmc::parser::PRCTLParser parser(s->getString("test-prctl").c_str());
		delete s;
		return 0;
	}
	
	if (s->isSet("verbose"))
	{
		setUpConsoleLogging();
		LOG4CPLUS_INFO(logger, "Enable verbose mode, log output gets printed to console.");
	}

	mrmc::parser::TraParser traparser(s->getString("trafile").c_str());	
	mrmc::parser::LabParser labparser(traparser.getMatrix()->getRowCount(), s->getString("labfile").c_str());
	mrmc::models::Dtmc<double> dtmc(traparser.getMatrix(), labparser.getLabeling());

	dtmc.printModelInformationToStream(std::cout);

	if (s != nullptr) {
		delete s;
	}
	
	LOG4CPLUS_INFO(logger, "Nothing more to do, exiting.");

	return 0;
}
