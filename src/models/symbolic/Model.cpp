#include "src/models/symbolic/Model.h"

#include <boost/algorithm/string/join.hpp>

#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/InvalidOperationException.h"

#include "src/adapters/AddExpressionAdapter.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

#include "src/models/symbolic/StandardRewardModel.h"

namespace storm {
    namespace models {
        namespace symbolic {
            template<storm::dd::DdType Type>
            Model<Type>::Model(storm::models::ModelType const& modelType,
                               std::shared_ptr<storm::dd::DdManager<Type>> manager,
                               storm::dd::Bdd<Type> reachableStates,
                               storm::dd::Bdd<Type> initialStates,
                               storm::dd::Add<Type> transitionMatrix,
                               std::set<storm::expressions::Variable> const& rowVariables,
                               std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter,
                               std::set<storm::expressions::Variable> const& columnVariables,
                               std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter,
                               std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                               std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                               std::unordered_map<std::string, RewardModelType> const& rewardModels)
            : ModelBase(modelType), manager(manager), reachableStates(reachableStates), initialStates(initialStates), transitionMatrix(transitionMatrix), rowVariables(rowVariables), rowExpressionAdapter(rowExpressionAdapter), columnVariables(columnVariables), columnExpressionAdapter(columnExpressionAdapter), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), labelToExpressionMap(labelToExpressionMap), rewardModels(rewardModels) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType Type>
            uint_fast64_t Model<Type>::getNumberOfStates() const {
                return reachableStates.getNonZeroCount();
            }
            
            template<storm::dd::DdType Type>
            uint_fast64_t Model<Type>::getNumberOfTransitions() const {
                return transitionMatrix.getNonZeroCount();
            }
            
