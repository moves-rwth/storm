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

#include <iostream>
#include <cstdio>
#include <sstream>

#include "mrmc-config.h"
#include "src/models/Dtmc.h"
#include "src/storage/SquareSparseMatrix.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/parser/read_lab_file.h"
#include "src/parser/read_tra_file.h"
#include "src/utility/settings.h"
#include "Eigen/Sparse"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

#include "src/exceptions/InvalidSettings.h"

log4cplus::Logger logger;

/*!
 * Initializes the logging framework.
 */
void setUpLogging() {
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender("mrmc.log"));
	fileLogAppender->setName("mainFileAppender");
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M} (%r ms) - %F:%L : %m%n")));
	logger = log4cplus::Logger::getInstance("mainLogger");
	logger.addAppender(fileLogAppender);

	// Uncomment these lines to enable console logging output
	// log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
	// consoleLogAppender->setName("mainConsoleAppender");
	// consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%s} (%r ms) - %F:%L : %m%n")));
	// logger.addAppender(consoleLogAppender);
}

int main(const int argc, const char* argv[]) {
	setUpLogging();

	mrmc::settings::Settings* s = NULL;
	
	LOG4CPLUS_INFO(logger, "This is the Markov Reward Model Checker (MRMC) by i2 of RWTH Aachen University.");

	// "Compute" the command line argument string with which MRMC was invoked and log as diagnostic information.
	std::stringstream commandStream;
	for (int i = 0; i < argc; ++i) {
		commandStream << argv[i] << " ";
	}
	LOG4CPLUS_INFO(logger, "MRMC command invoked " << commandStream.str());

	try {
		s = mrmc::settings::Settings::instance(argc, argv, nullptr);
	} catch (mrmc::exceptions::InvalidSettings&) {
		LOG4CPLUS_FATAL(logger, "Could not recover from settings error, terminating.");
		std::cout << std::endl << mrmc::settings::help << std::endl;
		delete s;
		return 1;
	}
	
	if (s->isSet("help")) {
		std::cout << mrmc::settings::help << std::endl;
		return 0;
	}
	if (s->isSet("help-config")) {
		std::cout << mrmc::settings::helpConfigfile << std::endl;
		return 0;
	}

	mrmc::storage::SquareSparseMatrix<double>* probMatrix = mrmc::parser::read_tra_file(s->getString("trafile").c_str());
	mrmc::models::AtomicPropositionsLabeling* labeling = mrmc::parser::read_lab_file(probMatrix->getRowCount(), s->getString("labfile").c_str());
	mrmc::models::Dtmc<double> dtmc(probMatrix, labeling);

	dtmc.printModelInformationToStream(std::cout);

	if (s != nullptr) {
		delete s;
	}

	LOG4CPLUS_INFO(logger, "Nothing more to do, exiting.");

	return 0;
}

