#include "src/models/PseudoModel.h"
#include "src/utility/constants.h"
#include "src/models/AbstractModel.h"

namespace storm {
	namespace models {

		template<typename ValueType>
		ModelBasedPseudoModel<ValueType>::ModelBasedPseudoModel(storm::models::AbstractModel<ValueType> const& model) : _model(model) {
			// Intentionally left empty.
		}

		template<typename ValueType>
		NonDeterministicMatrixBasedPseudoModel<ValueType>::NonDeterministicMatrixBasedPseudoModel(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) : _matrix(matrix), _nondeterministicChoiceIndices(nondeterministicChoiceIndices) {
			// Intentionally left empty.
		}

		template<typename ValueType>
		DeterministicMatrixBasedPseudoModel<ValueType>::DeterministicMatrixBasedPseudoModel(storm::storage::SparseMatrix<ValueType> const& matrix) : _matrix(matrix) {
			// Intentionally left empty.
		}

		template<typename ValueType>
		typename storm::storage::SparseMatrix<ValueType>::const_rows
		ModelBasedPseudoModel<ValueType>::getRows(uint_fast64_t state) const {
			return this->_model.getRows(state);
		}

		template<typename ValueType>
		typename storm::storage::SparseMatrix<ValueType>::const_rows
		NonDeterministicMatrixBasedPseudoModel<ValueType>::getRows(uint_fast64_t state) const {
			return this->_matrix.getRows(this->_nondeterministicChoiceIndices[state], this->_nondeterministicChoiceIndices[state + 1] - 1);
		}

		template<typename ValueType>
		typename storm::storage::SparseMatrix<ValueType>::const_rows
		DeterministicMatrixBasedPseudoModel<ValueType>::getRows(uint_fast64_t state) const {
			return this->_matrix.getRows(state, state);
		}

		template<typename ValueType>
		uint_fast64_t
		ModelBasedPseudoModel<ValueType>::getNumberOfStates() const {
			return this->_model.getNumberOfStates();
		}

		template<typename ValueType>
		uint_fast64_t
		NonDeterministicMatrixBasedPseudoModel<ValueType>::getNumberOfStates() const {
			return this->_matrix.getColumnCount();
		}

		template<typename ValueType>
		uint_fast64_t
		DeterministicMatrixBasedPseudoModel<ValueType>::getNumberOfStates() const {
			return this->_matrix.getColumnCount();
		}

		template<typename ValueType>
		storm::storage::SparseMatrix<ValueType>
		AbstractPseudoModel<ValueType>::extractPartitionDependencyGraph(storm::storage::Decomposition<storm::storage::StateBlock> const& decomposition) const {
			uint_fast64_t numberOfStates = decomposition.size();

			// First, we need to create a mapping of states to their SCC index, to ease the computation of dependency transitions later.
			std::vector<uint_fast64_t> stateToBlockMap(this->getNumberOfStates());
			for (uint_fast64_t i = 0; i < decomposition.size(); ++i) {
				for (auto state : decomposition[i]) {
					stateToBlockMap[state] = i;
				}
			}

			// The resulting sparse matrix will have as many rows/columns as there are blocks in the partition.
			storm::storage::SparseMatrixBuilder<ValueType> dependencyGraphBuilder(numberOfStates, numberOfStates);

			for (uint_fast64_t currentBlockIndex = 0; currentBlockIndex < decomposition.size(); ++currentBlockIndex) {
				// Get the next block.
				typename storm::storage::StateBlock const& block = decomposition[currentBlockIndex];

				// Now, we determine the blocks which are reachable (in one step) from the current block.
				boost::container::flat_set<uint_fast64_t> allTargetBlocks;
				for (auto state : block) {
					for (auto const& transitionEntry : this->getRows(state)) {
						uint_fast64_t targetBlock = stateToBlockMap[transitionEntry.getColumn()];

						// We only need to consider transitions that are actually leaving the SCC.
						if (targetBlock != currentBlockIndex) {
							allTargetBlocks.insert(targetBlock);
						}
					}
				}

				// Now we can just enumerate all the target SCCs and insert the corresponding transitions.
				for (auto targetBlock : allTargetBlocks) {
					dependencyGraphBuilder.addNextValue(currentBlockIndex, targetBlock, storm::utility::constantOne<ValueType>());
				}
			}

			return dependencyGraphBuilder.build();
		}

		template class ModelBasedPseudoModel<double>;
		template class NonDeterministicMatrixBasedPseudoModel<double>;
		template class DeterministicMatrixBasedPseudoModel<double>;
		template class ModelBasedPseudoModel <float> ;
		template class NonDeterministicMatrixBasedPseudoModel <float>;
		template class DeterministicMatrixBasedPseudoModel <float>;
		template class ModelBasedPseudoModel<int>;
		template class NonDeterministicMatrixBasedPseudoModel<int>;
		template class DeterministicMatrixBasedPseudoModel<int>;
	} // namespace models
} // namespace storm