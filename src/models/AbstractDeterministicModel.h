#ifndef STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_
#define STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_

#include "AbstractModel.h"
#include "GraphTransitions.h"

#include <memory>

namespace storm {

namespace models {

/*!
 *	@brief	Base class for all deterministic model classes.
 *
 *	This is base class defines a common interface for all deterministic models.
 */
template<class T>
class AbstractDeterministicModel: public AbstractModel<T> {

	public:
		/*! Constructs an abstract determinstic model from the given parameters.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param stateRewardVector The reward values associated with the states.
		 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
		 */
		AbstractDeterministicModel(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<T>> stateRewardVector, std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix)
			: AbstractModel<T>(transitionMatrix, stateLabeling, stateRewardVector, transitionRewardMatrix), backwardTransitions(nullptr) {
		}

		/*!
		 * Destructor.
		 */
		virtual ~AbstractDeterministicModel() {
			// Intentionally left empty.
		}

		/*!
		 * Copy Constructor.
		 */
		AbstractDeterministicModel(AbstractDeterministicModel const& other) : AbstractModel<T>(other), backwardTransitions(nullptr) {
			if (other.backwardTransitions != nullptr) {
				backwardTransitions = new storm::models::GraphTransitions<T>(*other.backwardTransitions);
			}
		}

		/*!
		 * Retrieves the size of the internal representation of the model in memory.
		 * @return the size of the internal representation of the model in memory
		 * measured in bytes.
		 */
		virtual uint_fast64_t getSizeInMemory() const {
			uint_fast64_t result = AbstractModel<T>::getSizeInMemory();
			if (backwardTransitions != nullptr) {
				result += backwardTransitions->getSizeInMemory();
			}
			return result;
		}

		/*!
		 * Retrieves a reference to the backwards transition relation.
		 * @return A reference to the backwards transition relation.
		 */
		storm::models::GraphTransitions<T>& getBackwardTransitions() {
			if (this->backwardTransitions == nullptr) {
				this->backwardTransitions = std::shared_ptr<storm::models::GraphTransitions<T>>(new storm::models::GraphTransitions<T>(this->getTransitionMatrix(), this->getNumberOfStates(), false));
			}
			return *this->backwardTransitions;
		}

	private:
		/*!
		 * A data structure that stores the predecessors for all states. This is
		 * needed for backwards directed searches.
		 */
		std::shared_ptr<storm::models::GraphTransitions<T>> backwardTransitions;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_ */
