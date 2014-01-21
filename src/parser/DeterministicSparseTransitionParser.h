#ifndef STORM_PARSER_DETERMINISTICSPARSETRANSITIONPARSER_H_
#define STORM_PARSER_DETERMINISTICSPARSETRANSITIONPARSER_H_

#include "src/storage/SparseMatrix.h"
#include "src/parser/Parser.h"

namespace storm {

namespace parser {

class DeterministicSparseTransitionParser {
public:
	/*
	 * A structure representing the result of the first pass of this parser. It contains the number of non-zero entries in the model and the highest state index.
	 */
	struct FirstPassResult {

		FirstPassResult() : numberOfNonzeroEntries(0), highestStateIndex(0) {
			// Intentionally left empty.
		}

		// The total number of non-zero entries of the model.
		uint_fast64_t numberOfNonzeroEntries;

		// The highest state index that appears in the model.
		uint_fast64_t highestStateIndex;
	};

	/*!
	 * Load a deterministic transition system from file and create a
	 * sparse adjacency matrix whose entries represent the weights of the edges.
	 *
	 * @param filename The path of file to be parsed.
	 * @param insertDiagonalEntriesIfMissing A flag set iff entries on the primary diagonal of the matrix should be added in case they are missing in the parsed file.
	 * @return A SparseMatrix containing the parsed transition system.
	 */
	static storm::storage::SparseMatrix<double> parseDeterministicTransitions(std::string const& filename, bool insertDiagonalEntriesIfMissing = true);

	/*!
	 * Load the transition rewards for a deterministic transition system from file and create a
	 * sparse adjacency matrix whose entries represent the rewards of the respective transitions.
	 */

	/*!
	 * Load the transition rewards for a deterministic transition system from file and create a
	 * sparse adjacency matrix whose entries represent the rewards of the respective transitions.
	 *
	 * @param filename The path of file to be parsed.
	 * @param rewardMatrixInformation A struct containing information that is used to check if the transition reward matrix fits to the rest of the model.
	 * @return A SparseMatrix containing the parsed transition rewards.
	 */
	static storm::storage::SparseMatrix<double> parseDeterministicTransitionRewards(std::string const& filename, RewardMatrixInformationStruct const& rewardMatrixInformation);

private:

	/*
	 * Performs the first pass on the input pointed to by the given buffer to obtain the number of
	 * transitions and the maximum node id.
	 *
	 * @param buffer The buffer that cointains the input.
	 * @param lineEndings The line endings that are to be used while parsing.
	 * @param insertDiagonalEntriesIfMissing A flag set iff entries on the primary diagonal of the matrix should be added in case they are missing in the parsed file.
	 * @return A structure representing the result of the first pass.
	 */
	static FirstPassResult firstPass(char* buffer, SupportedLineEndingsEnum lineEndings, bool insertDiagonalEntriesIfMissing = true);

	/*
	 * The main parsing routine.
	 * Opens the given file, calls the first pass and performs the second pass, parsing the content of the file into a SparseMatrix.
	 *
	 * @param filename The path of file to be parsed.
	 * @param rewardFile A flag set iff the file to be parsed contains transition rewards.
	 * @param insertDiagonalEntriesIfMissing A flag set iff entries on the primary diagonal of the matrix should be added in case they are missing in the parsed file.
	 * @param rewardMatrixInformation A struct containing information that is used to check if the transition reward matrix fits to the rest of the model.
	 * @return A SparseMatrix containing the parsed file contents.
	 */
	static storm::storage::SparseMatrix<double> parse(std::string const& filename, bool isRewardFile, RewardMatrixInformationStruct const& rewardMatrixInformation, bool insertDiagonalEntriesIfMissing = false);

};

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_DETERMINISTICSPARSETRANSITIONPARSER_H_ */
