#include "storm/models/sparse/Model.h"

#include <boost/algorithm/string/join.hpp>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/IllegalFunctionCallException.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/vector.h"
#include "storm/io/export.h"
#include "storm/utility/NumberTraits.h"


namespace storm {
    namespace models {
        namespace sparse {

            template <typename ValueType, typename RewardModelType>
            Model<ValueType, RewardModelType>::Model(ModelType modelType, storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components)
            : storm::models::Model<ValueType>(modelType), transitionMatrix(components.transitionMatrix), stateLabeling(components.stateLabeling), rewardModels(components.rewardModels),
                      choiceLabeling(components.choiceLabeling), stateValuations(components.stateValuations), choiceOrigins(components.choiceOrigins) {
                assertValidityOfComponents(components);
            }
            
            template <typename ValueType, typename RewardModelType>
            Model<ValueType, RewardModelType>::Model(ModelType modelType, storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components)
            : storm::models::Model<ValueType>(modelType), transitionMatrix(std::move(components.transitionMatrix)), stateLabeling(std::move(components.stateLabeling)), rewardModels(std::move(components.rewardModels)),
                      choiceLabeling(std::move(components.choiceLabeling)), stateValuations(std::move(components.stateValuations)), choiceOrigins(std::move(components.choiceOrigins)) {
                assertValidityOfComponents(components);
            }
            
            template <typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::assertValidityOfComponents(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components) const {
                
                // More costly checks are only asserted to avoid doing them in release mode.
                
                uint_fast64_t stateCount = this->getNumberOfStates();
                uint_fast64_t choiceCount = this->getTransitionMatrix().getRowCount();
                
                // general components for all model types.
                STORM_LOG_THROW(this->getTransitionMatrix().getColumnCount() == stateCount, storm::exceptions::IllegalArgumentException, "Invalid column count of transition matrix.");
                STORM_LOG_ASSERT(components.rateTransitions || this->hasParameters() || this->getTransitionMatrix().isProbabilistic(), "The matrix is not probabilistic.");
                STORM_LOG_THROW(this->getStateLabeling().getNumberOfItems() == stateCount, storm::exceptions::IllegalArgumentException, "Invalid item count (" << this->getStateLabeling().getNumberOfItems() << ") of state labeling (states: " << stateCount << ").");
                for (auto const& rewardModel : this->getRewardModels()) {
                    STORM_LOG_THROW(!rewardModel.second.hasStateRewards() || rewardModel.second.getStateRewardVector().size() == stateCount, storm::exceptions::IllegalArgumentException, "Invalid size (" << rewardModel.second.getStateRewardVector().size() << ") of state reward vector (states:" << stateCount  << ").");
                    STORM_LOG_THROW(!rewardModel.second.hasStateActionRewards() || rewardModel.second.getStateActionRewardVector().size() == choiceCount, storm::exceptions::IllegalArgumentException, "Invalid size of state reward vector.");
                    STORM_LOG_ASSERT(!rewardModel.second.hasTransitionRewards() || rewardModel.second.getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix()), "The transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
                }
                STORM_LOG_THROW(!this->hasChoiceLabeling() || this->getChoiceLabeling().getNumberOfItems() == choiceCount, storm::exceptions::IllegalArgumentException, "Invalid item count of choice labeling.");
                STORM_LOG_THROW(!this->hasStateValuations() || this->getStateValuations().getNumberOfStates() == stateCount, storm::exceptions::IllegalArgumentException, "Invalid choice count for choice origins.");
                STORM_LOG_THROW(!this->hasChoiceOrigins() || this->getChoiceOrigins()->getNumberOfChoices() == choiceCount, storm::exceptions::IllegalArgumentException, "Invalid choice count for choice origins.");
                
                // Branch on type of nondeterminism
                if (this->isOfType(ModelType::Dtmc) || this->isOfType(ModelType::Ctmc)) {
                    STORM_LOG_THROW(this->getTransitionMatrix().hasTrivialRowGrouping(), storm::exceptions::IllegalArgumentException, "Can not create deterministic model: Transition matrix has non-trivial row grouping.");
                    STORM_LOG_THROW(stateCount == this->getTransitionMatrix().getRowCount(), storm::exceptions::IllegalArgumentException, "Can not create deterministic model: Number of rows of transition matrix does not match state count.");
                    STORM_LOG_THROW(stateCount == this->getTransitionMatrix().getColumnCount(), storm::exceptions::IllegalArgumentException, "Can not create deterministic model: Number of columns of transition matrix does not match state count.");
                    STORM_LOG_ERROR_COND(!components.player1Matrix.is_initialized(), "Player 1 matrix given for a model that is no stochastic game (will be ignored).");
                } else if (this->isOfType(ModelType::Mdp) || this->isOfType(ModelType::MarkovAutomaton) || this->isOfType(ModelType::Pomdp)) {
                    STORM_LOG_THROW(stateCount == this->getTransitionMatrix().getRowGroupCount(), storm::exceptions::IllegalArgumentException, "Can not create nondeterministic model: Number of row groups (" << this->getTransitionMatrix().getRowGroupCount() << ") of transition matrix does not match state count (" << stateCount << ").");
                    STORM_LOG_THROW(stateCount == this->getTransitionMatrix().getColumnCount(), storm::exceptions::IllegalArgumentException, "Can not create nondeterministic model: Number of columns of transition matrix does not match state count.");
                    STORM_LOG_ERROR_COND(!components.player1Matrix.is_initialized(), "Player 1 matrix given for a model that is no stochastic game (will be ignored).");
                } else {
                    STORM_LOG_THROW(this->isOfType(ModelType::S2pg), storm::exceptions::IllegalArgumentException, "Invalid model type.");
                    STORM_LOG_THROW(components.player1Matrix.is_initialized(), storm::exceptions::IllegalArgumentException, "No player 1 matrix given for stochastic game.");
                    STORM_LOG_ASSERT(components.player1Matrix->isProbabilistic(), "Can not create stochastic game: There is a row in the p1 matrix with not exactly one entry.");
                    STORM_LOG_THROW(stateCount == components.player1Matrix->getRowGroupCount(), storm::exceptions::IllegalArgumentException, "Can not create stochastic game: Number of row groups of p1 matrix does not match state count.");
                    STORM_LOG_THROW(this->getTransitionMatrix().getRowGroupCount() == components.player1Matrix->getColumnCount(), storm::exceptions::IllegalArgumentException, "Can not create stochastic game: Number of row groups of p2 matrix does not match column count of p1 matrix.");
                }
                
                // Branch on continuous/discrete timing
                if (this->isOfType(ModelType::Ctmc) || this->isOfType(ModelType::MarkovAutomaton)) {
                    STORM_LOG_THROW(components.rateTransitions || components.exitRates.is_initialized(), storm::exceptions::IllegalArgumentException, "Can not create continuous time model: no rates are given.");
                    STORM_LOG_THROW(!components.exitRates.is_initialized() || components.exitRates->size() == stateCount, storm::exceptions::IllegalArgumentException, "Size of exit rate vector does not match state count.");
                    STORM_LOG_THROW(this->isOfType(ModelType::Ctmc) || components.markovianStates.is_initialized(), storm::exceptions::IllegalArgumentException, "Can not create Markov Automaton: no Markovian states given.");
                } else {
                    STORM_LOG_WARN_COND(!components.rateTransitions && !components.exitRates.is_initialized(), "Rates specified for discrete-time model. The rates will be ignored.");
                }
                STORM_LOG_WARN_COND(this->isOfType(ModelType::MarkovAutomaton) || !components.markovianStates.is_initialized(), "Markovian states given for a model that is not a Markov automaton (will be ignored).");
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
            uint_fast64_t Model<ValueType, RewardModelType>::getNumberOfChoices() const {
                return this->getTransitionMatrix().getRowCount();
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
                if (this->hasRewardModel(rewardModelName)) {
                    STORM_LOG_THROW(!(this->hasRewardModel(rewardModelName)), storm::exceptions::IllegalArgumentException, "A reward model with the given name '" << rewardModelName << "' already exists.");
                }
                STORM_LOG_ASSERT(newRewardModel.isCompatible(this->getNumberOfStates(), this->getTransitionMatrix().getRowCount()), "New reward model is not compatible.");
                this->rewardModels.emplace(rewardModelName, newRewardModel);
            }

            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::removeRewardModel(std::string const& rewardModelName) {
                auto it = this->rewardModels.find(rewardModelName);
                bool res = (it != this->rewardModels.end());
                if (res) {
                    this->rewardModels.erase(it->first);
                }
                return res;
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::restrictRewardModels(std::set<std::string> const& keptRewardModels) {
                std::set<std::string> removedRewardModels;
                for (auto const& rewModel : this->getRewardModels()) {
                    if (keptRewardModels.find(rewModel.first) == keptRewardModels.end()) {
                        removedRewardModels.insert(rewModel.first);
                    }
                }
                for (auto const& rewModelName : removedRewardModels) {
                    this->removeRewardModel(rewModelName);
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::hasUniqueRewardModel() const {
                return this->getNumberOfRewardModels() == 1;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::string const& Model<ValueType, RewardModelType>::getUniqueRewardModelName() const {
                STORM_LOG_THROW(this->getNumberOfRewardModels() == 1, storm::exceptions::IllegalFunctionCallException, "The reward model is not unique.");
                return this->rewardModels.begin()->first;
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
            bool Model<ValueType, RewardModelType>::hasStateValuations() const {
                return static_cast<bool>(stateValuations);
            }
            
            template<typename ValueType, typename RewardModelType>
            storm::storage::sparse::StateValuations const& Model<ValueType, RewardModelType>::getStateValuations() const {
                return stateValuations.get();
            }
            
            template<typename ValueType, typename RewardModelType>
            boost::optional<storm::storage::sparse::StateValuations> const& Model<ValueType, RewardModelType>::getOptionalStateValuations() const {
                return stateValuations;
            }

            template<typename ValueType, typename RewardModelType>
            boost::optional<storm::storage::sparse::StateValuations>& Model<ValueType, RewardModelType>::getOptionalStateValuations() {
                return stateValuations;
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::hasChoiceOrigins() const {
                return static_cast<bool>(choiceOrigins);
            }
            
            template<typename ValueType, typename RewardModelType>
            std::shared_ptr<storm::storage::sparse::ChoiceOrigins> const& Model<ValueType, RewardModelType>::getChoiceOrigins() const {
                return choiceOrigins.get();
            }
            
            template<typename ValueType, typename RewardModelType>
            boost::optional<std::shared_ptr<storm::storage::sparse::ChoiceOrigins>> const& Model<ValueType, RewardModelType>::getOptionalChoiceOrigins() const {
                return choiceOrigins;
            }

            template<typename ValueType, typename RewardModelType>
            boost::optional<std::shared_ptr<storm::storage::sparse::ChoiceOrigins>>& Model<ValueType, RewardModelType>::getOptionalChoiceOrigins() {
                return choiceOrigins;
            }
            
            template<typename ValueType, typename RewardModelType>
            void Model<ValueType, RewardModelType>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                this->printModelInformationFooterToStream(out);
            }

            template<typename ValueType, typename RewardModelType>
            std::size_t Model<ValueType, RewardModelType>::hash() const {
                std::size_t seed = 0;
                boost::hash_combine(seed,transitionMatrix.hash());
                boost::hash_combine(seed,stateLabeling.hash());
                for (auto const& rewModel : rewardModels) {
                    boost::hash_combine(seed,rewModel.second.hash());
                }
                if(choiceLabeling) {
                    boost::hash_combine(seed,choiceLabeling->hash());
                }
                if(stateValuations) {
                    boost::hash_combine(seed,stateValuations->hash());
                }
                if(choiceOrigins) {
                    boost::hash_combine(seed,choiceOrigins.get()->hash());
                }
                return seed;
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
                out << "State Labels: \t";
                this->getStateLabeling().printLabelingInformationToStream(out);
                out << "Choice Labels: \t";
                if (this->hasChoiceLabeling()) {
                    this->getChoiceLabeling().printLabelingInformationToStream(out);
                } else {
                    out << "none" << std::endl;
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
            void Model<ValueType, RewardModelType>::writeDotToStream(std::ostream& outStream, size_t maxWidthLabel, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>*, bool finalizeOutput) const {
                outStream << "digraph model {" << std::endl;

                // Write all states to the stream.
                for (uint_fast64_t state = 0, highestStateIndex = this->getNumberOfStates() - 1; state <= highestStateIndex; ++state) {
                    if (subsystem == nullptr || subsystem->get(state)) {
                        outStream << "\t" << state;
                        if (includeLabeling || firstValue != nullptr || secondValue != nullptr || stateColoring != nullptr || hasStateValuations()) {
                            outStream << " [ ";
                            
                            // If we need to print some extra information, do so now.
                            if (includeLabeling || firstValue != nullptr || secondValue != nullptr || hasStateValuations()) {
                                outStream << "label = \"" << state;
                                if (hasStateValuations()) {
                                    std::string stateInfo = getStateValuations().getStateInfo(state);
                                    std::vector<std::string> results;
                                    boost::split(results, stateInfo, [](char c) { return c == ',';});
                                    storm::utility::outputFixedWidth(outStream, results, maxWidthLabel);
                                }
                                outStream << ": ";
                                
                                // Now print the state labeling to the stream if requested.
                                if (includeLabeling) {
                                    outStream << "{";
                                    storm::utility::outputFixedWidth(outStream, this->getLabelsOfState(state), maxWidthLabel);
                                    outStream << "}";
                                }

                                outStream << this->additionalDotStateInfo(state);

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
            std::string Model<ValueType, RewardModelType>::additionalDotStateInfo(uint64_t state) const {
                return "";
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
            bool Model<ValueType, RewardModelType>::isSinkState(uint64_t state) const {
                for (auto const& entry : this->getTransitionMatrix().getRowGroup(state)) {
                    if (entry.getColumn() != state) { return false; }
                    if (!storm::utility::isOne(entry.getValue())) { return false; }
                }
                return true;
            }

            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::isSparseModel() const {
                return true;
            }
            
            template<typename ValueType, typename RewardModelType>
            bool Model<ValueType, RewardModelType>::supportsParameters() const {
                return std::is_same<ValueType, storm::RationalFunction>::value;
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

            std::set<storm::RationalFunctionVariable> getRateParameters(Model<storm::RationalFunction> const& model) {
                if (model.isOfType(storm::models::ModelType::Ctmc)) {
                    auto const& ctmc = model.template as<storm::models::sparse::Ctmc<storm::RationalFunction>>();
                    return storm::utility::vector::getVariables(ctmc->getExitRateVector());
                } else if (model.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    auto const& ma = model.template as<storm::models::sparse::MarkovAutomaton<storm::RationalFunction>>();
                    return storm::utility::vector::getVariables(ma->getExitRates());
                } else {
                    return {};
                }
            }

            std::set<storm::RationalFunctionVariable> getAllParameters(Model<storm::RationalFunction> const& model) {
                std::set<storm::RationalFunctionVariable> parameters = getProbabilityParameters(model);
                std::set<storm::RationalFunctionVariable> rewardParameters = getRewardParameters(model);
                parameters.insert(rewardParameters.begin(), rewardParameters.end());
                std::set<storm::RationalFunctionVariable> rateParameters = getRewardParameters(model);
                parameters.insert(rateParameters.begin(), rateParameters.end());
                return parameters;
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
