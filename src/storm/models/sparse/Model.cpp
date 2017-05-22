#include "storm/models/sparse/Model.h"

#include <boost/algorithm/string/join.hpp>

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/utility/vector.h"
#include "storm/adapters/CarlAdapter.h"
#include "storm/utility/NumberTraits.h"

#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace models {
        namespace sparse {
            template<typename ValueType, typename RewardModelType>
            Model<ValueType, RewardModelType>::Model(storm::models::ModelType const& modelType,
                                    storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                    storm::models::sparse::StateLabeling const& stateLabeling,
                                    std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                    boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling)
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
                                    boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling)
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
            RewardModelType& Model<ValueType, RewardModelType>::rewardModel(std::string const& rewardModelName) {
                STORM_LOG_ASSERT(this->hasRewardModel(rewardModelName), "Model has no reward model.");
                return this->rewardModels.find(rewardModelName)->second;
            }

            template<typename ValueType, typename RewardModelType>
            RewardModelType const& Model<ValueType, RewardModelType>::getRewardModel(std::string const& rewardModelName) const {
                auto it = this->rewardModels.find(rewardModelName);
                if (it == this->rewardModels.end()) {
                    if (rewardModelName.empty()) {
                        if (this->hasUniqueRewardModel()) {
                            return this->getUniqueRewardModel();
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Unable to refer to default reward model, because there is no default model or it is not unique.");
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "The requested reward model '" << rewardModelName << "' does not exist.");
                    }
                }
                return it->second;
            }

            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::addRewardModel(std::string const& rewardModelName, RewardModelType const& newRewardModel) {
                if(this->hasRewardModel(rewardModelName)) {
                    STORM_LOG_THROW(!(this->hasRewardModel(rewardModelName)), storm::exceptions::IllegalArgumentException, "A reward model with the given name '" << rewardModelName << "' already exists.");
                }
                STORM_LOG_ASSERT(newRewardModel.isCompatible(this->getNumberOfStates(), this->getTransitionMatrix().getRowCount()), "New reward model is not compatible.");
                this->rewardModels.emplace(rewardModelName, newRewardModel);
            }

            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::removeRewardModel(std::string const& rewardModelName) {
                auto it = this->rewardModels.find(rewardModelName);
                bool res = (it != this->rewardModels.end());
                if(res) {
                    this->rewardModels.erase(it->first);
                }
                return res;
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
            RewardModelType const& Model<ValueType, RewardModelType>::getUniqueRewardModel() const {
                STORM_LOG_THROW(this->getNumberOfRewardModels() == 1, storm::exceptions::IllegalFunctionCallException, "The reward model is not unique.");
                return this->rewardModels.cbegin()->second;
            }
            
            template<typename ValueType, typename RewardModelType>
            RewardModelType& Model<ValueType, RewardModelType>::getUniqueRewardModel() {
                STORM_LOG_THROW(this->getNumberOfRewardModels() == 1, storm::exceptions::IllegalFunctionCallException, "The reward model is not unique.");
                return this->rewardModels.begin()->second;
            }
            
            template<typename ValueType, typename RewardModelType>
            uint_fast64_t Model<ValueType, RewardModelType>::getNumberOfRewardModels() const {
                return this->rewardModels.size();
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::models::sparse::ChoiceLabeling const& Model<ValueType, RewardModelType>::getChoiceLabeling() const {
                return choiceLabeling.get();
            }
            
            template<typename ValueType, typename RewardModelType>
            boost::optional<storm::models::sparse::ChoiceLabeling> const& Model<ValueType, RewardModelType>::getOptionalChoiceLabeling() const {
                return choiceLabeling;
            }

            template<typename ValueType, typename RewardModelType>
            boost::optional<storm::models::sparse::ChoiceLabeling>& Model<ValueType, RewardModelType>::getOptionalChoiceLabeling() {
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
                if (this->hasChoiceLabeling()) {
                    this->getChoiceLabeling().printLabelingInformationToStream(out);
                } else {
                    out << "Choice Labels: \tnone";
                }
                out << "-------------------------------------------------------------- " << std::endl;
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::printRewardModelsInformationToStream(std::ostream& out) const {
                if (this->rewardModels.size()) {
                    std::vector<std::string> rewardModelNames;
                    std::for_each(this->rewardModels.cbegin(), this->rewardModels.cend(),
                                  [&rewardModelNames] (typename std::pair<std::string, RewardModelType> const& nameRewardModelPair) {
                                      if (nameRewardModelPair.first.empty()) { rewardModelNames.push_back("(default)"); } else { rewardModelNames.push_back(nameRewardModelPair.first); }
                                  });
                    out << "Reward Models:  " << boost::join(rewardModelNames, ", ") << std::endl;
                } else {
                    out << "Reward Models:  none" << std::endl;
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>*, bool finalizeOutput) const {
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
            bool Model<ValueType, RewardModelType>::supportsParameters() const {
#ifdef STORM_HAVE_CARL
                return std::is_same<ValueType, storm::RationalFunction>::value;
#else
		return false;
#endif
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::hasParameters() const {
                if (!this->supportsParameters()) {
                    return false;
                }
                // Check for parameters
                for (auto const& entry : this->getTransitionMatrix()) {
                    if (!storm::utility::isConstant(entry.getValue())) {
                        return true;
                    }
                }
                // Only constant values present
                return false;
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::isExact() const {
                return storm::NumberTraits<ValueType>::IsExact && storm::NumberTraits<typename RewardModelType::ValueType>::IsExact;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::unordered_map<std::string, RewardModelType>& Model<ValueType, RewardModelType>::getRewardModels() {
                return this->rewardModels;
            }

            template<typename ValueType, typename RewardModelType>
            std::unordered_map<std::string, RewardModelType> const& Model<ValueType, RewardModelType>::getRewardModels() const {
                return this->rewardModels;
            }
            
#ifdef STORM_HAVE_CARL
            std::set<storm::RationalFunctionVariable> getProbabilityParameters(Model<storm::RationalFunction> const& model) {
                return storm::storage::getVariables(model.getTransitionMatrix());
            }



            std::set<storm::RationalFunctionVariable> getRewardParameters(Model<storm::RationalFunction> const& model) {
                std::set<storm::RationalFunctionVariable> result;
                for(auto rewModel : model.getRewardModels()) {
                    std::set<storm::RationalFunctionVariable> tmp = getRewardModelParameters(rewModel.second);
                    result.insert(tmp.begin(), tmp.end());
                }
                return result;
            }
#endif
            
            template class Model<double>;
            template class Model<float>;

#ifdef STORM_HAVE_CARL
            template class Model<storm::RationalNumber>;

            template class Model<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Model<storm::RationalFunction>;
#endif
        }
    }
}
