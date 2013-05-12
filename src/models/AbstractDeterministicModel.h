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
			: AbstractModel<T>(transitionMatrix, stateLabeling, stateRewardVector, transitionRewardMatrix) {
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
                    for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator succIt = this->getTransitionMatrix()->constColumnIteratorBegin(state), succIte = this->getTransitionMatrix()->constColumnIteratorEnd(state); succIt != succIte; ++succIt) {
                        uint_fast64_t targetScc = stateToSccMap.find(*succIt)->second;
                            
                        // We only need to consider transitions that are actually leaving the SCC.
                        if (targetScc != currentSccIndex) {
                            allTargetSccs.insert(targetScc);
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
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_ */
