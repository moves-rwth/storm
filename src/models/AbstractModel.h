#ifndef STORM_MODELS_ABSTRACTMODEL_H_
#define STORM_MODELS_ABSTRACTMODEL_H_

#include "src/models/AtomicPropositionsLabeling.h"
#include "src/storage/BitVector.h"
#include "src/storage/SparseMatrix.h"
#include "src/utility/CommandLine.h"

#include <memory>
#include <vector>

namespace storm {
namespace models {

/*!
 *  @brief  Enumeration of all supported types of models.
 */
enum ModelType {
    Unknown, DTMC, CTMC, MDP, CTMDP
};

/*!
 *	@brief	Stream output operator for ModelType.
 */
std::ostream& operator<<(std::ostream& os, ModelType const type);

/*!
 *	@brief	Base class for all model classes.
 *
 *	This is base class defines a common interface for all models to identify
 *	their type and obtain the special model.
 */
template<class T>
class AbstractModel: public std::enable_shared_from_this<AbstractModel<T>> {

	public:
		/*! Constructs an abstract model from the given transition matrix and
		 * the given labeling of the states.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param stateRewardVector The reward values associated with the states.
		 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
		 */
		AbstractModel(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix,
				std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
				std::shared_ptr<std::vector<T>> stateRewardVector, std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix)
				: transitionMatrix(transitionMatrix), stateLabeling(stateLabeling), stateRewardVector(stateRewardVector), transitionRewardMatrix(transitionRewardMatrix) {
			// Intentionally left empty.
		}

		/*!
		 * Destructor.
		 */
		virtual ~AbstractModel() {
			// Intentionally left empty.
		}

		/*!
		 *	@brief Casts the model to the model type that was actually
		 *	created.
		 *
		 *	As all methods that work on generic models will use this
		 *	AbstractModel class, this method provides a convenient way to
		 *	cast an AbstractModel object to an object of a concrete model
		 *	type, which can be obtained via getType(). The mapping from an
		 *	element of the ModelType enum to the actual class must be done
		 *	by the caller.
		 *
		 *	This methods uses std::dynamic_pointer_cast internally.
		 *
		 *	@return Shared pointer of new type to this object.
		 */
		template <typename Model>
		std::shared_ptr<Model> as() {
			return std::dynamic_pointer_cast<Model>(this->shared_from_this());
		}
		
		/*!
		 *	@brief Return the actual type of the model.
		 *
		 *	Each model must implement this method.
		 *
		 *	@return	Type of the model.
		 */
		virtual ModelType getType() const = 0;

        /*!
         * Extracts the SCC dependency graph from the model according to the given SCC decomposition.
         *
         * @param stronglyConnectedComponents A vector containing the SCCs of the system.
         * @param stateToSccMap A mapping from state indices to
         */
        virtual storm::storage::SparseMatrix<bool> extractSccDependencyGraph(std::vector<std::vector<uint_fast64_t>> const& stronglyConnectedComponents, std::map<uint_fast64_t, uint_fast64_t> const& stateToSccMap) = 0;
    
		/*!
		 * Returns the state space size of the model.
		 * @return The size of the state space of the model.
		 */
		virtual uint_fast64_t getNumberOfStates() const {
			return this->getTransitionMatrix()->getColumnCount();
		}

		/*!
		 * Returns the number of (non-zero) transitions of the model.
		 * @return The number of (non-zero) transitions of the model.
		 */
		virtual uint_fast64_t getNumberOfTransitions() const {
			return this->getTransitionMatrix()->getNonZeroEntryCount();
		}

		/*!
		 * Returns a bit vector in which exactly those bits are set to true that
		 * correspond to a state labeled with the given atomic proposition.
		 * @param ap The atomic proposition for which to get the bit vector.
		 * @return A bit vector in which exactly those bits are set to true that
		 * correspond to a state labeled with the given atomic proposition.
		 */
		storm::storage::BitVector const& getLabeledStates(std::string const& ap) const {
			return stateLabeling->getLabeledStates(ap);
		}

