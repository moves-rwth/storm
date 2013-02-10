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
		AbstractDeterministicModel(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<T>> stateRewardVector, std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix)
			: AbstractModel<T>(transitionMatrix, stateLabeling, stateRewardVector, transitionRewardMatrix), backwardTransitions(nullptr) {
		}

		virtual ~AbstractDeterministicModel() {
			// Intentionally left empty.
		}

		AbstractDeterministicModel(AbstractDeterministicModel const& other) : AbstractModel<T>(other), backwardTransitions(nullptr) {
			if (other.backwardTransitions != nullptr) {
				backwardTransitions = new storm::models::GraphTransitions<T>(*other.backwardTransitions);
			}
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
