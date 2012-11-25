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

#include "mrmc-config.h"
#include "src/models/dtmc.h"
#include "src/sparse/static_sparse_matrix.h"
#include "src/models/atomic_propositions_labeling.h"
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
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%s} (%r ms) - %F:%L : %m%n")));
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

	LOG4CPLUS_INFO(logger, "This is the Markov Reward Model Checker (MRMC) by i2 of RWTH Aachen university.");

	try {
		s = new mrmc::settings::Settings(argc, argv, nullptr);
	}
	catch (mrmc::exceptions::InvalidSettings&) {
		LOG4CPLUS_FATAL(logger, "Could not recover from settings error, terminating.");
		delete s;
		return 1;
	}
	
	if (s->isSet("help"))
	{
		std::cout << s->getHelpForCommandline() << std::endl;
		return 0;
	}
	if (s->isSet("help-config"))
	{
		std::cout << s->getHelpForConfigfile() << std::endl;
		return 0;
	}

	mrmc::sparse::StaticSparseMatrix<double>* probMatrix = mrmc::parser::read_tra_file(s->getString("trafile").c_str());
	mrmc::models::AtomicPropositionsLabeling* labeling = mrmc::parser::read_lab_file(probMatrix->getRowCount(), s->getString("labfile").c_str());
	mrmc::models::Dtmc<double> dtmc(probMatrix, labeling);

	dtmc.printModelInformationToStream(std::cout);

	if (s != nullptr) {
		delete s;
	}

	LOG4CPLUS_INFO(logger, "Nothing more to do, exiting.");

	return 0;
}

