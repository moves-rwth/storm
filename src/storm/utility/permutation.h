#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace storm {

namespace storage {
class BitVector;
template<typename ValueType>
class SparseMatrix;
}  // namespace storage

namespace utility::permutation {
using index_type = uint64_t;

/*!
 * The order in which the states of a matrix are visited in a depth-first search or breadth-first search traversal.
 */
enum class OrderKind { Bfs, Dfs, ReverseBfs, ReverseDfs, Random };

/*!
 * Converts the given order to a string.
 */
std::string orderKindtoString(OrderKind order);

/*!
 * Returns a list of possible order kinds
 */
std::vector<std::string> orderKinds();

/*!
 * Gets the order from the given string.
 */
OrderKind orderKindFromString(std::string const& order);

/*!
 * Creates a random (uniformly distributed) permutation of the given size.
 */
std::vector<index_type> createRandomPermutation(index_type size);

/*!
 * Creates a random (uniformly distributed) permutation of the given size.
 * It is guaranteed that the same seed will always produce the same permutation.
 */
std::vector<index_type> createRandomPermutation(index_type size, index_type seed);

/*!
 * Creates a permutation that orders the states of the given matrix in the given exploration order.
 *
 * @note the resulting permutation will only contain those states that are reachable from the initial states.
 *
 * Example:
 * Let permutation[i_0] = 0, permutation[i_1] = 1, ..., permutation[i_n-1] = n-1.
 * i_0 is the firs initial state.
 * If the order is Dfs, i_1 is a successor of i_0, i_2 is a successor of i_1, etc. (assuming those successors exist).
 * If the order is Bfs, i_1 is the second initial state, ...
 *
 * @pre initialStates.size() == transitionMatrix.getRowGroupCount() == transitionMatrix.getColumnCount()
 */
template<typename ValueType>
std::vector<index_type> createPermutation(OrderKind order, storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                          storm::storage::BitVector const& initialStates);

/*!
 * Inverts the given permutation.
 * @return a vector v such that v[permutation[i]] == permutation[v[i]] == i for all i.
 */
std::vector<index_type> invertPermutation(std::vector<index_type> const& permutation);

/*!
 * Reverses the given permutation.
 * @note this does not reverse the given vector, but the permutation itself.
 * @return a vector v such that v[i] == permutation.size() - 1 - permutation[i] for all i.
 */
void reversePermutationInPlace(std::vector<index_type>& permutation);

/*!
 * Returns true if the given vector is a permutation of the numbers 0, 1, ..., n-1 for n = permutation.size().
 * In other words, every integer from 0 to n-1 occurs exactly once in the vector.
 */
bool isValidPermutation(std::vector<index_type> const& permutation);

}  // namespace utility::permutation
}  // namespace storm