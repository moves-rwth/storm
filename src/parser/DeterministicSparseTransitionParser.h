#ifndef STORM_PARSER_TRAPARSER_H_
#define STORM_PARSER_TRAPARSER_H_

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
	 *	@brief	Load a deterministic transition system from file and create a
	 *	sparse adjacency matrix whose entries represent the weights of the edges
	 */
	static storm::storage::SparseMatrix<double> parseDeterministicTransitions(std::string const& filename, bool insertDiagonalEntriesIfMissing = true);

	/*!
	 *	@brief	Load the transition rewards for a deterministic transition system from file and create a
	 *	sparse adjacency matrix whose entries represent the rewards of the respective transitions.
	 */
	static storm::storage::SparseMatrix<double> parseDeterministicTransitionRewards(std::string const& filename, RewardMatrixInformationStruct const& rewardMatrixInformation);

private:

	/*
	 * Performs the first pass on the input pointed to by the given buffer.
	 *
	 * @param buffer The buffer that cointains the input.
	 * @param lineEndings The line endings that are to be used while parsing.
	 * @param insertDiagonalEntriesIfMissing Flag determining whether
	 * @return A structure representing the result of the first pass.
	 */
	static FirstPassResult firstPass(char* buffer, SupportedLineEndingsEnum lineEndings, bool insertDiagonalEntriesIfMissing = true);

};

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_TRAPARSER_H_ */
