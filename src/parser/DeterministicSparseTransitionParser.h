#ifndef STORM_PARSER_DETERMINISTICSPARSETRANSITIONPARSER_H_
#define STORM_PARSER_DETERMINISTICSPARSETRANSITIONPARSER_H_

#include "src/storage/SparseMatrix.h"

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
	static storm::storage::SparseMatrix<double> parseDeterministicTransitions(std::string const& filename);

	/*!
	 * Load the transition rewards for a deterministic transition system from file and create a
	 * sparse adjacency matrix whose entries represent the rewards of the respective transitions.
	 */

	/*!
	 * Load the transition rewards for a deterministic transition system from file and create a
	 * sparse adjacency matrix whose entries represent the rewards of the respective transitions.
	 *
	 * @param filename The path of file to be parsed.
	 * @param transitionMatrix The transition matrix of the model in which the reward matrix is to be used in.
	 *                         The dimensions (rows and columns) of the two matrices should match.
	 * @return A SparseMatrix containing the parsed transition rewards.
	 */
	static storm::storage::SparseMatrix<double> parseDeterministicTransitionRewards(std::string const& filename, storm::storage::SparseMatrix<double> const & transitionMatrix);

private:

	/*
	 * Performs the first pass on the input pointed to by the given buffer to obtain the number of
	 * transitions and the maximum node id.
	 *
	 * @param buffer The buffer that cointains the input.
	 * @param insertDiagonalEntriesIfMissing A flag set iff entries on the primary diagonal of the matrix should be added in case they are missing in the parsed file.
	 * @return A structure representing the result of the first pass.
	 */
	static FirstPassResult firstPass(char* buffer, bool insertDiagonalEntriesIfMissing = true);

	/*
	 * The main parsing routine.
	 * Opens the given file, calls the first pass and performs the second pass, parsing the content of the file into a SparseMatrix.
	 *
	 * @param filename The path of file to be parsed.
	 * @param rewardFile A flag set iff the file to be parsed contains transition rewards.
	 * @param insertDiagonalEntriesIfMissing A flag set iff entries on the primary diagonal of the matrix should be added in case they are missing in the parsed file.
	 * @param transitionMatrix The transition matrix of the model in which the reward matrix is to be used in.
	 *                         The dimensions (rows and columns) of the two matrices should match.
	 * @return A SparseMatrix containing the parsed file contents.
	 */
	static storm::storage::SparseMatrix<double> parse(std::string const& filename, bool isRewardFile, storm::storage::SparseMatrix<double> const & transitionMatrix);

};

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_DETERMINISTICSPARSETRANSITIONPARSER_H_ */
