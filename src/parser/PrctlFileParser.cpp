/*
 * PrctlFileParser.cpp
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#include <sstream>

#include "PrctlFileParser.h"
#include "PrctlParser.h"
#include "modelchecker/EigenDtmcPrctlModelChecker.h"
#include "modelchecker/GmmxxDtmcPrctlModelChecker.h"
#include "modelchecker/GmmxxMdpPrctlModelChecker.h"

namespace storm {
namespace parser {

PrctlFileParser::PrctlFileParser(std::string filename, storm::models::Dtmc<double>& dtmc, enum libraries library) {
	//Here ,the model checker which uses the correct library is created and a reference stored in the pointer modelChecker:
	storm::modelchecker::SparseDtmcPrctlModelChecker<double>* modelChecker = nullptr;
	switch(library) {
	//case EIGEN:
		//Eigen Model Checker is not completely implemented at the moment, thus using Eigen is not possible...
		//Current behaviour: Fall back to GMMXX...
		//modelChecker = new storm::modelchecker::EigenDtmcPrctlModelChecker<double>(dtmc);
	//	break;
	case GMMXX:
	default:					//Note: GMMXX is default, hence default branches here, too.
		modelChecker = new storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>(dtmc);
		break;
	}

	//Now, the check function (which works on abstract model checkers) can parse the formulas and invoke the model checker:
	check(filename, modelChecker);
	delete modelChecker;
}

PrctlFileParser::PrctlFileParser(std::string filename, storm::models::Mdp<double>& mdp, enum libraries library) {
	//Here ,the model checker which uses the correct library is created and a reference stored in the pointer modelChecker:
	storm::modelchecker::SparseMdpPrctlModelChecker<double>* modelChecker = nullptr;
	switch(library) {
	//case EIGEN:
		//Eigen MDP Model Checker is not implemented yet
		//Current behaviour: Fall back to GMMXX...
	//	break;
	case GMMXX:
	default:					//Note: GMMXX is default, hence default branches here, too.
		modelChecker = new storm::modelchecker::GmmxxMdpPrctlModelChecker<double>(mdp);
		break;
	}

	//Now, the check function (which works on abstract model checkers) can parse the formulas and invoke the model checker:
	check(filename, modelChecker);
	delete modelChecker;
}

PrctlFileParser::~PrctlFileParser() {
	//intentionally left empty
}

void PrctlFileParser::check(std::string filename, storm::modelchecker::AbstractModelChecker<double>* modelChecker) {
	// Open file
	std::ifstream inputFileStream(filename, std::ios::in);

	while(!inputFileStream.eof()) {
		std::string line;
		//The while loop reads the input file line by line
		while (std::getline(inputFileStream, line)) {
			PrctlParser parser(line);
			storm::formula::AbstractStateFormula<double>* stateFormula = dynamic_cast<storm::formula::AbstractStateFormula<double>*>(parser.getFormula());
			if (stateFormula != nullptr) {
				modelChecker->check(*stateFormula);
			}
			storm::formula::NoBoundOperator<double>* noBoundFormula = dynamic_cast<storm::formula::NoBoundOperator<double>*>(parser.getFormula());
			if (noBoundFormula != nullptr) {
				modelChecker->check(*noBoundFormula);
			}
			delete parser.getFormula();
		}
	}
}

} /* namespace parser */
} /* namespace storm */
