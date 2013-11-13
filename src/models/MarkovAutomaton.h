/*
 * MarkovAutomaton.h
 *
 *  Created on: 07.11.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELS_MA_H_
#define STORM_MODELS_MA_H_

#include "AbstractNondeterministicModel.h"
#include "AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/settings/Settings.h"
#include "src/utility/vector.h"

namespace storm {
	namespace models {

		template <class T>
		class MarkovAutomaton : public storm::models::AbstractNondeterministicModel<T> {

		public:
			MarkovAutomaton(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
							std::vector<uint_fast64_t>&& nondeterministicChoiceIndices,
							storm::storage::BitVector const& markovianChoices, std::vector<T> const& exitRates,
							boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
							boost::optional<std::vector<storm::storage::VectorSet<uint_fast64_t>>> const& optionalChoiceLabeling)
							: AbstractNondeterministicModel<T>(transitionMatrix, stateLabeling, nondeterministicChoiceIndices, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling),
							  markovianChoices(markovianChoices), exitRates(exitRates) {
				if (this->hasTransitionRewards()) {
					if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
						LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
						throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
					}
				}
			}

			MarkovAutomaton(storm::storage::SparseMatrix<T>&& transitionMatrix,
							storm::models::AtomicPropositionsLabeling&& stateLabeling,
							std::vector<uint_fast64_t>&& nondeterministicChoiceIndices,
							storm::storage::BitVector const& markovianChoices, std::vector<T> const& exitRates,
							boost::optional<std::vector<T>>&& optionalStateRewardVector,
							boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
							boost::optional<std::vector<storm::storage::VectorSet<uint_fast64_t>>>&& optionalChoiceLabeling)
							// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
							: AbstractNondeterministicModel<T>(std::move(transitionMatrix), std::move(stateLabeling), std::move(nondeterministicChoiceIndices), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
																std::move(optionalChoiceLabeling)), markovianChoices(std::move(markovianChoices)), exitRates(std::move(exitRates)) {
		        if (this->hasTransitionRewards()) {
		            if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
		                LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
		                throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
		            }
		        }
			}

			MarkovAutomaton(MarkovAutomaton<T> const & markovAutomaton) : AbstractNondeterministicModel<T>(markovAutomaton) {
				// Intentionally left empty.
			}

			MarkovAutomaton(MarkovAutomaton<T>&& markovAutomaton) : AbstractNondeterministicModel<T>(std::move(markovAutomaton)) {
				// Intentionally left empty.
			}

			~MarkovAutomaton() {
				// Intentionally left empty.
			}

			storm::models::ModelType getType() const {
				return MA;
			}

		private:

			std::vector<T> exitRates;
			storm::storage::BitVector markovianChoices;

		};
	}
}

#endif /* STORM_MODELS_MA_H_ */
