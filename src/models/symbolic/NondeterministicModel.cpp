#include "src/models/symbolic/NondeterministicModel.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

#include "src/models/symbolic/StandardRewardModel.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            template<storm::dd::DdType Type>
            NondeterministicModel<Type>::NondeterministicModel(storm::models::ModelType const& modelType,
                                                               std::shared_ptr<storm::dd::DdManager<Type>> manager,
                                                               storm::dd::Bdd<Type> reachableStates,
                                                               storm::dd::Bdd<Type> initialStates,
                                                               storm::dd::Add<Type> transitionMatrix,
                                                               std::set<storm::expressions::Variable> const& rowVariables,
                                                               std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter,
                                                               std::set<storm::expressions::Variable> const& columnVariables,
                                                               std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter,
                                                               std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                                                               std::set<storm::expressions::Variable> const& nondeterminismVariables,
                                                               std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                                                               std::unordered_map<std::string, RewardModelType> const& rewardModels)
            : Model<Type>(modelType, manager, reachableStates, initialStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, columnExpressionAdapter, rowColumnMetaVariablePairs, labelToExpressionMap, rewardModels), nondeterminismVariables(nondeterminismVariables) {
                
                // Prepare the mask of illegal nondeterministic choices.
                illegalMask = !(transitionMatrix.notZero().existsAbstract(this->getColumnVariables())) && reachableStates;
            }
            
            template<storm::dd::DdType Type>
            uint_fast64_t NondeterministicModel<Type>::getNumberOfChoices() const {
                std::set<storm::expressions::Variable> rowAndNondeterminsmVariables;
                std::set_union(this->getNondeterminismVariables().begin(), this->getNondeterminismVariables().end(), this->getRowVariables().begin(), this->getRowVariables().end(), std::inserter(rowAndNondeterminsmVariables, rowAndNondeterminsmVariables.begin()));
                
                storm::dd::Add<Type> tmp = this->getTransitionMatrix().notZero().existsAbstract(this->getColumnVariables()).toAdd().sumAbstract(rowAndNondeterminsmVariables);
                return static_cast<uint_fast64_t>(tmp.getValue());
            }
            
            template<storm::dd::DdType Type>
            std::set<storm::expressions::Variable> const& NondeterministicModel<Type>::getNondeterminismVariables() const {
                return nondeterminismVariables;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Bdd<Type> const& NondeterministicModel<Type>::getIllegalMask() const {
                return illegalMask;
            }
            
            template<storm::dd::DdType Type>
            storm::dd::Bdd<Type> NondeterministicModel<Type>::getIllegalSuccessorMask() const {
                storm::dd::Bdd<Type> transitionMatrixBdd = this->getTransitionMatrix().notZero();
                return !transitionMatrixBdd && transitionMatrixBdd.existsAbstract(this->getColumnVariables());
            }
            
            template<storm::dd::DdType Type>
            void NondeterministicModel<Type>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                out << "Choices: \t" << this->getNumberOfChoices() << std::endl;
                this->printModelInformationFooterToStream(out);
            }
            
            template<storm::dd::DdType Type>
            void NondeterministicModel<Type>::printDdVariableInformationToStream(std::ostream& out) const {
                uint_fast64_t nondeterminismVariableCount = 0;
                for (auto const& metaVariable : this->getNondeterminismVariables()) {
                    nondeterminismVariableCount += this->getManager().getMetaVariable(metaVariable).getNumberOfDdVariables();
                }
                Model<Type>::printDdVariableInformationToStream(out);
                out << ", nondeterminism: " << this->getNondeterminismVariables().size() << " meta variables (" << nondeterminismVariableCount << " DD variables)";
            }
                        
            // Explicitly instantiate the template class.
            template class NondeterministicModel<storm::dd::DdType::CUDD>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm