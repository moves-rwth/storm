#include "SymbolicToSparseTransformer.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace transformer {

        template<storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> transformSymbolicToSparseModel(std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> const& symbolicModel) {
            switch (symbolicModel->getType()) {
                case storm::models::ModelType::Dtmc:
                    return SymbolicDtmcToSparseDtmcTransformer<Type, ValueType>().translate(*symbolicModel->template as<storm::models::symbolic::Dtmc<Type, ValueType>>());
                case storm::models::ModelType::Mdp:
                    return SymbolicMdpToSparseMdpTransformer<Type, ValueType>::translate(*symbolicModel->template as<storm::models::symbolic::Mdp<Type, ValueType>>());
                case storm::models::ModelType::Ctmc:
                    return SymbolicCtmcToSparseCtmcTransformer<Type, ValueType>::translate(*symbolicModel->template as<storm::models::symbolic::Ctmc<Type, ValueType>>());
                default:
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Transformation of symbolic " << symbolicModel->getType() << " to sparse model is not implemented.");
            }
            return nullptr;
        }
        

        template<storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> SymbolicDtmcToSparseDtmcTransformer<Type, ValueType>::translate(storm::models::symbolic::Dtmc<Type, ValueType> const& symbolicDtmc) {
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
            for(auto const& label : symbolicDtmc.getLabels()) {
                labelling.addLabel(label, symbolicDtmc.getStates(label).toVector(this->odd));
            }
            return std::make_shared<storm::models::sparse::Dtmc<ValueType>>(transitionMatrix, labelling, rewardModels);
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Odd const& SymbolicDtmcToSparseDtmcTransformer<Type, ValueType>::getOdd() const {
            return this->odd;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::sparse::Mdp<ValueType>> SymbolicMdpToSparseMdpTransformer<Type, ValueType>::translate(storm::models::symbolic::Mdp<Type, ValueType> const& symbolicMdp) {
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
            for(auto const& label : symbolicMdp.getLabels()) {
                labelling.addLabel(label, symbolicMdp.getStates(label).toVector(odd));
            }
            return std::make_shared<storm::models::sparse::Mdp<ValueType>>(transitionMatrix, labelling, rewardModels);
        }

        template<storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> SymbolicCtmcToSparseCtmcTransformer<Type, ValueType>::translate(
                storm::models::symbolic::Ctmc<Type, ValueType> const& symbolicCtmc) {
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
            for(auto const& label : symbolicCtmc.getLabels()) {
                labelling.addLabel(label, symbolicCtmc.getStates(label).toVector(odd));
            }
            return std::make_shared<storm::models::sparse::Ctmc<ValueType>>(transitionMatrix, labelling, rewardModels);
        }

        template std::shared_ptr<storm::models::sparse::Model<double>> transformSymbolicToSparseModel<storm::dd::DdType::CUDD, double>(std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> const& symbolicModel);
        template std::shared_ptr<storm::models::sparse::Model<double>> transformSymbolicToSparseModel<storm::dd::DdType::Sylvan, double>(std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>> const& symbolicModel);
        template std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> transformSymbolicToSparseModel<storm::dd::DdType::Sylvan, storm::RationalNumber>(std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, storm::RationalNumber>> const& symbolicModel);
        template std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> transformSymbolicToSparseModel<storm::dd::DdType::Sylvan, storm::RationalFunction>(std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, storm::RationalFunction>> const& symbolicModel);
        
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
