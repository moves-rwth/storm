#ifndef STORM_MODELS_ABSTRACTNONDETERMINISTICMODEL_H_
#define STORM_MODELS_ABSTRACTNONDETERMINISTICMODEL_H_

#include <memory>

#include "AbstractModel.h"
#include "src/storage/Scheduler.h"

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
             * All values are copied.
             * @param transitionMatrix The matrix representing the transitions in the model.
             * @param stateLabeling The labeling that assigns a set of atomic
             * propositions to each state.
             * @param choiceIndices A mapping from states to rows in the transition matrix.
             * @param stateRewardVector The reward values associated with the states.
             * @param transitionRewardMatrix The reward values associated with the transitions of the model.
             * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
             */
            AbstractNondeterministicModel(storm::storage::SparseMatrix<T> const& transitionMatrix,
                                          storm::models::AtomicPropositionsLabeling const& stateLabeling,
                                          std::vector<uint_fast64_t> const& nondeterministicChoiceIndices,
                                          boost::optional<std::vector<T>> const& optionalStateRewardVector,
                                          boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
                                          boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
			: AbstractModel<T>(transitionMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
				this->nondeterministicChoiceIndices = nondeterministicChoiceIndices;
            }
            
            /*! Constructs an abstract non-determinstic model from the given parameters.
             * All values are moved.
             * @param transitionMatrix The matrix representing the transitions in the model.
             * @param stateLabeling The labeling that assigns a set of atomic
             * propositions to each state.
             * @param choiceIndices A mapping from states to rows in the transition matrix.
             * @param stateRewardVector The reward values associated with the states.
             * @param transitionRewardMatrix The reward values associated with the transitions of the model.
             */
            AbstractNondeterministicModel(storm::storage::SparseMatrix<T>&& transitionMatrix,
                                          storm::models::AtomicPropositionsLabeling&& stateLabeling,
                                          std::vector<uint_fast64_t>&& nondeterministicChoiceIndices,
                                          boost::optional<std::vector<T>>&& optionalStateRewardVector,
                                          boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
                                          boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling)
                // The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
                : AbstractModel<T>(std::move(transitionMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
                               std::move(optionalChoiceLabeling)), nondeterministicChoiceIndices(std::move(nondeterministicChoiceIndices)) {
                // Intentionally left empty.
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
             * Move Constructor.
             */
            AbstractNondeterministicModel(AbstractNondeterministicModel&& other) : AbstractModel<T>(std::move(other)),
                nondeterministicChoiceIndices(std::move(other.nondeterministicChoiceIndices)) {
                // Intentionally left empty.
            }
            
            /*!
             * Returns the number of choices for all states of the MDP.
             * @return The number of choices for all states of the MDP.
             */
            uint_fast64_t getNumberOfChoices() const {
                return this->transitionMatrix.getRowCount();
            }
            
            /*!
             * Retrieves the size of the internal representation of the model in memory.
             *
             * @return the size of the internal representation of the model in memory
             * measured in bytes.
             */
            virtual uint_fast64_t getSizeInMemory() const {
                return AbstractModel<T>::getSizeInMemory() + nondeterministicChoiceIndices.size() * sizeof(uint_fast64_t);
            }
            
            /*!
             * Retrieves the vector indicating which matrix rows represent non-deterministic choices
             * of a certain state.
             * @param the vector indicating which matrix rows represent non-deterministic choices
             * of a certain state.
             */
            std::vector<uint_fast64_t> const& getNondeterministicChoiceIndices() const {
                return nondeterministicChoiceIndices;
            }
            
            virtual typename storm::storage::SparseMatrix<T>::const_rows getRows(uint_fast64_t state) const override {
                return this->transitionMatrix.getRows(nondeterministicChoiceIndices[state], nondeterministicChoiceIndices[state + 1] - 1);
            }
        
            /*!
             * Retrieves the backward transition relation of the model, i.e. a set of transitions
             * between states that correspond to the reversed transition relation of this model.
             * Note: This overwrites the getBackwardsTransitions of the AbstractModel, since the simple
             * transposition of the state matrix does not yield correct results for non-deterministic models.
             *
             * @return A sparse matrix that represents the backward transitions of this model.
             */
            storm::storage::SparseMatrix<T> getBackwardTransitions() const {
                return this->getTransitionMatrix().transpose(true);
            }

            /*!
             * Calculates a hash over all values contained in this Model.
             * @return size_t A Hash Value
             */
            virtual size_t getHash() const override {
                std::size_t result = 0;
                std::size_t hashTmp = storm::utility::Hash<uint_fast64_t>::getHash(nondeterministicChoiceIndices);
                boost::hash_combine(result, AbstractModel<T>::getHash());
                boost::hash_combine(result, hashTmp);
                return result;
            }

            /*!
            * Prints information about the model to the specified stream.
            * @param out The stream the information is to be printed to.
            */
            virtual void printModelInformationToStream(std::ostream& out) const override {
                out << "-------------------------------------------------------------- " << std::endl;
                out << "Model type: \t\t" << this->getType() << std::endl;
                out << "States: \t\t" << this->getNumberOfStates() << std::endl;
                out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
                out << "Choices: \t\t" << this->getNumberOfChoices() << std::endl;
                this->getStateLabeling().printAtomicPropositionsInformationToStream(out);
                out << "Size in memory: \t" << (this->getSizeInMemory())/1024 << " kbytes" << std::endl;
                out << "-------------------------------------------------------------- " << std::endl;
            }

            virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<T> const* firstValue = nullptr, std::vector<T> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const override {
                AbstractModel<T>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
    
                // Write the probability distributions for all the states.
                for (uint_fast64_t state = 0, highestStateIndex = this->getNumberOfStates() - 1; state <= highestStateIndex; ++state) {
                    uint_fast64_t rowCount = nondeterministicChoiceIndices[state + 1] - nondeterministicChoiceIndices[state];
                    bool highlightChoice = true;
        
                    // For this, we need to iterate over all available nondeterministic choices in the current state.
                    for (uint_fast64_t choice = 0; choice < rowCount; ++choice) {
                        typename storm::storage::SparseMatrix<T>::const_rows row = this->transitionMatrix.getRow(nondeterministicChoiceIndices[state] + choice);
                        
                        if (scheduler != nullptr) {
                            // If the scheduler picked the current choice, we will not make it dotted, but highlight it.
                            if ((*scheduler)[state] == choice) {
                                highlightChoice = true;
                            } else {
                                highlightChoice = false;
                            }
                        }
            
                        // For each nondeterministic choice, we draw an arrow to an intermediate node to better display
                        // the grouping of transitions.
                        outStream << "\t\"" << state << "c" << choice << "\" [shape = \"point\"";
            
                        // If we were given a scheduler to highlight, we do so now.
                        if (scheduler != nullptr) {
                            if (highlightChoice) {
                                outStream << ", fillcolor=\"red\"";
                            }
                        }
                        outStream << "];" << std::endl;
                        
                        outStream << "\t" << state << " -> \"" << state << "c" << choice << "\"";
            
                        // If we were given a scheduler to highlight, we do so now.
                        if (scheduler != nullptr) {
                            if (highlightChoice) {
                                outStream << " [color=\"red\", penwidth = 2]";
                            } else {
                                outStream << " [style = \"dotted\"]";
                            }
                        }
                        outStream << ";" << std::endl;
            
                        // Now draw all probabilitic arcs that belong to this nondeterminstic choice.
                        for (auto const& transition : row) {
                            if (subsystem == nullptr || subsystem->get(transition.first)) {
                                outStream << "\t\"" << state << "c" << choice << "\" -> " << transition.first << " [ label= \"" << transition.second << "\" ]";
                                
                                // If we were given a scheduler to highlight, we do so now.
                                if (scheduler != nullptr) {
                                    if (highlightChoice) {
                                        outStream << " [color=\"red\", penwidth = 2]";
                                    } else {
                                        outStream << " [style = \"dotted\"]";
                                    }
                                }
                                outStream << ";" << std::endl;
                            }
                        }
                    }
                }
    
                if (finalizeOutput) {
                    outStream << "}" << std::endl;
                }
            }

            /*!
             * Assigns this model a new set of choiceLabels, giving each choice a label with the stateId
             * @return void
             */
            virtual void setStateIdBasedChoiceLabeling() override {
                std::vector<boost::container::flat_set<uint_fast64_t>> newChoiceLabeling;
    
                size_t stateCount = this->getNumberOfStates();
                size_t choiceCount = this->getNumberOfChoices();
                newChoiceLabeling.resize(choiceCount);
    
                for (size_t state = 0; state < stateCount; ++state) {
                    for (size_t choice = this->nondeterministicChoiceIndices.at(state); choice < this->nondeterministicChoiceIndices.at(state + 1); ++choice) {
                        newChoiceLabeling.at(choice).insert(state);
                    }
                }
    
                this->choiceLabeling.reset(newChoiceLabeling);
            }
        
        protected:
            /*! A vector of indices mapping states to the choices (rows) in the transition matrix. */
            std::vector<uint_fast64_t> nondeterministicChoiceIndices;
        };
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_ */
