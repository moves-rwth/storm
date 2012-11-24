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

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

#include "src/exceptions/InvalidSettings.h"
 
int main(const int argc, const char* argv[]) {
	mrmc::settings::Settings* s = NULL;
	log4cplus::BasicConfigurator loggingConfig;
	loggingConfig.configure();
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
	LOG4CPLUS_INFO(logger, "This is the Markov Reward Model Checker (MRMC) by i2 of RWTH Aachen university.");
	
	try
	{
		s = new mrmc::settings::Settings(argc, argv, nullptr);
	}
	catch (mrmc::exceptions::InvalidSettings&)
	{
		std::cout << "Could not recover from settings error, terminating." << std::endl;
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

	return 0;
}

