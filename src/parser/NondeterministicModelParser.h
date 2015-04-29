#ifndef STORM_PARSER_NONDETERMINISTICMODELPARSER_H_
#define STORM_PARSER_NONDETERMINISTICMODELPARSER_H_

#include "src/models/sparse/Mdp.h"

namespace storm {
	namespace parser {

		/*!
		 * Loads a nondeterministic model (Mdp or Ctmdp) from files.
		 *
		 * Given the file paths of the files holding the transitions, the atomic propositions and optionally the state- and transition rewards
		 * it loads the files, parses them and returns the desired model.
		 */
		class NondeterministicModelParser {

		public:

			/*!
			 * A structure containing the parsed components of a nondeterministic model.
			 */
			struct Result {

				/*!
				 * The copy constructor.
				 *
				 * @param transitionSystem The transition system to be contained in the Result.
				 * @param labeling The the labeling of the transition system to be contained in the Result.
				 */
				Result(storm::storage::SparseMatrix<double>& transitionSystem, storm::models::sparse::StateLabeling& labeling) : transitionSystem(transitionSystem), labeling(labeling) {
					// Intentionally left empty.
				}

				/*!
				 * The move constructor.
				 *
				 * @param transitionSystem The transition system to be contained in the Result.
				 * @param labeling The the labeling of the transition system to be contained in the Result.
				 */
				Result(storm::storage::SparseMatrix<double>&& transitionSystem, storm::models::sparse::StateLabeling&& labeling) : transitionSystem(std::move(transitionSystem)), labeling(std::move(labeling)) {
					// Intentionally left empty.
				}

				/*!
				 * A matrix representing the transitions of the model
				 */
				storm::storage::SparseMatrix<double> transitionSystem;

				/*!
				 * The labels of each state.
				 */
				storm::models::sparse::StateLabeling labeling;

				/*!
				 * Optional rewards for each state.
				 */
				boost::optional<std::vector<double>> stateRewards;

				/*!
				 * Optional rewards for each transition.
				 */
				boost::optional<storm::storage::SparseMatrix<double>> transitionRewards;
			};

			/*!
			 * Parse a Mdp.
			 *
			 * This method is an adapter to the actual parsing function.
			 * I.e. it uses @parseNondeterministicModel internally to parse the given input files, takes its result and compiles it into a Dtmc.
			 *
			 * @note The number of states of the model is determined by the transitions file.
			 *       The labeling file may therefore not contain labels of states that are not contained in the transitions file.
			 *
			 * @param transitionsFilename The path and name of the file containing the transitions of the model.
			 * @param labelingFilename The path and name of the file containing the labels for the states of the model.
			 * @param stateRewardFilename The path and name of the file containing the state reward of the model. This file is optional.
			 * @param transitionRewardFilename The path and name of the file containing the transition rewards of the model. This file is optional.
			 * @return The parsed Mdp.
			 */
            static storm::models::sparse::Mdp<double> parseMdp(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const & stateRewardFilename = "", std::string const & transitionRewardFilename = "");

		private:

			/*!
			 * Parses a nondeterministic model from the given files.
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
			static Result parseNondeterministicModel(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const & stateRewardFilename = "", std::string const & transitionRewardFilename = "");

		};

	} /* namespace parser */
} /* namespace storm */

#endif /* STORM_PARSER_NONDETERMINISTICMODELPARSER_H_ */
