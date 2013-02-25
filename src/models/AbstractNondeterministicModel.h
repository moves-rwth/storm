#ifndef STORM_MODELS_ABSTRACTNONDETERMINISTICMODEL_H_
#define STORM_MODELS_ABSTRACTNONDETERMINISTICMODEL_H_

#include "AbstractModel.h"
#include "GraphTransitions.h"

#include <memory>

namespace storm {

namespace models {

/*!
 *	@brief	Base class for all non-deterministic model classes.
 *
 *	This is base class defines a common interface for all non-deterministic models.
 */
template<class T>
class AbstractNondeterministicModel: public AbstractModel<T> {

	public:
		/*! Constructs an abstract non-determinstic model from the given parameters.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param choiceIndices A mapping from states to rows in the transition matrix.
		 * @param stateRewardVector The reward values associated with the states.
		 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
		 */
		AbstractNondeterministicModel(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices,
			std::shared_ptr<std::vector<T>> stateRewardVector,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix)
			: AbstractModel<T>(transitionMatrix, stateLabeling, stateRewardVector, transitionRewardMatrix),
			  nondeterministicChoiceIndices(nondeterministicChoiceIndices) {
		}

		/*!
		 * Destructor.
		 */
		virtual ~AbstractNondeterministicModel() {
			// Intentionally left empty.
		}

		/*!
		 * Copy Constructor.
		 */
		AbstractNondeterministicModel(AbstractNondeterministicModel const& other) : AbstractModel<T>(other),
				nondeterministicChoiceIndices(other.nondeterministicChoiceIndices) {
			// Intentionally left empty.
		}

		/*!
		 * Retrieves the size of the internal representation of the model in memory.
		 * @return the size of the internal representation of the model in memory
		 * measured in bytes.
		 */
		virtual uint_fast64_t getSizeInMemory() const {
			return AbstractModel<T>::getSizeInMemory() + nondeterministicChoiceIndices->size() * sizeof(uint_fast64_t);
		}

		/*!
		 * Retrieves the vector indicating which matrix rows represent non-deterministic choices
		 * of a certain state.
		 * @param the vector indicating which matrix rows represent non-deterministic choices
		 * of a certain state.
		 */
		std::shared_ptr<std::vector<uint_fast64_t>> getNondeterministicChoiceIndices() const {
			return nondeterministicChoiceIndices;
		}

	private:
		/*! A vector of indices mapping states to the choices (rows) in the transition matrix. */
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_ */
