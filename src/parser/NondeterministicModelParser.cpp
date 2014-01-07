/*
 * NonDeterministicModelParser.cpp
 *
 *  Created on: 14.01.2013
 *      Author: Philipp Berger
 */

#include "src/parser/NondeterministicModelParser.h"

#include <string>
#include <vector>

#include "src/parser/NondeterministicSparseTransitionParser.h"
#include "src/parser/AtomicPropositionLabelingParser.h"
#include "src/parser/SparseStateRewardParser.h"

namespace storm {
	namespace parser {

		/*!
		 * Parses a transition file and a labeling file and produces an intermediate Result Container
		 * Note that the labeling file may have at most as many nodes as the transition file!
		 *
		 * @param transitionSystemFile String containing the location of the transition file (....tra)
		 * @param labelingFile String containing the location of the labeling file (....lab)
		 * @param stateRewardFile String containing the location of the state reward file (...srew)
		 * @param transitionRewardFile String containing the location of the transition reward file (...trew)
		 */
		NondeterministicModelParser::Result NondeterministicModelParser::parseNondeterministicModel(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile, std::string const & transitionRewardFile) {

			NondeterministicSparseTransitionParser::Result transitionParserResult(std::move(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(transitionSystemFile)));
			storm::storage::SparseMatrix<double> transitions(std::move(transitionParserResult.transitionMatrix));

			uint_fast64_t stateCount = transitions.getColumnCount();
			uint_fast64_t rowCount = transitions.getRowCount();

			storm::models::AtomicPropositionsLabeling labeling(std::move(storm::parser::AtomicPropositionLabelingParser(stateCount, labelingFile)));

			Result result(std::move(transitions), std::move(transitionParserResult.rowMapping), std::move(labeling));

			// Only parse state rewards of a file is given.
			if (stateRewardFile != "") {
				result.stateRewards.reset(storm::parser::SparseStateRewardParser::parseSparseStateReward(stateCount, stateRewardFile));
			}

			// Only parse transition rewards of a file is given.
			if (transitionRewardFile != "") {
				RewardMatrixInformationStruct rewardMatrixInfo(rowCount, stateCount, &result.rowMapping);
				result.transitionRewards.reset(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(transitionRewardFile, rewardMatrixInfo).transitionMatrix);
			}
			return result;
		}

		/*!
		 * Uses the Function parseNondeterministicModel internally to parse the given input files.
		 * @note This is a Short-Hand for Constructing a Mdp directly from the data returned by @parseNondeterministicModel
		 * @return A Mdp Model
		 */
		storm::models::Mdp<double> NondeterministicModelParser::parseMdp(std::string const & transitionSystemFile, std::string const & labelingFile, std::string const & stateRewardFile, std::string const & transitionRewardFile) {

			Result parserResult = parseNondeterministicModel(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile);
			return storm::models::Mdp<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.rowMapping), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
		}

		/*!
		 * Uses the Function parseNondeterministicModel internally to parse the given input files.
		 * @note This is a Short-Hand for Constructing a Ctmdp directly from the data returned by @parseNondeterministicModel
		 * @return A Ctmdp Model
		 */
		storm::models::Ctmdp<double> NondeterministicModelParser::parseCtmdp(std::string const & transitionSystemFile, std::string const & labelingFile, std::string const & stateRewardFile, std::string const & transitionRewardFile) {

			Result parserResult = parseNondeterministicModel(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile);
			return storm::models::Ctmdp<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(parserResult.rowMapping), std::move(parserResult.stateRewards), std::move(parserResult.transitionRewards), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
		}

	} /* namespace parser */
} /* namespace storm */
