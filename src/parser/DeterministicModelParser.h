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
 *	@brief Loads a deterministic model (Dtmc or Ctmc) from files.
 *
 *	Given the file paths of the files holding the transitions, the atomic propositions and optionally the state- and transition rewards
 *	it loads the files, parses them and returns the desired model.
 *
 *	@note This class creates a new Dtmc or Ctmc object
 *
 *	@note The labeling representation in the file may use at most as much nodes as are specified in the transition system.
 */

class DeterministicModelParser {

public:

	/*!
	 * @brief A struct containing the parsed components of a deterministic model.
	 */
	struct Result {

		Result(storm::storage::SparseMatrix<double>& transitionSystem, storm::models::AtomicPropositionsLabeling& labeling) : transitionSystem(transitionSystem), labeling(labeling) {
			// Intentionally left empty.
		}

		Result(storm::storage::SparseMatrix<double>&& transitionSystem, storm::models::AtomicPropositionsLabeling&& labeling) : transitionSystem(std::move(transitionSystem)), labeling(std::move(labeling)) {
			// Intentionally left empty.
		}

		// A matrix representing the transitions of the model
		storm::storage::SparseMatrix<double> transitionSystem;

		// The labels of each state.
		storm::models::AtomicPropositionsLabeling labeling;

		// Optional rewards for each state.
		boost::optional<std::vector<double>> stateRewards;

		// Optional rewards for each transition.
		boost::optional<storm::storage::SparseMatrix<double>> transitionRewards;
	};


	/*!
	 * @brief Parse a Dtmc.
	 */
	static storm::models::Dtmc<double> parseDtmc(std::string const & transitionSystemFile,
												 std::string const & labelingFile,
												 std::string const & stateRewardFile = "",
												 std::string const & transitionRewardFile = "");

	/*!
	 * @brief Parse a Ctmc.
	 */
	static storm::models::Ctmc<double> parseCtmc(std::string const & transitionSystemFile,
										  	     std::string const & labelingFile,
										  	     std::string const & stateRewardFile = "",
										  	     std::string const & transitionRewardFile = "");

private:

	/*!
	 * @brief Call sub-parsers on the given files and fill the container with the results.
	 */
	static Result parseDeterministicModel(std::string const & transitionSystemFile,
												   std::string const & labelingFile,
												   std::string const & stateRewardFile = "",
												   std::string const & transitionRewardFile = "");

};

} /* namespace parser */

} /* namespace storm */

#endif /* STORM_PARSER_DETERMINISTICMODELPARSER_H_ */
