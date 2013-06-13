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
 *	@note This class creates a new Mdp object that can
 *	be accessed via getMdp(). However, it will not delete this object!
 *
 *	@note The labeling representation in the file may use at most as much nodes as are specified in the mdp.
 */

storm::models::Mdp<double> NondeterministicModelParserAsMdp(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");
storm::models::Ctmdp<double> NondeterministicModelParserAsCtmdp(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");


/*!
 * @brief This Class acts as a container much like std::pair for the five return values of the NondeterministicModelParser
 */
template <class T>
class NondeterministicModelParserResultContainer {
public:
	storm::storage::SparseMatrix<T> transitionSystem;
	storm::models::AtomicPropositionsLabeling labeling;
	std::vector<uint_fast64_t> rowMapping;
	boost::optional<std::vector<T>> stateRewards;
	boost::optional<storm::storage::SparseMatrix<T>> transitionRewards;
	NondeterministicModelParserResultContainer(storm::storage::SparseMatrix<T> transitionSystem, std::vector<uint_fast64_t> rowMapping, storm::models::AtomicPropositionsLabeling labeling) : transitionSystem(transitionSystem), labeling(labeling), rowMapping(rowMapping) { }
private:
	NondeterministicModelParserResultContainer() {}
};


NondeterministicModelParserResultContainer<double> parseNondeterministicModel(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

} /* namespace parser */

} /* namespace storm */

#endif /* STORM_PARSER_NONDETERMINISTICMODELPARSER_H_ */
