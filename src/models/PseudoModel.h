#ifndef STORM_MODELS_PSEUDOMODEL_H_
#define STORM_MODELS_PSEUDOMODEL_H_

#include <cstdint> 
#include "src/storage/SparseMatrix.h"
#include "src/storage/Decomposition.h"
#include "src/storage/StateBlock.h"

namespace storm {
	namespace models {
		// Forward declare the abstract model class.
		template <typename ValueType> class AbstractModel;

		/*!
		* This classes encapsulate the model/transitionmatrix on which the SCC decomposition is performed.
		* The Abstract Base class is specialized by the two possible representations:
		* - For a model the implementation ModelBasedPseudoModel hands the call to getRows() through to the model
		* - For a matrix of a nondeterministic model the implementation NonDeterministicMatrixBasedPseudoModel emulates the call
		*   on the matrix itself like the model function would
		* - For a matrix of a deterministic model the implementation DeterministicMatrixBasedPseudoModel emulates the call
		*   on the matrix itself like the model function would
		*/
		template <typename ValueType>
		class AbstractPseudoModel {
		public:
			AbstractPseudoModel() {}
			virtual ~AbstractPseudoModel() {}
			
			virtual typename storm::storage::SparseMatrix<ValueType>::const_rows getRows(uint_fast64_t state) const = 0;
			
			/*!
			 *	Calculates the number of states in the represented system.	
			 *	@return The Number of States in the underlying model/transition matrix
			 */
			virtual uint_fast64_t getNumberOfStates() const = 0;

			/*!
			 * Extracts the dependency graph from a (pseudo-) model according to the given partition.
			 *
			 * @param decomposition A decomposition containing the blocks of the partition of the system.
			 * @return A sparse matrix with bool entries that represents the dependency graph of the blocks of the partition.
			 */
			virtual storm::storage::SparseMatrix<ValueType> extractPartitionDependencyGraph(storm::storage::Decomposition<storm::storage::StateBlock> const& decomposition) const;
		};

		template <typename ValueType>
		class ModelBasedPseudoModel : public AbstractPseudoModel<ValueType> {
		public:
			/*!
			* Creates an encapsulation for the SCC decomposition based on a model
			* @param model The Model on which the decomposition is to be performed
			*/
			ModelBasedPseudoModel(storm::models::AbstractModel<ValueType> const& model);
			virtual typename storm::storage::SparseMatrix<ValueType>::const_rows getRows(uint_fast64_t state) const override;
			virtual uint_fast64_t getNumberOfStates() const override;
		private:
			storm::models::AbstractModel<ValueType> const& _model;
		};

		template <typename ValueType>
		class NonDeterministicMatrixBasedPseudoModel : public AbstractPseudoModel<ValueType> {
		public:
			/*!
			* Creates an encapsulation for the SCC decomposition based on a matrix
			* @param matrix The Matrix on which the decomposition is to be performed
			*/
			NonDeterministicMatrixBasedPseudoModel(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices);
			virtual typename storm::storage::SparseMatrix<ValueType>::const_rows getRows(uint_fast64_t state) const override;
			virtual uint_fast64_t getNumberOfStates() const override;
		private:
			storm::storage::SparseMatrix<ValueType> const& _matrix;
			std::vector<uint_fast64_t> const& _nondeterministicChoiceIndices;
		};

		template <typename ValueType>
		class DeterministicMatrixBasedPseudoModel : public AbstractPseudoModel<ValueType> {
		public:
			/*!
			* Creates an encapsulation for the SCC decomposition based on a matrix
			* @param matrix The Matrix on which the decomposition is to be performed
			*/
			DeterministicMatrixBasedPseudoModel(storm::storage::SparseMatrix<ValueType> const& matrix);
			virtual typename storm::storage::SparseMatrix<ValueType>::const_rows getRows(uint_fast64_t state) const override;
			virtual uint_fast64_t getNumberOfStates() const override;
		private:
			storm::storage::SparseMatrix<ValueType> const& _matrix;
		};
	}
}

#endif // STORM_MODELS_PSEUDOMODEL_H_