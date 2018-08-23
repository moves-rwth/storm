#include "SymbolicToSparseTransformer.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/logic/AtomicExpressionFormula.h"
#include "storm/logic/AtomicLabelFormula.h"

namespace storm {
    namespace transformer {

        struct LabelInformation {
            LabelInformation(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
                for (auto const& formula : formulas) {
                    std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> atomicLabelFormulas = formula->getAtomicLabelFormulas();
                    for (auto const& labelFormula : atomicLabelFormulas) {
                        atomicLabels.insert(labelFormula->getLabel());
                    }
                    
                    std::vector<std::shared_ptr<storm::logic::AtomicExpressionFormula const>> atomicExpressionFormulas = formula->getAtomicExpressionFormulas();
                    for (auto const& expressionFormula : atomicExpressionFormulas) {
                        std::stringstream ss;
                        ss << expressionFormula->getExpression();
                        expressionLabels[ss.str()] = expressionFormula->getExpression();
                    }
                }
            }
            
            std::set<std::string> atomicLabels;
            std::map<std::string, storm::expressions::Expression> expressionLabels;
        };
        
        template<storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> SymbolicDtmcToSparseDtmcTransformer<Type, ValueType>::translate(storm::models::symbolic::Dtmc<Type, ValueType> const& symbolicDtmc, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
            
            this->odd = symbolicDtmc.getReachableStates().createOdd();
            storm::storage::SparseMatrix<ValueType> transitionMatrix = symbolicDtmc.getTransitionMatrix().toMatrix(this->odd, this->odd);
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>> rewardModels;
            for (auto const& rewardModelNameAndModel : symbolicDtmc.getRewardModels()) {
                boost::optional<std::vector<ValueType>> stateRewards;
                boost::optional<std::vector<ValueType>> stateActionRewards;
                boost::optional<storm::storage::SparseMatrix<ValueType>> transitionRewards;
                if (rewardModelNameAndModel.second.hasStateRewards()) {
                    stateRewards = rewardModelNameAndModel.second.getStateRewardVector().toVector(this->odd);
                }
                if (rewardModelNameAndModel.second.hasStateActionRewards()) {
                    stateActionRewards = rewardModelNameAndModel.second.getStateActionRewardVector().toVector(this->odd);
                }
                if (rewardModelNameAndModel.second.hasTransitionRewards()) {
                    transitionRewards = rewardModelNameAndModel.second.getTransitionRewardMatrix().toMatrix(this->odd, this->odd);
                }
                rewardModels.emplace(rewardModelNameAndModel.first,storm::models::sparse::StandardRewardModel<ValueType>(stateRewards, stateActionRewards, transitionRewards));
            }
            storm::models::sparse::StateLabeling labelling(transitionMatrix.getRowGroupCount());
            
            labelling.addLabel("init", symbolicDtmc.getInitialStates().toVector(this->odd));
            labelling.addLabel("deadlock", symbolicDtmc.getDeadlockStates().toVector(this->odd));
            if (formulas.empty()) {
                for (auto const& label : symbolicDtmc.getLabels()) {
                    labelling.addLabel(label, symbolicDtmc.getStates(label).toVector(this->odd));
                }
            } else {
                LabelInformation labelInfo(formulas);
                for (auto const& label : labelInfo.atomicLabels) {
                    labelling.addLabel(label, symbolicDtmc.getStates(label).toVector(this->odd));
                }
                for (auto const& expressionLabel : labelInfo.expressionLabels) {
                    labelling.addLabel(expressionLabel.first, symbolicDtmc.getStates(expressionLabel.second).toVector(this->odd));
                }
            }
            return std::make_shared<storm::models::sparse::Dtmc<ValueType>>(transitionMatrix, labelling, rewardModels);
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Odd const& SymbolicDtmcToSparseDtmcTransformer<Type, ValueType>::getOdd() const {
            return this->odd;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::sparse::Mdp<ValueType>> SymbolicMdpToSparseMdpTransformer<Type, ValueType>::translate(storm::models::symbolic::Mdp<Type, ValueType> const& symbolicMdp, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
            storm::dd::Odd odd = symbolicMdp.getReachableStates().createOdd();
            storm::storage::SparseMatrix<ValueType> transitionMatrix = symbolicMdp.getTransitionMatrix().toMatrix(symbolicMdp.getNondeterminismVariables(), odd, odd);
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>> rewardModels;
            for (auto const& rewardModelNameAndModel : symbolicMdp.getRewardModels()) {
                boost::optional<std::vector<ValueType>> stateRewards;
                boost::optional<std::vector<ValueType>> stateActionRewards;
                boost::optional<storm::storage::SparseMatrix<ValueType>> transitionRewards;
                if (rewardModelNameAndModel.second.hasStateRewards()) {
                    stateRewards = rewardModelNameAndModel.second.getStateRewardVector().toVector(odd);
                }
                // Note: .getStateActionRewardVector().toVector(odd); does not work as it needs to have information regarding the nondeterminism
                // One could use transitionMatrix().toMatrixVector instead.
                STORM_LOG_THROW(!rewardModelNameAndModel.second.hasStateActionRewards(), storm::exceptions::NotImplementedException, "Translation of symbolic to explicit state-action rewards is not yet supported.");
                STORM_LOG_THROW(!rewardModelNameAndModel.second.hasTransitionRewards(), storm::exceptions::NotImplementedException, "Translation of symbolic to explicit transition rewards is not yet supported.");
                rewardModels.emplace(rewardModelNameAndModel.first,storm::models::sparse::StandardRewardModel<ValueType>(stateRewards, stateActionRewards, transitionRewards));
            }
            storm::models::sparse::StateLabeling labelling(transitionMatrix.getRowGroupCount());

            labelling.addLabel("init", symbolicMdp.getInitialStates().toVector(odd));
            labelling.addLabel("deadlock", symbolicMdp.getDeadlockStates().toVector(odd));
            if (formulas.empty()) {
                for (auto const& label : symbolicMdp.getLabels()) {
                    labelling.addLabel(label, symbolicMdp.getStates(label).toVector(odd));
                }
            } else {
                LabelInformation labelInfo(formulas);
                for (auto const& label : labelInfo.atomicLabels) {
                    labelling.addLabel(label, symbolicMdp.getStates(label).toVector(odd));
                }
                for (auto const& expressionLabel : labelInfo.expressionLabels) {
                    labelling.addLabel(expressionLabel.first, symbolicMdp.getStates(expressionLabel.second).toVector(odd));
                }
            }
            
            return std::make_shared<storm::models::sparse::Mdp<ValueType>>(transitionMatrix, labelling, rewardModels);
        }

        template<storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> SymbolicCtmcToSparseCtmcTransformer<Type, ValueType>::translate(storm::models::symbolic::Ctmc<Type, ValueType> const& symbolicCtmc, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
            storm::dd::Odd odd = symbolicCtmc.getReachableStates().createOdd();
            storm::storage::SparseMatrix<ValueType> transitionMatrix = symbolicCtmc.getTransitionMatrix().toMatrix(odd, odd);
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>> rewardModels;
            for (auto const& rewardModelNameAndModel : symbolicCtmc.getRewardModels()) {
                boost::optional<std::vector<ValueType>> stateRewards;
                boost::optional<std::vector<ValueType>> stateActionRewards;
                boost::optional<storm::storage::SparseMatrix<ValueType>> transitionRewards;
                if (rewardModelNameAndModel.second.hasStateRewards()) {
                    stateRewards = rewardModelNameAndModel.second.getStateRewardVector().toVector(odd);
                }
                if (rewardModelNameAndModel.second.hasStateActionRewards()) {
                    stateActionRewards = rewardModelNameAndModel.second.getStateActionRewardVector().toVector(odd);
                }
                if (rewardModelNameAndModel.second.hasTransitionRewards()) {
                    transitionRewards = rewardModelNameAndModel.second.getTransitionRewardMatrix().toMatrix(odd, odd);
                }
                rewardModels.emplace(rewardModelNameAndModel.first,storm::models::sparse::StandardRewardModel<ValueType>(stateRewards, stateActionRewards, transitionRewards));
            }
            storm::models::sparse::StateLabeling labelling(transitionMatrix.getRowGroupCount());

            labelling.addLabel("init", symbolicCtmc.getInitialStates().toVector(odd));
            labelling.addLabel("deadlock", symbolicCtmc.getDeadlockStates().toVector(odd));
            if (formulas.empty()) {
                for (auto const& label : symbolicCtmc.getLabels()) {
                    labelling.addLabel(label, symbolicCtmc.getStates(label).toVector(odd));
                }
            } else {
                LabelInformation labelInfo(formulas);
                for (auto const& label : labelInfo.atomicLabels) {
                    labelling.addLabel(label, symbolicCtmc.getStates(label).toVector(odd));
                }
                for (auto const& expressionLabel : labelInfo.expressionLabels) {
                    labelling.addLabel(expressionLabel.first, symbolicCtmc.getStates(expressionLabel.second).toVector(odd));
                }
            }
            
            return std::make_shared<storm::models::sparse::Ctmc<ValueType>>(transitionMatrix, labelling, rewardModels);
        }

        template class SymbolicDtmcToSparseDtmcTransformer<storm::dd::DdType::CUDD, double>;
        template class SymbolicDtmcToSparseDtmcTransformer<storm::dd::DdType::Sylvan, double>;
        template class SymbolicDtmcToSparseDtmcTransformer<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        template class SymbolicDtmcToSparseDtmcTransformer<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        
        template class SymbolicMdpToSparseMdpTransformer<storm::dd::DdType::CUDD, double>;
        template class SymbolicMdpToSparseMdpTransformer<storm::dd::DdType::Sylvan, double>;
        template class SymbolicMdpToSparseMdpTransformer<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        template class SymbolicMdpToSparseMdpTransformer<storm::dd::DdType::Sylvan, storm::RationalFunction>;

        template class SymbolicCtmcToSparseCtmcTransformer<storm::dd::DdType::CUDD, double>;
        template class SymbolicCtmcToSparseCtmcTransformer<storm::dd::DdType::Sylvan, double>;
        template class SymbolicCtmcToSparseCtmcTransformer<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        template class SymbolicCtmcToSparseCtmcTransformer<storm::dd::DdType::Sylvan, storm::RationalFunction>;

    }
}
