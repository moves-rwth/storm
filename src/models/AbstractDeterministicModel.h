#ifndef STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_
#define STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_

#include "AbstractModel.h"

#include <memory>
#include <sstream>

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
		 * All values are copied.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param stateRewardVector The reward values associated with the states.
		 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
		 */
		AbstractDeterministicModel(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
				boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix)
			: AbstractModel<T>(transitionMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix) {
		}

		/*! Constructs an abstract determinstic model from the given parameters.
		 * Moves all references.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param stateRewardVector The reward values associated with the states.
		 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
		 */
		AbstractDeterministicModel(storm::storage::SparseMatrix<T>&& transitionMatrix, storm::models::AtomicPropositionsLabeling&& stateLabeling,
				boost::optional<std::vector<T>>&& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix)
				// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
			: AbstractModel<T>(std::move(transitionMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix)) {
			// Intentionally left empty.
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
		AbstractDeterministicModel(AbstractDeterministicModel const& other) : AbstractModel<T>(other) {
			// Intentionally left empty.
		}

		/*!
		 * Move Constructor.
		 */
		AbstractDeterministicModel(AbstractDeterministicModel && other) : AbstractModel<T>(std::move(other)) {
			// Intentionally left empty.
		}
    
        /*!
         * Returns an iterator to the successors of the given state.
         *
         * @param state The state for which to return the iterator.
         * @return An iterator to the successors of the given state.
         */
        virtual typename storm::storage::SparseMatrix<T>::ConstIndexIterator constStateSuccessorIteratorBegin(uint_fast64_t state) const {
            return this->transitionMatrix.constColumnIteratorBegin(state);
        }
    
        /*!
         * Returns an iterator pointing to the element past the successors of the given state.
         *
         * @param state The state for which to return the iterator.
         * @return An iterator pointing to the element past the successors of the given state.
         */
        virtual typename storm::storage::SparseMatrix<T>::ConstIndexIterator constStateSuccessorIteratorEnd(uint_fast64_t state) const {
            return this->transitionMatrix.constColumnIteratorEnd(state);
        }

		/*!
		 * Calculates a hash over all values contained in this Model.
		 * @return size_t A Hash Value
		 */
		virtual std::size_t getHash() const override {
			return AbstractModel<T>::getHash();
		}
    
        virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<T> const* firstValue = nullptr, std::vector<T> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const override {
            AbstractModel<T>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
            
            // Simply iterate over all transitions and draw the arrows with probability information attached.
            for (uint_fast64_t i = 0; i < this->transitionMatrix.getRowCount(); ++i) {
                for (auto transitionIt = this->transitionMatrix.begin(i), transitionIte = this->transitionMatrix.end(i); transitionIt != transitionIte; ++transitionIt) {
                    if (*transitionIt != storm::utility::constGetZero<T>()) {
                        if (subsystem == nullptr || subsystem->get(transitionIt.column())) {
                            outStream << "\t" << i << " -> " << transitionIt.column() << " [ label= \"" << *transitionIt << "\" ];" << std::endl;
                        }
                    }
                }
            }
                        
            if (finalizeOutput) {
                outStream << "}" << std::endl;
            }
        }
};

} // namespace models
} // namespace storm

#endif /* STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_ */
