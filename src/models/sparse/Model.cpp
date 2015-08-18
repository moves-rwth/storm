#include "src/models/sparse/Model.h"

#include <boost/algorithm/string/join.hpp>

#include "src/utility/vector.h"
#include "src/adapters/CarlAdapter.h"

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace models {
        namespace sparse {
            template<typename ValueType, typename RewardModelType>
            Model<ValueType, RewardModelType>::Model(storm::models::ModelType const& modelType,
                                    storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                    storm::models::sparse::StateLabeling const& stateLabeling,
                                    std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                    boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : ModelBase(modelType), transitionMatrix(transitionMatrix), stateLabeling(stateLabeling),
            rewardModels(rewardModels), choiceLabeling(optionalChoiceLabeling) {
                for (auto const& rewardModel : this->getRewardModels()) {
                    STORM_LOG_THROW(!rewardModel.second.hasTransitionRewards() || rewardModel.second.getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix()), storm::exceptions::IllegalArgumentException, "The transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            Model<ValueType, RewardModelType>::Model(storm::models::ModelType const& modelType,
                                    storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                    storm::models::sparse::StateLabeling&& stateLabeling,
                                    std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                    boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : ModelBase(modelType), transitionMatrix(std::move(transitionMatrix)), stateLabeling(std::move(stateLabeling)),
            rewardModels(std::move(rewardModels)), choiceLabeling(std::move(optionalChoiceLabeling)) {
                for (auto const& rewardModel : this->getRewardModels()) {
                    STORM_LOG_THROW(!rewardModel.second.hasTransitionRewards() || rewardModel.second.getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix()), storm::exceptions::IllegalArgumentException, "The transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::storage::SparseMatrix<ValueType> Model<ValueType, RewardModelType>::getBackwardTransitions() const {
                return this->getTransitionMatrix().transpose(true);
            }
            
            template<typename ValueType, typename RewardModelType>
            typename storm::storage::SparseMatrix<ValueType>::const_rows Model<ValueType, RewardModelType>::getRows(storm::storage::sparse::state_type state) const {
                return this->getTransitionMatrix().getRowGroup(state);
            }
            
            template<typename ValueType, typename RewardModelType>
            uint_fast64_t Model<ValueType, RewardModelType>::getNumberOfStates() const  {
                return this->getTransitionMatrix().getColumnCount();
            }
            
            template<typename ValueType, typename RewardModelType>
            uint_fast64_t Model<ValueType, RewardModelType>::getNumberOfTransitions() const {
                return this->getTransitionMatrix().getNonzeroEntryCount();
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::storage::BitVector const& Model<ValueType, RewardModelType>::getInitialStates() const {
                return this->getStates("init");
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::storage::BitVector const& Model<ValueType, RewardModelType>::getStates(std::string const& label) const {
                return stateLabeling.getStates(label);
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::hasLabel(std::string const& label) const {
                return stateLabeling.containsLabel(label);
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::storage::SparseMatrix<ValueType> const& Model<ValueType, RewardModelType>::getTransitionMatrix() const {
                return transitionMatrix;
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::storage::SparseMatrix<ValueType>& Model<ValueType, RewardModelType>::getTransitionMatrix() {
                return transitionMatrix;
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::hasRewardModel(std::string const& rewardModelName) const {
                return this->rewardModels.find(rewardModelName) != this->rewardModels.end();
            }
            
            template<typename ValueType, typename RewardModelType>
            RewardModelType const& Model<ValueType, RewardModelType>::getRewardModel(std::string const& rewardModelName) const {
                auto it = this->rewardModels.find(rewardModelName);
                STORM_LOG_THROW(it != this->rewardModels.end(), storm::exceptions::IllegalArgumentException, "The requested reward model does not exist.");
                return it->second;
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::hasUniqueRewardModel() const {
                return this->getNumberOfRewardModels() == 1;
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::hasRewardModel() const {
                return !this->rewardModels.empty();
            }
            
            template<typename ValueType, typename RewardModelType>
            typename std::unordered_map<std::string, RewardModelType>::const_iterator Model<ValueType, RewardModelType>::getUniqueRewardModel() const {
                STORM_LOG_THROW(this->getNumberOfRewardModels() == 1, storm::exceptions::IllegalFunctionCallException, "The reward model is not unique.");
                return this->rewardModels.cbegin();
            }
            
            template<typename ValueType, typename RewardModelType>
            uint_fast64_t Model<ValueType, RewardModelType>::getNumberOfRewardModels() const {
                return this->rewardModels.size();
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<LabelSet> const& Model<ValueType, RewardModelType>::getChoiceLabeling() const {
                return choiceLabeling.get();
            }
            
            template<typename ValueType, typename RewardModelType>
            boost::optional<std::vector<LabelSet>> const& Model<ValueType, RewardModelType>::getOptionalChoiceLabeling() const {
                return choiceLabeling;
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::models::sparse::StateLabeling const& Model<ValueType, RewardModelType>::getStateLabeling() const {
                return stateLabeling;
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::models::sparse::StateLabeling& Model<ValueType, RewardModelType>::getStateLabeling() {
                return stateLabeling;
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::hasChoiceLabeling() const {
                return static_cast<bool>(choiceLabeling);
            }
            
            template<typename ValueType, typename RewardModelType>
            std::size_t Model<ValueType, RewardModelType>::getSizeInBytes() const {
                std::size_t result = transitionMatrix.getSizeInBytes() + stateLabeling.getSizeInBytes();
                for (auto const& rewardModel : this->rewardModels) {
                    result += rewardModel.second.getSizeInBytes();
                }
                if (hasChoiceLabeling()) {
                    result += getChoiceLabeling().size() * sizeof(LabelSet);
                }
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                this->printModelInformationFooterToStream(out);
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::printModelInformationHeaderToStream(std::ostream& out) const {
                out << "-------------------------------------------------------------- " << std::endl;
                out << "Model type: \t" << this->getType() << " (sparse)" << std::endl;
                out << "States: \t" << this->getNumberOfStates() << std::endl;
                out << "Transitions: \t" << this->getNumberOfTransitions() << std::endl;
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::printModelInformationFooterToStream(std::ostream& out) const {
                this->printRewardModelsInformationToStream(out);
                this->getStateLabeling().printLabelingInformationToStream(out);
                out << "Size in memory: \t" << (this->getSizeInBytes())/1024 << " kbytes" << std::endl;
                out << "-------------------------------------------------------------- " << std::endl;
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::printRewardModelsInformationToStream(std::ostream& out) const {
                if (this->rewardModels.size()) {
                    std::vector<std::string> rewardModelNames;
                    std::for_each(this->rewardModels.cbegin(), this->rewardModels.cend(), [&rewardModelNames] (typename std::pair<std::string, RewardModelType> const& nameRewardModelPair) { rewardModelNames.push_back(nameRewardModelPair.first); });
                    out << "Reward Models: " << boost::join(rewardModelNames, ", ") << std::endl;
                } else {
                    out << "Reward Models: none" << std::endl;
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>* scheduler, bool finalizeOutput) const {
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
            
            template<typename ValueType, typename RewardModelType>
            std::set<std::string> Model<ValueType, RewardModelType>::getLabelsOfState(storm::storage::sparse::state_type state) const {
                return this->stateLabeling.getLabelsOfState(state);
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::setTransitionMatrix(storm::storage::SparseMatrix<ValueType> const& transitionMatrix) {
                this->transitionMatrix = transitionMatrix;
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::setTransitionMatrix(storm::storage::SparseMatrix<ValueType>&& transitionMatrix) {
                this->transitionMatrix = std::move(transitionMatrix);
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::isSparseModel() const {
                return true;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::unordered_map<std::string, RewardModelType>& Model<ValueType, RewardModelType>::getRewardModels() {
                return this->rewardModels;
            }

            template<typename ValueType, typename RewardModelType>
            std::unordered_map<std::string, RewardModelType> const& Model<ValueType, RewardModelType>::getRewardModels() const {
                return this->rewardModels;
            }

            
            template class Model<double>;
            template class Model<float>;
            
#ifdef STORM_HAVE_CARL
            template class Model<storm::RationalFunction>;
#endif
            
        }
    }
}