            template<storm::dd::DdType Type>
            storm::dd::DdManager<Type> const& Model<Type>::getManager() const {
                return *manager;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::DdManager<Type>& Model<Type>::getManager() {
                return *manager;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Bdd<Type> const& Model<Type>::getReachableStates() const {
                return reachableStates;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Bdd<Type> const& Model<Type>::getInitialStates() const {
                return initialStates;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Bdd<Type> Model<Type>::getStates(std::string const& label) const {
                STORM_LOG_THROW(labelToExpressionMap.find(label) != labelToExpressionMap.end(), storm::exceptions::IllegalArgumentException, "The label " << label << " is invalid for the labeling of the model.");
                return rowExpressionAdapter->translateExpression(labelToExpressionMap.at(label)).toBdd() && this->reachableStates;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Bdd<Type> Model<Type>::getStates(storm::expressions::Expression const& expression) const {
                return rowExpressionAdapter->translateExpression(expression).toBdd() && this->reachableStates;
            }
            
            template<storm::dd::DdType Type>
            bool Model<Type>::hasLabel(std::string const& label) const {
                return labelToExpressionMap.find(label) != labelToExpressionMap.end();
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Add<Type> const& Model<Type>::getTransitionMatrix() const {
                return transitionMatrix;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Add<Type>& Model<Type>::getTransitionMatrix() {
                return transitionMatrix;
            }
            
            template<storm::dd::DdType Type>
            std::size_t Model<Type>::getSizeInBytes() const {
                return sizeof(*this) + sizeof(DdNode) * (reachableStates.getNodeCount() + initialStates.getNodeCount() + transitionMatrix.getNodeCount());
            }
            
            template<storm::dd::DdType Type>
            std::set<storm::expressions::Variable> const& Model<Type>::getRowVariables() const {
                return rowVariables;
            }
            
            template<storm::dd::DdType Type>
            std::set<storm::expressions::Variable> const& Model<Type>::getColumnVariables() const {
                return columnVariables;
            }
            
            template<storm::dd::DdType Type>
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& Model<Type>::getRowColumnMetaVariablePairs() const {
                return rowColumnMetaVariablePairs;
            }
            
            template<storm::dd::DdType Type>
            void Model<Type>::setTransitionMatrix(storm::dd::Add<Type> const& transitionMatrix) {
                this->transitionMatrix = transitionMatrix;
            }
            
            template<storm::dd::DdType Type>
            std::map<std::string, storm::expressions::Expression> const& Model<Type>::getLabelToExpressionMap() const {
                return labelToExpressionMap;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Add<Type> Model<Type>::getRowColumnIdentity() const {
                storm::dd::Add<Type> result = this->getManager().getAddOne();
                for (auto const& pair : this->getRowColumnMetaVariablePairs()) {
                    result *= this->getManager().getIdentity(pair.first).equals(this->getManager().getIdentity(pair.second));
                    result *= this->getManager().getRange(pair.first).toAdd() * this->getManager().getRange(pair.second).toAdd();
                }
                return result;
            }
            
            template<storm::dd::DdType Type>
            bool Model<Type>::hasRewardModel(std::string const& rewardModelName) const {
                return this->rewardModels.find(rewardModelName) != this->rewardModels.end();
            }
            
            template<storm::dd::DdType Type>
            typename Model<Type>::RewardModelType const& Model<Type>::getRewardModel(std::string const& rewardModelName) const {
                auto it = this->rewardModels.find(rewardModelName);
                if (it == this->rewardModels.end()) {
                    if (rewardModelName.empty()) {
                        if (this->hasUniqueRewardModel()) {
                            return this->getUniqueRewardModel()->second;
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Unable to refer to default reward model, because there is no default model or it is not unique.");
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "The requested reward model '" << rewardModelName << "' does not exist.");
                    }
                }
                return it->second;
            }
            
            template<storm::dd::DdType Type>
            typename std::unordered_map<std::string, typename Model<Type>::RewardModelType>::const_iterator Model<Type>::getUniqueRewardModel() const {
                STORM_LOG_THROW(this->hasUniqueRewardModel(), storm::exceptions::InvalidOperationException, "Cannot retrieve unique reward model, because there is no unique one.");
                return this->rewardModels.begin();
            }
            
            template<storm::dd::DdType Type>
            bool Model<Type>::hasUniqueRewardModel() const {
                return this->rewardModels.size() == 1;
            }
            
            template<storm::dd::DdType Type>
            bool Model<Type>::hasRewardModel() const {
                return !this->rewardModels.empty();
            }
            
            template<storm::dd::DdType Type>
            void Model<Type>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                this->printModelInformationFooterToStream(out);
            }
            
            template<storm::dd::DdType Type>
            void Model<Type>::printModelInformationHeaderToStream(std::ostream& out) const {
                out << "-------------------------------------------------------------- " << std::endl;
                out << "Model type: \t" << this->getType() << " (symbolic)" << std::endl;
                out << "States: \t" << this->getNumberOfStates() << " (" << reachableStates.getNodeCount() << " nodes)" << std::endl;
                out << "Transitions: \t" << this->getNumberOfTransitions() << " (" << transitionMatrix.getNodeCount() << " nodes)" << std::endl;
            }
            
            template<storm::dd::DdType Type>
            void Model<Type>::printModelInformationFooterToStream(std::ostream& out) const {
                this->printRewardModelsInformationToStream(out);
                this->printDdVariableInformationToStream(out);
                out << std::endl;
                out << "Labels: \t" << this->labelToExpressionMap.size() << std::endl;
                for (auto const& label : labelToExpressionMap) {
                    out << "   * " << label.first << std::endl;
                }
                out << "Size in memory: \t" << (this->getSizeInBytes())/1024 << " kbytes" << std::endl;
                out << "-------------------------------------------------------------- " << std::endl;
            }
            
            template<storm::dd::DdType Type>
            void Model<Type>::printRewardModelsInformationToStream(std::ostream& out) const {
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

            template<storm::dd::DdType Type>
            void Model<Type>::printDdVariableInformationToStream(std::ostream& out) const {
                uint_fast64_t rowVariableCount = 0;
                for (auto const& metaVariable : this->rowVariables) {
                    rowVariableCount += this->getManager().getMetaVariable(metaVariable).getNumberOfDdVariables();
                }
                uint_fast64_t columnVariableCount = 0;
                for (auto const& metaVariable : this->columnVariables) {
                    columnVariableCount += this->getManager().getMetaVariable(metaVariable).getNumberOfDdVariables();
                }
                
                out << "Variables: \t" << "rows: " << this->rowVariables.size() << " meta variables (" << rowVariableCount << " DD variables)" << ", columns: " << this->columnVariables.size() << " meta variables (" << columnVariableCount << " DD variables)";
            }
            
            template<storm::dd::DdType Type>
            bool Model<Type>::isSymbolicModel() const {
                return true;
            }
            
            // Explicitly instantiate the template class.
            template class Model<storm::dd::DdType::CUDD>;
        } // namespace symbolic
    } // namespace models
} // namespace storm