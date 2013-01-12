/*
 * DtmcParser.h
 *
 *  Created on: 19.12.2012
 *      Author: thomas
 */

#ifndef DTMCPARSER_H_
#define DTMCPARSER_H_

#include "src/parser/Parser.h"
#include "src/models/Dtmc.h"

namespace storm {
namespace parser {

/*!
 *	@brief Load label and transition file and return initialized dtmc object
 *
 *	@Note This class creates a new Dtmc object that can
 *	be accessed via getDtmc(). However, it will not delete this object!
 *
 *	@Note The labeling representation in the file may use at most as much nodes as are specified in the dtmc.
 */
class DtmcParser: public storm::parser::Parser {
public:
	DtmcParser(std::string const & transitionSystemFile, std::string const & labelingFile,
			std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

	std::shared_ptr<storm::models::Dtmc<double>> getDtmc() {
		return this->dtmc;
	}

private:
	std::shared_ptr<storm::models::Dtmc<double>> dtmc;
};

} /* namespace parser */
} /* namespace storm */
#endif /* DTMCPARSER_H_ */
