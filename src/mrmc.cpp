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

#include "src/utility/osDetection.h"
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
#include "src/utility/settings.h"
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

	mrmc::settings::Settings* s = NULL;
	
	LOG4CPLUS_INFO(logger, "This is the Markov Reward Model Checker (MRMC) by i2 of RWTH Aachen University.");

	// "Compute" the command line argument string with which MRMC was invoked and log as diagnostic information.
	std::stringstream commandStream;
	for (int i = 0; i < argc; ++i) {
		commandStream << argv[i] << " ";
	}
	LOG4CPLUS_INFO(logger, "MRMC command invoked " << commandStream.str());

	try {
		s = mrmc::settings::newInstance(argc, argv, nullptr);
	} catch (mrmc::exceptions::InvalidSettings&) {
		LOG4CPLUS_FATAL(logger, "Could not recover from settings error, terminating.");
		std::cout << std::endl << mrmc::settings::help;
		delete s;
		return 1;
	}
	
	if (s->isSet("help")) {
		std::cout << mrmc::settings::help;
		delete s;
		return 0;
	}
	if (s->isSet("help-config")) {
		std::cout << mrmc::settings::helpConfigfile;
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

	// Uncomment this if you want to see the first model checking procedure in action. :)
	mrmc::modelChecker::EigenDtmcPrctlModelChecker<double> mc(dtmc);
	mrmc::formula::AP<double>* trueFormula = new mrmc::formula::AP<double>(std::string("true"));
	mrmc::formula::AP<double>* ap = new mrmc::formula::AP<double>(std::string("observe0Greater1"));
	mrmc::formula::Until<double>* until = new mrmc::formula::Until<double>(trueFormula, ap);
	
	std::vector<double>* eigenResult = NULL;
	try {
		eigenResult = mc.checkPathFormula(*until);
	} catch (mrmc::exceptions::NoConvergence& nce) {
		// solver did not converge
		LOG4CPLUS_ERROR(logger, "EigenDtmcPrctlModelChecker did not converge with " << nce.getIterationCount() << " of max. " << nce.getMaxIterationCount() << "Iterations!");
		return -1;
	}
	delete until;

	mrmc::modelChecker::GmmxxDtmcPrctlModelChecker<double> mcG(dtmc);
	mrmc::formula::AP<double>* trueFormulaG = new mrmc::formula::AP<double>(std::string("true"));
	mrmc::formula::AP<double>* apG = new mrmc::formula::AP<double>(std::string("observe0Greater1"));
	mrmc::formula::Until<double>* untilG = new mrmc::formula::Until<double>(trueFormulaG, apG);
	std::vector<double>* gmmResult = mcG.checkPathFormula(*untilG);
	delete untilG;

	if (eigenResult->size() != gmmResult->size()) {
		LOG4CPLUS_ERROR(logger, "Warning: Eigen and GMM produced different results (Eigen: " << eigenResult->size() << ", Gmm: " << gmmResult->size() << ") in size!");
	} else {
		LOG4CPLUS_INFO(logger, "Testing for different entries");
		for (unsigned int i = 0; i < eigenResult->size(); ++i) {
			if (std::abs((eigenResult->at(i) - gmmResult->at(i))) > 0) {
				LOG4CPLUS_ERROR(logger, "Warning: Eigen and GMM produced different results in entry " << i << " (Eigen: " << eigenResult->at(i) << ", Gmm: " << gmmResult->at(i) << ")!");
			}
			if (eigenResult->at(i) != 0.0) {
				LOG4CPLUS_INFO(logger, "Non zero entry " << eigenResult->at(i) << " at " << i);
			}
		}
	}

	/*
	LOG4CPLUS_INFO(logger, "Result: ");
	LOG4CPLUS_INFO(logger, "Entry : EigenResult at Entry : GmmResult at Entry");
	for (int i = 0; i < eigenResult->size(); ++i) {
		LOG4CPLUS_INFO(logger, i << " : " << eigenResult->at(i) << " : " << gmmResult->at(i));
	}*/

	if (s != nullptr) {
		delete s;
	}
	
	LOG4CPLUS_INFO(logger, "Nothing more to do, exiting.");

	return 0;
}