		/*!
		 * Retrieves whether the given atomic proposition is a valid atomic proposition in this model.
		 * @param atomicProposition The atomic proposition to be checked for validity.
		 * @return True if the given atomic proposition is valid in this model.
		 */
		bool hasAtomicProposition(std::string const& atomicProposition) const {
			return stateLabeling->containsAtomicProposition(atomicProposition);
		}

		/*!
		 * Returns a pointer to the matrix representing the transition probability
		 * function.
		 * @return A pointer to the matrix representing the transition probability
		 * function.
		 */
		std::shared_ptr<storm::storage::SparseMatrix<T>> getTransitionMatrix() const {
			return transitionMatrix;
		}

		/*!
		 * Returns a pointer to the matrix representing the transition rewards.
		 * @return A pointer to the matrix representing the transition rewards.
		 */
		std::shared_ptr<storm::storage::SparseMatrix<T>> getTransitionRewardMatrix() const {
			return transitionRewardMatrix;
		}

		/*!
		 * Returns a pointer to the vector representing the state rewards.
		 * @return A pointer to the vector representing the state rewards.
		 */
		std::shared_ptr<std::vector<T>> getStateRewardVector() const {
			return stateRewardVector;
		}

		/*!
		 * Returns the set of states with which the given state is labeled.
		 * @return The set of states with which the given state is labeled.
		 */
		std::set<std::string> const getPropositionsForState(uint_fast64_t const& state) const {
			return stateLabeling->getPropositionsForState(state);
		}

		/*!
		 * Returns the state labeling associated with this model.
		 * @return The state labeling associated with this model.
		 */
		std::shared_ptr<storm::models::AtomicPropositionsLabeling> getStateLabeling() const {
			return stateLabeling;
		}

		/*!
		 * Retrieves whether this model has a state reward model.
		 * @return True if this model has a state reward model.
		 */
		bool hasStateRewards() const {
			return stateRewardVector != nullptr;
		}

		/*!
		 * Retrieves whether this model has a transition reward model.
		 * @return True if this model has a transition reward model.
		 */
		bool hasTransitionRewards() const {
			return transitionRewardMatrix != nullptr;
		}

		/*!
		 * Retrieves the size of the internal representation of the model in memory.
		 * @return the size of the internal representation of the model in memory
		 * measured in bytes.
		 */
		virtual uint_fast64_t getSizeInMemory() const {
			uint_fast64_t result = transitionMatrix->getSizeInMemory() + stateLabeling->getSizeInMemory();
			if (stateRewardVector != nullptr) {
				result += stateRewardVector->size() * sizeof(T);
			}
			if (transitionRewardMatrix != nullptr) {
				result += transitionRewardMatrix->getSizeInMemory();
			}
			return result;
		}

		/*!
		 * Prints information about the model to the specified stream.
		 * @param out The stream the information is to be printed to.
		 */
		void printModelInformationToStream(std::ostream& out) const {
			out << "-------------------------------------------------------------- "
				<< std::endl;
			out << "Model type: \t\t" << this->getType() << std::endl;
			out << "States: \t\t" << this->getNumberOfStates() << std::endl;
			out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
			this->getStateLabeling()->printAtomicPropositionsInformationToStream(out);
			out << "Size in memory: \t"
				<< (this->getSizeInMemory())/1024 << " kbytes" << std::endl;
			out << "-------------------------------------------------------------- "
				<< std::endl;
		}

	private:
		/*! A matrix representing the likelihoods of moving between states. */
		std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix;

		/*! The labeling of the states of the model. */
		std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling;

		/*! The state-based rewards of the model. */
		std::shared_ptr<std::vector<T>> stateRewardVector;

		/*! The transition-based rewards of the model. */
		std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix;
};

} // namespace models
} // namespace storm

#endif /* STORM_MODELS_ABSTRACTMODEL_H_ */
