/*
 * PrctlFileParser.h
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_PARSER_PRCTLFILEPARSER_H_
#define STORM_PARSER_PRCTLFILEPARSER_H_

#include "models/Dtmc.h"
#include "models/Mdp.h"
#include "formula/Prctl.h"
#include "modelchecker/AbstractModelChecker.h"

namespace storm {
namespace parser {

/*!
 * Reads a PRCTL formula from a file and return the formula tree.
 *
 * @note
 * This class creates a PctlFormula object which can be accessed through the getFormula() method (of base
 * class PrctlParser). However, it will not delete this object.
 */
class PrctlFileParser {
public:
	enum libraries {
		GMMXX,
		EIGEN
	};

	/*!
	 * Reads a given file of formulas and checks each of these against a given DTMC.
	 *
	 * @param filename The name of the file to parse
	 * @param dtmc		 The DTMC model to check
	 * @param library  Specifies the library that should perform the algebraic operations during model checking (default is GMMxx)
	 */
	PrctlFileParser(std::string filename, storm::models::Dtmc<double>& dtmc, enum libraries library=GMMXX);

	/*!
	 * Reads a given file of formulas and checks each of these against a given MDP.
	 *
	 * @param filename The name of the file to parse
	 * @param mdp		 The MDP model to check
	 * @param library  Specifies the library that should perform the algebraic operations during model checking (default is GMMxx, which at the moment also is the only implemented version...)
	 */
	PrctlFileParser(std::string filename, storm::models::Mdp<double>& mdp, enum libraries library=GMMXX);

protected:
	/*!
	 * Does the actual checking.
	 * This procedure is equal for all model types (only the model checker is different, so it has to be created in
	 * different methods beforehand)
	 *
	 * @param filename     The name of the file to parse
	 * @param modelChecker The model checker that checks the formula (has to know its model!)
	 */
	void check(std::string filename, storm::modelchecker::AbstractModelChecker<double>* modelChecker);

	/*!
	 * Destructor.
	 * At this time, empty
	 *
	 */
	virtual ~PrctlFileParser();
};

} /* namespace parser */
} /* namespace storm */

#endif /* STORM_PARSER_PRCTLFILEPARSER_H_ */
