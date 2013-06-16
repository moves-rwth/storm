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

#include <boost/optional.hpp>

namespace storm {
namespace parser {

/*!
 *	@brief Load label and transition file and returns an initialized dtmc or ctmc object.
 *
 *	@note This class creates a new Dtmc or Ctmc object
 *
 *	@note The labeling representation in the file may use at most as much nodes as are specified in the transition system.
 */


storm::models::Dtmc<double> DeterministicModelParserAsDtmc(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");
storm::models::Ctmc<double> DeterministicModelParserAsCtmc(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

/*!
 * @brief This Class acts as a container much like std::pair for the four return values of the DeterministicModelParser
 */
template <class T>
class DeterministicModelParserResultContainer {
public:
	storm::storage::SparseMatrix<T> transitionSystem;
	storm::models::AtomicPropositionsLabeling labeling;
	boost::optional<std::vector<T>> stateRewards;
	boost::optional<storm::storage::SparseMatrix<T>> transitionRewards;
	DeterministicModelParserResultContainer(storm::storage::SparseMatrix<T>& transitionSystem, storm::models::AtomicPropositionsLabeling& labeling) : transitionSystem(transitionSystem), labeling(labeling) { }
	DeterministicModelParserResultContainer(storm::storage::SparseMatrix<T>&& transitionSystem, storm::models::AtomicPropositionsLabeling&& labeling) : transitionSystem(std::move(transitionSystem)), labeling(std::move(labeling)) { }

	DeterministicModelParserResultContainer(const DeterministicModelParserResultContainer & other) : transitionSystem(other.transitionSystem), 
		labeling(other.labeling), stateRewards(other.stateRewards), transitionRewards(other.transitionRewards) {}
	DeterministicModelParserResultContainer(DeterministicModelParserResultContainer && other) : transitionSystem(std::move(other.transitionSystem)), 
		labeling(std::move(other.labeling)), stateRewards(std::move(other.stateRewards)), transitionRewards(std::move(other.transitionRewards)) {}
private:
	DeterministicModelParserResultContainer() {}
};


DeterministicModelParserResultContainer<double> parseDeterministicModel(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_DETERMINISTICMODELPARSER_H_ */
