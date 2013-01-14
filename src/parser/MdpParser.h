/*
 * MdpParser.h
 *
 *  Created on: 14.01.2013
 *      Author: thomas
 */

#ifndef STORM_PARSER_MDPPARSER_H_
#define STORM_PARSER_MDPPARSER_H_

#include "src/parser/Parser.h"
#include "src/models/Mdp.h"

namespace storm {
namespace parser {

/*!
 *	@brief Load label and transition file and return initialized mdp object
 *
 *	@Note This class creates a new Mdp object that can
 *	be accessed via getMdp(). However, it will not delete this object!
 *
 *	@Note The labeling representation in the file may use at most as much nodes as are specified in the mdp.
 */
class MdpParser: public storm::parser::Parser {
public:
	MdpParser(std::string const & transitionSystemFile, std::string const & labelingFile,
			std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

	std::shared_ptr<storm::models::Mdp<double>> getMdp() {
		return this->mdp;
	}

private:
	std::shared_ptr<storm::models::Mdp<double>> mdp;
};

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_MDPPARSER_H_ */
