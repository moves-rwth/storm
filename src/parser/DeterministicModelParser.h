/*
 * DtmcParser.h
 *
 *  Created on: 19.12.2012
 *      Author: thomas
 */

#ifndef STORM_PARSER_DETERMINISTICMODELPARSER_H_
#define STORM_PARSER_DETERMINISTICMODELPARSER_H_

#include "src/models/Dtmc.h"
#include "src/models/Ctmc.h"

namespace storm {
	namespace parser {

		/*!
		 * Loads a deterministic model (Dtmc or Ctmc) from files.
		 *
		 * Given the file paths of the files holding the transitions, the atomic propositions and optionally the state- and transition rewards
		 * it loads the files, parses them and returns the desired model.
		 */
		class DeterministicModelParser {

		public:

			/*!
			 * A structure containing the parsed components of a deterministic model.
			 */
			struct Result {

				/*!
				 * The copy constructor.
				 *
				 * @param transitionSystem The transition system to be contained in the Result.
				 * @param labeling The the labeling of the transition system to be contained in the Result.
				 */
				Result(storm::storage::SparseMatrix<double>& transitionSystem, storm::models::AtomicPropositionsLabeling& labeling) : transitionSystem(transitionSystem), labeling(labeling) {
					// Intentionally left empty.
				}

				/*!
				 * The move constructor.
				 *
				 * @param transitionSystem The transition system to be contained in the Result.
				 * @param labeling The the labeling of the transition system to be contained in the Result.
				 */
				Result(storm::storage::SparseMatrix<double>&& transitionSystem, storm::models::AtomicPropositionsLabeling&& labeling) : transitionSystem(std::move(transitionSystem)), labeling(std::move(labeling)) {
					// Intentionally left empty.
				}

				//! A matrix representing the transitions of the model
				storm::storage::SparseMatrix<double> transitionSystem;

				//! The labels of each state.
				storm::models::AtomicPropositionsLabeling labeling;

				//! Optional rewards for each state.
				boost::optional<std::vector<double>> stateRewards;

				//! Optional rewards for each transition.
				boost::optional<storm::storage::SparseMatrix<double>> transitionRewards;
			};


			/*!
			 * Parse a Dtmc.
			 *
			 * This method is an adapter to the actual parsing function.
			 * I.e. it uses @parseDeterministicModel internally to parse the given input files, takes its result and compiles it into a Dtmc.
			 *
			 * @note The number of states of the model is determined by the transitions file.
			 *       The labeling file may therefore not contain labels of states that are not contained in the transitions file.
			 *
			 * @param transitionsFilename The path and name of the file containing the transitions of the model.
			 * @param labelingFilename The path and name of the file containing the labels for the states of the model.
			 * @param stateRewardFilename The path and name of the file containing the state reward of the model. This file is optional.
			 * @param transitionRewardFilename The path and name of the file containing the transition rewards of the model. This file is optional.
			 * @return The parsed Dtmc.
			 */
			static storm::models::Dtmc<double> parseDtmc(std::string const & transitionsFilename,
														 std::string const & labelingFilename,
														 std::string const & stateRewardFilename = "",
														 std::string const & transitionRewardFilename = "");

			/*!
			 * Parse a Ctmc.
			 *
			 * This method is an adapter to the actual parsing function.
			 * I.e. it uses @parseDeterministicModel internally to parse the given input files, takes its result and compiles it into a Ctmc.
			 *
			 * @note The number of states of the model is determined by the transitions file.
			 *       The labeling file may therefore not contain labels of states that are not contained in the transitions file.
			 *
			 * @param transitionsFilename The path and name of the file containing the transitions of the model.
			 * @param labelingFilename The path and name of the file containing the labels for the states of the model.
			 * @param stateRewardFilename The path and name of the file containing the state reward of the model. This file is optional.
			 * @param transitionRewardFilename The path and name of the file containing the transition rewards of the model. This file is optional.
			 * @return The parsed Ctmc.
			 */
			static storm::models::Ctmc<double> parseCtmc(std::string const & transitionsFilename,
														 std::string const & labelingFilename,
														 std::string const & stateRewardFilename = "",
														 std::string const & transitionRewardFilename = "");

		private:

			/*!
			 * Parses a deterministic model from the given files.
			 * Calls sub-parsers on the given files and fills the container with the results.
			 *
			 * @note The number of states of the model is determined by the transitions file.
			 *       The labeling file may therefore not contain labels of states that are not contained in the transitions file.
			 *
			 * @param transitionsFilename The path and name of the file containing the transitions of the model.
			 * @param labelingFilename The path and name of the file containing the labels for the states of the model.
			 * @param stateRewardFilename The path and name of the file containing the state reward of the model. This file is optional.
			 * @param transitionRewardFilename The path and name of the file containing the transition rewards of the model. This file is optional.
			 * @return The parsed model encapsulated in a Result structure.
			 */
			static Result parseDeterministicModel(std::string const & transitionsFilename,
												  std::string const & labelingFilename,
												  std::string const & stateRewardFilename = "",
												  std::string const & transitionRewardFilename = "");
		};

	} /* namespace parser */
} /* namespace storm */

#endif /* STORM_PARSER_DETERMINISTICMODELPARSER_H_ */
