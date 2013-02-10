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
class AbstractNonDeterministicModel: public AbstractModel<T> {

	public:
	AbstractNonDeterministicModel(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<uint_fast64_t>> choiceIndices,
			std::shared_ptr<std::vector<T>> stateRewardVector,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix)
			: AbstractModel<T>(transitionMatrix, stateLabeling, stateRewardVector, transitionRewardMatrix),
			  choiceIndices(choiceIndices) {
		}

		virtual ~AbstractNonDeterministicModel() {
			// Intentionally left empty.
		}

		AbstractNonDeterministicModel(AbstractNonDeterministicModel const& other) : AbstractModel<T>(other),
				choiceIndices(other.choiceIndices) {

		}

	private:
		/*! A vector of indices mapping states to the choices (rows) in the transition matrix. */
		std::shared_ptr<std::vector<uint_fast64_t>> choiceIndices;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_ */
