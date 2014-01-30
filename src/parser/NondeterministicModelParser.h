/*
 * NonDeterministicModelParser.h
 *
 *  Created on: 14.01.2013
 *      Author: thomas
 */

#ifndef STORM_PARSER_NONDETERMINISTICMODELPARSER_H_
#define STORM_PARSER_NONDETERMINISTICMODELPARSER_H_

#include "src/models/Mdp.h"
#include "src/models/Ctmdp.h"

namespace storm {
	namespace parser {

		class NondeterministicModelParser {

		public:

			/*!
			 * @brief This Class acts as a container much like std::pair for the five return values of the NondeterministicModelParser
			 */
			struct Result {

				Result(storm::storage::SparseMatrix<double>& transitionSystem, std::vector<uint_fast64_t>& rowMapping, storm::models::AtomicPropositionsLabeling& labeling) : transitionSystem(transitionSystem), labeling(labeling), rowMapping(rowMapping) {
					// Intentionally left empty.
				}

				Result(storm::storage::SparseMatrix<double>&& transitionSystem, std::vector<uint_fast64_t>&& rowMapping, storm::models::AtomicPropositionsLabeling&& labeling) : transitionSystem(std::move(transitionSystem)), labeling(std::move(labeling)), rowMapping(std::move(rowMapping)) {
					// Intentionally left empty.
				}

				// A matrix representing the transitions of the model
				storm::storage::SparseMatrix<double> transitionSystem;

				// The labels of each state.
				storm::models::AtomicPropositionsLabeling labeling;

				// A mapping from rows of the matrix to states of the model.
				// This resolves the nondeterministic choices inside the transition system.
				std::vector<uint_fast64_t> rowMapping;

				// Optional rewards for each state.
				boost::optional<std::vector<double>> stateRewards;

				// Optional rewards for each transition.
				boost::optional<storm::storage::SparseMatrix<double>> transitionRewards;
			};

			/*!
			 *	@brief Load label and transition file and return initialized Mdp object
			 *
			 *	@note This class creates a new Mdp object that can
			 *	be accessed via getMdp(). However, it will not delete this object!
			 *
			 *	@note The labeling representation in the file may use at most as much nodes as are specified in the mdp.
			 */
			static storm::models::Mdp<double> parseMdp(std::string const & transitionSystemFile, std::string const & labelingFile, std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");


			/*!
			 *
			 */
			static storm::models::Ctmdp<double> parseCtmdp(std::string const & transitionSystemFile, std::string const & labelingFile, std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

		private:

			/*!
			 *
			 */
			static Result parseNondeterministicModel(std::string const & transitionSystemFile, std::string const & labelingFile, std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");

		};

	} /* namespace parser */
} /* namespace storm */

#endif /* STORM_PARSER_NONDETERMINISTICMODELPARSER_H_ */
