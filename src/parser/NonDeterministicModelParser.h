/*
 * NonDeterministicModelParser.h
 *
 *  Created on: 14.01.2013
 *      Author: thomas
 */

#ifndef STORM_PARSER_NONDETERMINISTICMODELPARSER_H_
#define STORM_PARSER_NONDETERMINISTICMODELPARSER_H_

#include "src/parser/Parser.h"
#include "src/models/Mdp.h"
#include "src/models/Ctmdp.h"

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
class NonDeterministicModelParser: public storm::parser::Parser {
public:
	NonDeterministicModelParser(std::string const & transitionSystemFile, std::string const & labelingFile,
			std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

	std::shared_ptr<storm::models::Mdp<double>> getMdp() {
		if (this->mdp == nullptr) {
			this->mdp = std::shared_ptr<storm::models::Mdp<double>>(new storm::models::Mdp<double>(
				this->probabilityMatrix, this->stateLabeling, this->rowMapping, this->stateRewards, this->transitionRewardMatrix
			));
		}
		return this->mdp;
	}

	std::shared_ptr<storm::models::Ctmdp<double>> getCtmdp() {
		if (this->ctmdp == nullptr) {
			this->ctmdp = std::shared_ptr<storm::models::Ctmdp<double>>(new storm::models::Ctmdp<double>(
				this->probabilityMatrix, this->stateLabeling, this->rowMapping, this->stateRewards, this->transitionRewardMatrix
			));
		}
		return this->ctmdp;
	}

private:
	std::shared_ptr<storm::storage::SparseMatrix<double>> probabilityMatrix;
	std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling;
	std::shared_ptr<std::vector<uint_fast64_t>> rowMapping;
	std::shared_ptr<std::vector<double>> stateRewards;
	std::shared_ptr<storm::storage::SparseMatrix<double>> transitionRewardMatrix;

	std::shared_ptr<storm::models::Mdp<double>> mdp;
	std::shared_ptr<storm::models::Ctmdp<double>> ctmdp;
};

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_NONDETERMINISTICMODELPARSER_H_ */
