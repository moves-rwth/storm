#include "src/parser/DeterministicModelParser.h"

#include <string>
#include <vector>

#include "src/models/sparse/StandardRewardModel.h"

#include "src/parser/DeterministicSparseTransitionParser.h"
#include "src/parser/AtomicPropositionLabelingParser.h"
#include "src/parser/SparseStateRewardParser.h"

namespace storm {
	namespace parser {

		DeterministicModelParser::Result DeterministicModelParser::parseDeterministicModel(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const & stateRewardFilename, std::string const & transitionRewardFilename) {

			// Parse the transitions.
			storm::storage::SparseMatrix<double> transitions(std::move(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(transitionsFilename)));

			uint_fast64_t stateCount = transitions.getColumnCount();

			// Parse the state labeling.
			storm::models::sparse::StateLabeling labeling(std::move(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(stateCount, labelingFilename)));

			// Construct the result.
			DeterministicModelParser::Result result(std::move(transitions), std::move(labeling));

			// Only parse state rewards if a file is given.
			if (stateRewardFilename != "") {
				result.stateRewards = storm::parser::SparseStateRewardParser::parseSparseStateReward(stateCount, stateRewardFilename);
			}

			// Only parse transition rewards if a file is given.
			if (transitionRewardFilename != "") {
				result.transitionRewards = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(transitionRewardFilename, result.transitionSystem);
			}

			return result;
		}

        storm::models::sparse::Dtmc<double> DeterministicModelParser::parseDtmc(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename) {
            DeterministicModelParser::Result parserResult(std::move(parseDeterministicModel(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename)));

            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<double>> rewardModels;
            if (!stateRewardFilename.empty() || !transitionRewardFilename.empty()) {
                rewardModels.insert(std::make_pair("", storm::models::sparse::StandardRewardModel<double>(parserResult.stateRewards, boost::optional<std::vector<double>>(), parserResult.transitionRewards)));
            }
            return storm::models::sparse::Dtmc<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(rewardModels), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
		}

        storm::models::sparse::Ctmc<double> DeterministicModelParser::parseCtmc(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const & stateRewardFilename, std::string const & transitionRewardFilename) {
            DeterministicModelParser::Result parserResult(std::move(parseDeterministicModel(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename)));

            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<double>> rewardModels;
            if (!stateRewardFilename.empty() || !transitionRewardFilename.empty()) {
                rewardModels.insert(std::make_pair("", storm::models::sparse::StandardRewardModel<double>(parserResult.stateRewards, boost::optional<std::vector<double>>(), parserResult.transitionRewards)));
            }
            return storm::models::sparse::Ctmc<double>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(rewardModels), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
		}

	} /* namespace parser */
} /* namespace storm */
