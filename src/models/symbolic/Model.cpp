#include "src/models/symbolic/Model.h"

#include "src/exceptions/InvalidArgumentException.h"

#include "src/adapters/AddExpressionAdapter.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

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
                               boost::optional<storm::dd::Add<Type>> const& optionalStateRewardVector,
                               boost::optional<storm::dd::Add<Type>> const& optionalTransitionRewardMatrix)
            : ModelBase(modelType), manager(manager), reachableStates(reachableStates), initialStates(initialStates), transitionMatrix(transitionMatrix), rowVariables(rowVariables), rowExpressionAdapter(rowExpressionAdapter), columnVariables(columnVariables), columnExpressionAdapter(columnExpressionAdapter), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), labelToExpressionMap(labelToExpressionMap), stateRewardVector(optionalStateRewardVector), transitionRewardMatrix(optionalTransitionRewardMatrix) {
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
                STORM_LOG_THROW(labelToExpressionMap.find(label) != labelToExpressionMap.end(), storm::exceptions::InvalidArgumentException, "The label " << label << " is invalid for the labeling of the model.");
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
            storm::dd::Add<Type> const& Model<Type>::getTransitionRewardMatrix() const {
                return transitionRewardMatrix.get();
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Add<Type>& Model<Type>::getTransitionRewardMatrix() {
                return transitionRewardMatrix.get();
            }
            
            template<storm::dd::DdType Type>
            boost::optional<storm::dd::Add<Type>> const& Model<Type>::getOptionalTransitionRewardMatrix() const {
                return transitionRewardMatrix;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Add<Type> const& Model<Type>::getStateRewardVector() const {
                return stateRewardVector.get();
            }
            
            template<storm::dd::DdType Type>
            boost::optional<storm::dd::Add<Type>> const& Model<Type>::getOptionalStateRewardVector() const {
                return stateRewardVector;
            }
            
            template<storm::dd::DdType Type>
            bool Model<Type>::hasStateRewards() const {
                return static_cast<bool>(stateRewardVector);
            }
            
            template<storm::dd::DdType Type>
            bool Model<Type>::hasTransitionRewards() const {
                return static_cast<bool>(transitionRewardMatrix);
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
            void Model<Type>::printModelInformationToStream(std::ostream& out) const {
                out << "-------------------------------------------------------------- " << std::endl;
                out << "Model type: \t" << this->getType() << " (symbolic)" << std::endl;
                out << "States: \t" << this->getNumberOfStates() << " (" << reachableStates.getNodeCount() << " nodes)" << std::endl;
                out << "Transitions: \t" << this->getNumberOfTransitions() << " (" << transitionMatrix.getNodeCount() << " nodes)" << std::endl;
                
                uint_fast64_t rowVariableCount = 0;
                for (auto const& metaVariable : this->rowVariables) {
                    rowVariableCount += this->getManager().getMetaVariable(metaVariable).getNumberOfDdVariables();
                }
                uint_fast64_t columnVariableCount = 0;
                for (auto const& metaVariable : this->columnVariables) {
                    columnVariableCount += this->getManager().getMetaVariable(metaVariable).getNumberOfDdVariables();
                }
                
                out << "Variables: \t" << "rows: " << this->rowVariables.size() << " meta variables (" << rowVariableCount << " DD variables)" << ", columns: " << this->columnVariables.size() << " meta variables (" << columnVariableCount << " DD variables)" << std::endl;
                out << "Labels: \t" << this->labelToExpressionMap.size() << std::endl;
                for (auto const& label : labelToExpressionMap) {
                    out << "   * " << label.first << std::endl;
                }
                out << "Size in memory: \t" << (this->getSizeInBytes())/1024 << " kbytes" << std::endl;
                out << "-------------------------------------------------------------- " << std::endl;
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