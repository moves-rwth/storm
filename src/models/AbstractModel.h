#ifndef STORM_MODELS_ABSTRACTMODEL_H_
#define STORM_MODELS_ABSTRACTMODEL_H_

#include <memory>
#include <vector>
#include <boost/container/flat_set.hpp>
#include <boost/optional.hpp>

#include "src/models/AtomicPropositionsLabeling.h"
#include "src/storage/BitVector.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/Scheduler.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/utility/macros.h"
#include "src/utility/Hash.h"
#include "src/utility/vector.h"

#include "src/exceptions/InvalidOperationException.h"

namespace storm {
namespace models {

/*!
 *  @brief  Enumeration of all supported types of models.
 */
enum ModelType {
    Unknown, DTMC, CTMC, MDP, CTMDP, MA
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
		/*! Copy Constructor for an abstract model from the given transition matrix and
		 * the given labeling of the states. Creates copies of all given references.
		 * @param other The Source Abstract Model
		 */
		AbstractModel(AbstractModel<T> const& other) 
			: transitionMatrix(other.transitionMatrix),
			choiceLabeling(other.choiceLabeling),
			stateLabeling(other.stateLabeling),
			stateRewardVector(other.stateRewardVector),
			transitionRewardMatrix(other.transitionRewardMatrix) {
			// Intentionally left empty.
		}

		/*! Move Constructor for an abstract model from the given transition matrix and
		 * the given labeling of the states. Creates copies of all given references.
		 * @param other The Source Abstract Model
		 */
		AbstractModel(AbstractModel<T>&& other)
			: transitionMatrix(std::move(other.transitionMatrix)), choiceLabeling(std::move(other.choiceLabeling)),
			stateLabeling(std::move(other.stateLabeling)), stateRewardVector(std::move(other.stateRewardVector)),
			transitionRewardMatrix(std::move(other.transitionRewardMatrix)) {
			// Intentionally left empty.
		}

		/*! Constructs an abstract model from the given transition matrix and
		 * the given labeling of the states. Creates copies of all given references.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param optionalStateRewardVector The reward values associated with the states.
		 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
         * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
		 */
		AbstractModel(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
				boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
				: transitionMatrix(transitionMatrix), stateLabeling(stateLabeling) {
					
			if (optionalStateRewardVector) {
				this->stateRewardVector.reset(optionalStateRewardVector.get());
			}
			if (optionalTransitionRewardMatrix) {
				this->transitionRewardMatrix.reset(optionalTransitionRewardMatrix.get());
			}
            if (optionalChoiceLabeling) {
                this->choiceLabeling.reset(optionalChoiceLabeling.get());
            }
		}

		/*! Constructs an abstract model from the given transition matrix and
		 * the given labeling of the states. Moves all given references.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param optionalStateRewardVector The reward values associated with the states.
		 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
		 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
		 */
		AbstractModel(storm::storage::SparseMatrix<T>&& transitionMatrix, storm::models::AtomicPropositionsLabeling&& stateLabeling,
				boost::optional<std::vector<T>>&& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling) :
				transitionMatrix(std::move(transitionMatrix)), choiceLabeling(std::move(optionalChoiceLabeling)),
                stateLabeling(std::move(stateLabeling)), stateRewardVector(std::move(optionalStateRewardVector)),
                transitionRewardMatrix(std::move(optionalTransitionRewardMatrix)) {
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
         * Extracts the dependency graph from the model according to the given partition.
         *
         * @param decomposition A decomposition containing the blocks of the partition of the system.
         * @return A sparse matrix with bool entries that represents the dependency graph of the blocks of the partition.
         */
        template <typename BlockType>
        storm::storage::SparseMatrix<T> extractPartitionDependencyGraph(storm::storage::Decomposition<BlockType> const& decomposition) const {
            uint_fast64_t numberOfStates = decomposition.size();
            
            // First, we need to create a mapping of states to their SCC index, to ease the computation of dependency transitions later.
            std::vector<uint_fast64_t> stateToBlockMap(this->getNumberOfStates());
            for (uint_fast64_t i = 0; i < decomposition.size(); ++i) {
                for (auto state : decomposition[i]) {
                    stateToBlockMap[state] = i;
                }
            }
            
            // The resulting sparse matrix will have as many rows/columns as there are blocks in the partition.
            storm::storage::SparseMatrixBuilder<T> dependencyGraphBuilder(numberOfStates, numberOfStates);
            
            for (uint_fast64_t currentBlockIndex = 0; currentBlockIndex < decomposition.size(); ++currentBlockIndex) {
                // Get the next block.
                typename storm::storage::Decomposition<BlockType>::block_type const& block = decomposition[currentBlockIndex];
                
                // Now, we determine the blocks which are reachable (in one step) from the current block.
                boost::container::flat_set<uint_fast64_t> allTargetBlocks;
                for (auto state : block) {
                    for (auto const& transitionEntry : this->getRows(state)) {
                        uint_fast64_t targetBlock = stateToBlockMap[transitionEntry.first];
                        
                        // We only need to consider transitions that are actually leaving the SCC.
                        if (targetBlock != currentBlockIndex) {
                            allTargetBlocks.insert(targetBlock);
                        }
                    }
                }
                
                // Now we can just enumerate all the target SCCs and insert the corresponding transitions.
                for (auto targetBlock : allTargetBlocks) {
                    dependencyGraphBuilder.addNextValue(currentBlockIndex, targetBlock, storm::utility::constantOne<T>());
                }
            }
            
            return dependencyGraphBuilder.build();
        }

        /*!
         * Retrieves the backward transition relation of the model, i.e. a set of transitions
         * between states that correspond to the reversed transition relation of this model.
         *
         * @return A sparse matrix that represents the backward transitions of this model.
         */
        storm::storage::SparseMatrix<T> getBackwardTransitions() const {
            return this->transitionMatrix.transpose(true);
        }

        /*!
         * Returns an object representing the matrix rows associated with the given state.
         *
         * @param state The state for which to retrieve the rows.
         * @return An object representing the matrix rows associated with the given state.
         */
        virtual typename storm::storage::SparseMatrix<T>::const_rows getRows(uint_fast64_t state) const {
            return this->transitionMatrix.getRowGroup(state);
        }
    
		/*!
		 * Returns the state space size of the model.
		 * @return The size of the state space of the model.
		 */
		virtual uint_fast64_t getNumberOfStates() const {
			return this->getTransitionMatrix().getColumnCount();
		}

		/*!
		 * Returns the number of (non-zero) transitions of the model.
		 * @return The number of (non-zero) transitions of the model.
		 */
		virtual uint_fast64_t getNumberOfTransitions() const {
			return this->getTransitionMatrix().getNonzeroEntryCount();
		}

        /*!
         * Retrieves the initial states of the model.
         *
         * @return The initial states of the model represented by a bit vector.
         */
        storm::storage::BitVector const& getInitialStates() const {
            return this->getLabeledStates("init");
        }
    
		/*!
		 * Returns a bit vector in which exactly those bits are set to true that
		 * correspond to a state labeled with the given atomic proposition.
		 * @param ap The atomic proposition for which to get the bit vector.
		 * @return A bit vector in which exactly those bits are set to true that
		 * correspond to a state labeled with the given atomic proposition.
		 */
		storm::storage::BitVector const& getLabeledStates(std::string const& ap) const {
			return stateLabeling.getLabeledStates(ap);
		}

		/*!
		 * Retrieves whether the given atomic proposition is a valid atomic proposition in this model.
		 * @param atomicProposition The atomic proposition to be checked for validity.
		 * @return True if the given atomic proposition is valid in this model.
		 */
		bool hasAtomicProposition(std::string const& atomicProposition) const {
			return stateLabeling.containsAtomicProposition(atomicProposition);
		}

		/*!
		 * Returns a pointer to the matrix representing the transition probability
		 * function.
		 * @return A pointer to the matrix representing the transition probability
		 * function.
		 */
		storm::storage::SparseMatrix<T> const& getTransitionMatrix() const {
			return transitionMatrix;
		}

		/*!
		 * Returns a pointer to the matrix representing the transition rewards.
		 * @return A pointer to the matrix representing the transition rewards.
		 */
		storm::storage::SparseMatrix<T> const& getTransitionRewardMatrix() const {
			return transitionRewardMatrix.get();
		}

		/*!
		 * Returns a pointer to the vector representing the state rewards.
		 * @return A pointer to the vector representing the state rewards.
		 */
		std::vector<T> const& getStateRewardVector() const {
			return stateRewardVector.get();
		}
    
        /*!
         * Returns the labels for the choices of the model, if there are any.
         * @return The labels for the choices of the model.
         */
        std::vector<boost::container::flat_set<uint_fast64_t>> const& getChoiceLabeling() const {
            return choiceLabeling.get();
        }

		/*!
		 * Returns the set of labels with which the given state is labeled.
         *
         * @param state The state for which to return the set of labels.
		 * @return The set of labels with which the given state is labeled.
		 */
		std::set<std::string> getLabelsForState(uint_fast64_t state) const {
			return stateLabeling.getPropositionsForState(state);
		}

		/*!
		 * Returns the state labeling associated with this model.
		 * @return The state labeling associated with this model.
		 */
		storm::models::AtomicPropositionsLabeling const& getStateLabeling() const {
			return stateLabeling;
		}

		/*!
		 * Retrieves whether this model has a state reward model.
		 * @return True if this model has a state reward model.
		 */
		bool hasStateRewards() const {
			return static_cast<bool>(stateRewardVector);
		}

		/*!
		 * Retrieves whether this model has a transition reward model.
		 * @return True if this model has a transition reward model.
		 */
		bool hasTransitionRewards() const {
			return static_cast<bool>(transitionRewardMatrix);
		}
    
        /*!
         * Retrieves whether this model has a labeling for the choices.
         * @return True if this model has a labeling.
         */
        bool hasChoiceLabeling() const {
            return static_cast<bool>(choiceLabeling);
        }

    void convertTransitionRewardsToStateRewards() {
        STORM_LOG_THROW(this->hasTransitionRewards(), storm::exceptions::InvalidOperationException, "Cannot reduce non-existant transition rewards to state rewards.");
        if (this->hasStateRewards()) {
            storm::utility::vector::addVectorsInPlace(stateRewardVector.get(), transitionMatrix.getPointwiseProductRowSumVector(transitionRewardMatrix.get()));
        } else {
            this->stateRewardVector = transitionMatrix.getPointwiseProductRowSumVector(transitionRewardMatrix.get());
        }
        this->transitionRewardMatrix.reset();
    }
    
		/*!
		 * Retrieves the size of the internal representation of the model in memory.
		 * @return the size of the internal representation of the model in memory
		 * measured in bytes.
		 */
		virtual uint_fast64_t getSizeInMemory() const {
			uint_fast64_t result = transitionMatrix.getSizeInMemory() + stateLabeling.getSizeInMemory();
			if (stateRewardVector) {
				result += getStateRewardVector().size() * sizeof(T);
			}
			if (hasTransitionRewards()) {
				result += getTransitionRewardMatrix().getSizeInMemory();
			}
			return result;
		}
    
        /*!
         * Prints information about the model to the specified stream.
         * @param out The stream the information is to be printed to.
         */
        virtual void printModelInformationToStream(std::ostream& out) const {
            out << "-------------------------------------------------------------- " << std::endl;
            out << "Model type: \t\t" << this->getType() << std::endl;
            out << "States: \t\t" << this->getNumberOfStates() << std::endl;
            out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
            this->getStateLabeling().printAtomicPropositionsInformationToStream(out);
            out << "Size in memory: \t" << (this->getSizeInMemory())/1024 << " kbytes" << std::endl;
            out << "-------------------------------------------------------------- " << std::endl;
        }
    
		/*!
		 * Calculates a hash over all values contained in this Model.
		 * @return size_t A Hash Value
		 */
		virtual size_t getHash() const {
			std::size_t result = 0;
			boost::hash_combine(result, transitionMatrix.hash());
			boost::hash_combine(result, stateLabeling.getHash());
			if (stateRewardVector) {
				boost::hash_combine(result, storm::utility::Hash<T>::getHash(stateRewardVector.get()));
			}
			if (transitionRewardMatrix) {
				boost::hash_combine(result, transitionRewardMatrix.get().hash());
			}
			return result;
		}

		/*!
		 * Assigns this model a new set of choiceLabels, giving each choice the stateId
		 * @return void
		 */
		virtual void setStateIdBasedChoiceLabeling() = 0;
    
        /*
         * Applies the given scheduler to the model.
         *
         * @param scheduler The scheduler to apply.
         * @return The model resulting from applying the scheduler to the model.
         */
        virtual std::shared_ptr<AbstractModel<T>> applyScheduler(storm::storage::Scheduler const& scheduler) const = 0;
    
        /*!
         * Exports the model to the dot-format and prints the result to the given stream.
         *
         * @param outStream The stream to which the model is to be written. 
         * @param includeLabling If set to true, the states will be exported with their labels.
         * @param subsystem If not null, this represents the subsystem that is to be exported.
         * @param firstValue If not null, the values in this vector are attached to the states.
         * @param secondValue If not null, the values in this vector are attached to the states.
         * @param stateColoring If not null, this is a mapping from states to color codes.
         * @param colors A mapping of color codes to color names.
         * @param finalizeOutput A flag that sets whether or not the dot stream is closed with a curly brace.
         * @return A string containing the exported model in dot-format.
         */
        virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<T> const* firstValue = nullptr, std::vector<T> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const {
            outStream << "digraph model {" << std::endl;
        
            // Write all states to the stream.
            for (uint_fast64_t state = 0, highestStateIndex = this->getNumberOfStates() - 1; state <= highestStateIndex; ++state) {
                if (subsystem == nullptr || subsystem->get(state)) {
                    outStream << "\t" << state;
                    if (includeLabeling || firstValue != nullptr || secondValue != nullptr || stateColoring != nullptr) {
                        outStream << " [ ";
                        
                        // If we need to print some extra information, do so now.
                        if (includeLabeling || firstValue != nullptr || secondValue != nullptr) {
                            outStream << "label = \"" << state << ": ";
                    
                            // Now print the state labeling to the stream if requested.
                            if (includeLabeling) {
                                outStream << "{";
                                bool includeComma = false;
                                for (std::string const& label : this->getLabelsForState(state)) {
                                    if (includeComma) {
                                        outStream << ", ";
                                    } else {
                                        includeComma = true;
                                    }
                                    outStream << label;
                                }
                                outStream << "}";
                            }
                    
                            // If we are to include some values for the state as well, we do so now.
                            if (firstValue != nullptr || secondValue != nullptr) {
                                outStream << " [";
                                if (firstValue != nullptr) {
                                    outStream << (*firstValue)[state];
                                    if (secondValue != nullptr) {
                                        outStream << ", ";
                                    }
                                }
                                if (secondValue != nullptr) {
                                    outStream << (*secondValue)[state];
                                }
                                outStream << "]";
                            }
                            outStream << "\"";
                    
                            // Now, we color the states if there were colors given.
                            if (stateColoring != nullptr && colors != nullptr) {
                                outStream << ", ";
                                outStream << " style = filled, fillcolor = " << (*colors)[(*stateColoring)[state]];
                            }
                        }
                        outStream << " ]";
                    }
                    outStream << ";" << std::endl;
                }
            }
            
            // If this methods has not been called from a derived class, we want to close the digraph here.
            if (finalizeOutput) {
                outStream << "}" << std::endl;
            }
        }
    
protected:
		/*! A matrix representing the likelihoods of moving between states. */
		storm::storage::SparseMatrix<T> transitionMatrix;

		/*! The labeling that is associated with the choices for each state. */
        boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabeling;
private:
		/*! The labeling of the states of the model. */
		storm::models::AtomicPropositionsLabeling stateLabeling;

		/*! The state-based rewards of the model. */
		boost::optional<std::vector<T>> stateRewardVector;

		/*! The transition-based rewards of the model. */
		boost::optional<storm::storage::SparseMatrix<T>> transitionRewardMatrix;
};

} // namespace models
} // namespace storm

#endif /* STORM_MODELS_ABSTRACTMODEL_H_ */
