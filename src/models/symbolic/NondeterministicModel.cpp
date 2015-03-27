#include "src/models/symbolic/NondeterministicModel.h"

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
                                                               boost::optional<storm::dd::Add<Type>> const& optionalStateRewardVector,
                                                               boost::optional<storm::dd::Add<Type>> const& optionalTransitionRewardMatrix)
            : Model<Type>(modelType, manager, reachableStates, initialStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, columnExpressionAdapter, rowColumnMetaVariablePairs, labelToExpressionMap, optionalStateRewardVector, optionalTransitionRewardMatrix), nondeterminismVariables(nondeterminismVariables) {
                
                // Prepare the mask of illegal nondeterministic choices.
                illegalMask = transitionMatrix.notZero().existsAbstract(this->getColumnVariables());
                illegalMask = !illegalMask && reachableStates;
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
            void NondeterministicModel<Type>::printModelInformationToStream(std::ostream& out) const {
                out << "-------------------------------------------------------------- " << std::endl;
                out << "Model type: \t" << this->getType() << " (symbolic)" << std::endl;
                out << "States: \t" << this->getNumberOfStates() << " (" << this->getReachableStates().getNodeCount() << " nodes)" << std::endl;
                out << "Transitions: \t" << this->getNumberOfTransitions() << " (" << this->getTransitionMatrix().getNodeCount() << " nodes)" << std::endl;
                out << "Choices: \t" << this->getNumberOfChoices() << std::endl;
                out << "Variables: \t" << "rows: " << this->getRowVariables().size() << ", columns: " << this->getColumnVariables().size() << ", nondeterminism: " << this->getNondeterminismVariables().size() << std::endl;
                out << "Labels: \t" << this->getLabelToExpressionMap().size() << std::endl;
                for (auto const& label : this->getLabelToExpressionMap()) {
                    out << "   * " << label.first << std::endl;
                }
                out << "Size in memory: \t" << (this->getSizeInBytes())/1024 << " kbytes" << std::endl;
                out << "-------------------------------------------------------------- " << std::endl;
            }
            
            // Explicitly instantiate the template class.
            template class NondeterministicModel<storm::dd::DdType::CUDD>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm