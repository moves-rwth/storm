/*
 * DtmcParser.h
 *
 *  Created on: 19.12.2012
 *      Author: thomas
 */

#ifndef STORM_PARSER_DETERMINISTICMODELPARSER_H_
#define STORM_PARSER_DETERMINISTICMODELPARSER_H_

#include "src/parser/Parser.h"
#include "src/models/Dtmc.h"
#include "src/models/Ctmc.h"

namespace storm {
namespace parser {

/*!
 *	@brief Load label and transition file and return initialized dtmc or ctmc object.
 *
 *	@Note This class creates a new Dtmc or Ctmc object that can
 *	be accessed via getDtmc() or getCtmc(). However, it will not delete this object!
 *
 *	@Note The labeling representation in the file may use at most as much nodes as are specified in the transition system.
 */
class DeterministicModelParser: public storm::parser::Parser {
	public:
		DeterministicModelParser(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

		std::shared_ptr<storm::models::Dtmc<double>> getDtmc() {
			if (this->dtmc == nullptr) {
				this->dtmc = std::shared_ptr<storm::models::Dtmc<double>>(new storm::models::Dtmc<double>(this->transitionSystem, this->labeling, this->stateRewards, this->transitionRewards));
			}
			return this->dtmc;
		}
		std::shared_ptr<storm::models::Ctmc<double>> getCtmc() {
			if (this->ctmc == nullptr) {
				this->ctmc = std::shared_ptr<storm::models::Ctmc<double>>(new storm::models::Ctmc<double>(this->transitionSystem, this->labeling, this->stateRewards, this->transitionRewards));
			}
			return this->ctmc;
		}

	private:
		std::shared_ptr<storm::storage::SparseMatrix<double>> transitionSystem;
		std::shared_ptr<storm::models::AtomicPropositionsLabeling> labeling;
		std::shared_ptr<std::vector<double>> stateRewards;
		std::shared_ptr<storm::storage::SparseMatrix<double>> transitionRewards;
	
		std::shared_ptr<storm::models::Dtmc<double>> dtmc;
		std::shared_ptr<storm::models::Ctmc<double>> ctmc;
};

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_DETERMINISTICMODELPARSER_H_ */
