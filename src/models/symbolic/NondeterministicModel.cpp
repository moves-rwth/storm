#include "src/models/symbolic/NondeterministicModel.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/models/symbolic/StandardRewardModel.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            template<storm::dd::DdType Type, typename ValueType>
            NondeterministicModel<Type, ValueType>::NondeterministicModel(storm::models::ModelType const& modelType,
                                                                          std::shared_ptr<storm::dd::DdManager<Type>> manager,
                                                                          storm::dd::Bdd<Type> reachableStates,
                                                                          storm::dd::Bdd<Type> initialStates,
                                                                          storm::dd::Bdd<Type> deadlockStates,
                                                                          storm::dd::Add<Type, ValueType> transitionMatrix,
                                                                          std::set<storm::expressions::Variable> const& rowVariables,
                                                                          std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> rowExpressionAdapter,
                                                                          std::set<storm::expressions::Variable> const& columnVariables,
                                                                          std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> columnExpressionAdapter,
                                                                          std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                                                                          std::set<storm::expressions::Variable> const& nondeterminismVariables,
                                                                          std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                                                                          std::unordered_map<std::string, RewardModelType> const& rewardModels)
            : Model<Type>(modelType, manager, reachableStates, initialStates, deadlockStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, columnExpressionAdapter, rowColumnMetaVariablePairs, labelToExpressionMap, rewardModels), nondeterminismVariables(nondeterminismVariables) {
                
                // Prepare the mask of illegal nondeterministic choices.
                illegalMask = transitionMatrix.notZero().existsAbstract(this->getColumnVariables());
                illegalMask = !illegalMask && reachableStates;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            uint_fast64_t NondeterministicModel<Type, ValueType>::getNumberOfChoices() const {
                std::set<storm::expressions::Variable> rowAndNondeterminismVariables;
                std::set_union(this->getNondeterminismVariables().begin(), this->getNondeterminismVariables().end(), this->getRowVariables().begin(), this->getRowVariables().end(), std::inserter(rowAndNondeterminismVariables, rowAndNondeterminismVariables.begin()));
                
                storm::dd::Add<Type, uint_fast64_t> tmp = this->getTransitionMatrix().notZero().existsAbstract(this->getColumnVariables()).template toAdd<uint_fast64_t>().sumAbstract(rowAndNondeterminismVariables);
                return tmp.getValue();
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::set<storm::expressions::Variable> const& NondeterministicModel<Type, ValueType>::getNondeterminismVariables() const {
                return nondeterminismVariables;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Bdd<Type> const& NondeterministicModel<Type, ValueType>::getIllegalMask() const {
                return illegalMask;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            void NondeterministicModel<Type, ValueType>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                out << "Choices: \t" << this->getNumberOfChoices() << std::endl;
                this->printModelInformationFooterToStream(out);
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            void NondeterministicModel<Type, ValueType>::printDdVariableInformationToStream(std::ostream& out) const {
                uint_fast64_t nondeterminismVariableCount = 0;
                for (auto const& metaVariable : this->getNondeterminismVariables()) {
                    nondeterminismVariableCount += this->getManager().getMetaVariable(metaVariable).getNumberOfDdVariables();
                }
                Model<Type>::printDdVariableInformationToStream(out);
                out << ", nondeterminism: " << this->getNondeterminismVariables().size() << " meta variables (" << nondeterminismVariableCount << " DD variables)";
            }
            
            // Explicitly instantiate the template class.
            template class NondeterministicModel<storm::dd::DdType::CUDD, double>;
            template class NondeterministicModel<storm::dd::DdType::Sylvan, double>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm