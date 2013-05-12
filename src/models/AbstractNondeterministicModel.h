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
		 * Returns the number of choices for all states of the MDP.
		 * @return The number of choices for all states of the MDP.
		 */
		uint_fast64_t getNumberOfChoices() const {
			return this->getTransitionMatrix()->getRowCount();
		}

        /*!
         * Extracts the SCC dependency graph from the model according to the given SCC decomposition.
         *
         * @param stronglyConnectedComponents A vector containing the SCCs of the system.
         * @param stateToSccMap A mapping from state indices to
         */
        virtual storm::storage::SparseMatrix<bool> extractSccDependencyGraph(std::vector<std::vector<uint_fast64_t>> const& stronglyConnectedComponents, std::map<uint_fast64_t, uint_fast64_t> const& stateToSccMap) {
            // The resulting sparse matrix will have as many rows/columns as there are SCCs.
            uint_fast64_t numberOfStates = stronglyConnectedComponents.size();
            storm::storage::SparseMatrix<bool> sccDependencyGraph(numberOfStates);
            sccDependencyGraph.initialize();
            
            for (uint_fast64_t currentSccIndex = 0; currentSccIndex < stronglyConnectedComponents.size(); ++currentSccIndex) {
                // Get the actual SCC.
                std::vector<uint_fast64_t> const& scc = stronglyConnectedComponents[currentSccIndex];
                
                // Now, we determine the SCCs which are reachable (in one step) from the current SCC.
                std::set<uint_fast64_t> allTargetSccs;
                for (auto state : scc) {
                    for (uint_fast64_t rowIndex = (*nondeterministicChoiceIndices)[state]; rowIndex < (*nondeterministicChoiceIndices)[state + 1]; ++rowIndex) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator succIt = this->getTransitionMatrix()->constColumnIteratorBegin(rowIndex), succIte = this->getTransitionMatrix()->constColumnIteratorEnd(rowIndex); succIt != succIte; ++succIt) {
                            uint_fast64_t targetScc = stateToSccMap.find(*succIt)->second;
                            
                            // We only need to consider transitions that are actually leaving the SCC.
                            if (targetScc != currentSccIndex) {
                                allTargetSccs.insert(targetScc);
                            }
                        }
                    }
                }
                
                // Now we can just enumerate all the target SCCs and insert the corresponding transitions.
                for (auto targetScc : allTargetSccs) {
                    sccDependencyGraph.insertNextValue(currentSccIndex, targetScc, true);
                }
            }
            
            // Finalize the matrix.
            sccDependencyGraph.finalize(true);
            
            return sccDependencyGraph;
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
