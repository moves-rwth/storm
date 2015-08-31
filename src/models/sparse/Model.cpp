#include "src/models/sparse/Model.h"

#include "src/utility/vector.h"
#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            template<typename ValueType>
            Model<ValueType>::Model(storm::models::ModelType const& modelType,
                                    storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                    storm::models::sparse::StateLabeling const& stateLabeling,
                                    boost::optional<std::vector<ValueType>> const& optionalStateRewardVector,
                                    boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix,
                                    boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : ModelBase(modelType),
            transitionMatrix(transitionMatrix),
            stateLabeling(stateLabeling),
            stateRewardVector(optionalStateRewardVector),
            transitionRewardMatrix(optionalTransitionRewardMatrix),
            choiceLabeling(optionalChoiceLabeling) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            Model<ValueType>::Model(storm::models::ModelType const& modelType,
                                    storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                    storm::models::sparse::StateLabeling&& stateLabeling,
                                    boost::optional<std::vector<ValueType>>&& optionalStateRewardVector,
                                    boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix,
                                    boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : ModelBase(modelType),
            transitionMatrix(std::move(transitionMatrix)),
            stateLabeling(std::move(stateLabeling)),
            stateRewardVector(std::move(optionalStateRewardVector)),
            transitionRewardMatrix(std::move(optionalTransitionRewardMatrix)),
            choiceLabeling(std::move(optionalChoiceLabeling)) {
                // Intentionally left empty.
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<ValueType> Model<ValueType>::getBackwardTransitions() const {
                return this->getTransitionMatrix().transpose(true);
            }
            
            template <typename ValueType>
            typename storm::storage::SparseMatrix<ValueType>::const_rows Model<ValueType>::getRows(storm::storage::sparse::state_type state) const {
                return this->getTransitionMatrix().getRowGroup(state);
            }
            
            template <typename ValueType>
            uint_fast64_t Model<ValueType>::getNumberOfStates() const  {
                return this->getTransitionMatrix().getColumnCount();
            }
            
            template <typename ValueType>
            uint_fast64_t Model<ValueType>::getNumberOfTransitions() const {
                return this->getTransitionMatrix().getNonzeroEntryCount();
            }
            
            template <typename ValueType>
            storm::storage::BitVector const& Model<ValueType>::getInitialStates() const {
                return this->getStates("init");
            }
            
            template <typename ValueType>
            storm::storage::BitVector const& Model<ValueType>::getStates(std::string const& label) const {
                return stateLabeling.getStates(label);
            }
            
            template <typename ValueType>
            bool Model<ValueType>::hasLabel(std::string const& label) const {
                return stateLabeling.containsLabel(label);
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<ValueType> const& Model<ValueType>::getTransitionMatrix() const {
                return transitionMatrix;
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<ValueType>& Model<ValueType>::getTransitionMatrix() {
                return transitionMatrix;
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<ValueType> const& Model<ValueType>::getTransitionRewardMatrix() const {
                return transitionRewardMatrix.get();
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<ValueType>& Model<ValueType>::getTransitionRewardMatrix() {
                return transitionRewardMatrix.get();
            }
            
            template <typename ValueType>
            boost::optional<storm::storage::SparseMatrix<ValueType>> const& Model<ValueType>::getOptionalTransitionRewardMatrix() const {
                return transitionRewardMatrix;
            }
            
            template <typename ValueType>
            std::vector<ValueType> const& Model<ValueType>::getStateRewardVector() const {
                return stateRewardVector.get();
            }
            
            template <typename ValueType>
            std::vector<ValueType>& Model<ValueType>::getStateRewardVector() {
                return stateRewardVector.get();
            }
            
            template <typename ValueType>
            boost::optional<std::vector<ValueType>> const& Model<ValueType>::getOptionalStateRewardVector() const {
                return stateRewardVector;
            }
            
            template <typename ValueType>
            std::vector<LabelSet> const& Model<ValueType>::getChoiceLabeling() const {
                return choiceLabeling.get();
            }
            
            template <typename ValueType>
            boost::optional<std::vector<LabelSet>> const& Model<ValueType>::getOptionalChoiceLabeling() const {
                return choiceLabeling;
            }
            
            template <typename ValueType>
            storm::models::sparse::StateLabeling const& Model<ValueType>::getStateLabeling() const {
                return stateLabeling;
            }
            
            template <typename ValueType>
            storm::models::sparse::StateLabeling& Model<ValueType>::getStateLabeling() {
                return stateLabeling;
            }
            
            template <typename ValueType>
            bool Model<ValueType>::hasStateRewards() const {
                return static_cast<bool>(stateRewardVector);
            }
            
            template <typename ValueType>
            bool Model<ValueType>::hasTransitionRewards() const {
                return static_cast<bool>(transitionRewardMatrix);
            }
            
            template <typename ValueType>
            bool Model<ValueType>::hasChoiceLabeling() const {
                return static_cast<bool>(choiceLabeling);
            }
            
            template <typename ValueType>
            void Model<ValueType>::convertTransitionRewardsToStateRewards() {
                STORM_LOG_THROW(this->hasTransitionRewards(), storm::exceptions::InvalidOperationException, "Cannot reduce non-existant transition rewards to state rewards.");
                if (this->hasStateRewards()) {
                    storm::utility::vector::addVectors(stateRewardVector.get(), transitionMatrix.getPointwiseProductRowSumVector(transitionRewardMatrix.get()), stateRewardVector.get());
                } else {
                    this->stateRewardVector = transitionMatrix.getPointwiseProductRowSumVector(transitionRewardMatrix.get());
                }
                this->transitionRewardMatrix = boost::optional<storm::storage::SparseMatrix<ValueType>>();
            }
            
            template <typename ValueType>
            std::size_t Model<ValueType>::getSizeInBytes() const {
                std::size_t result = transitionMatrix.getSizeInBytes() + stateLabeling.getSizeInBytes();
                if (stateRewardVector) {
                    result += getStateRewardVector().size() * sizeof(ValueType);
                }
                if (hasTransitionRewards()) {
                    result += getTransitionRewardMatrix().getSizeInBytes();
                }
                if (hasChoiceLabeling()) {
                    result += getChoiceLabeling().size() * sizeof(LabelSet);
                }
                return result;
            }
            
            template <typename ValueType>
            void Model<ValueType>::printModelInformationToStream(std::ostream& out) const {
                out << "-------------------------------------------------------------- " << std::endl;
                out << "Model type: \t" << this->getType() << " (sparse)" << std::endl;
                out << "States: \t" << this->getNumberOfStates() << std::endl;
                out << "Transitions: \t" << this->getNumberOfTransitions() << std::endl;
                this->getStateLabeling().printLabelingInformationToStream(out);
                out << "Size in memory: \t" << (this->getSizeInBytes())/1024 << " kbytes" << std::endl;
                out << "-------------------------------------------------------------- " << std::endl;
            }
            
            template <typename ValueType>
            void Model<ValueType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>* scheduler, bool finalizeOutput) const {
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
                                    for (std::string const& label : this->getLabelsOfState(state)) {
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
            
            template <typename ValueType>
            std::set<std::string> Model<ValueType>::getLabelsOfState(storm::storage::sparse::state_type state) const {
                return this->stateLabeling.getLabelsOfState(state);
            }
            
            template <typename ValueType>
            void Model<ValueType>::setTransitionMatrix(storm::storage::SparseMatrix<ValueType> const& transitionMatrix) {
                this->transitionMatrix = transitionMatrix;
            }
            
            template <typename ValueType>
            void Model<ValueType>::setTransitionMatrix(storm::storage::SparseMatrix<ValueType>&& transitionMatrix) {
                this->transitionMatrix = std::move(transitionMatrix);
            }
            
            template <typename ValueType>
            bool Model<ValueType>::isSparseModel() const {
                return true;
            }
            
            template class Model<double>;
            template class Model<float>;
            
#ifdef STORM_HAVE_CARL
            template class Model<storm::RationalFunction>;
#endif
            
        }
    }
}