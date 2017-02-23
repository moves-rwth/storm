#include <storm/exceptions/InvalidArgumentException.h>
#include <storm/exceptions/UnexpectedException.h>
#include "storm/transformer/SparseParametricModelSimplifier.h"


#include "storm/adapters/CarlAdapter.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/stateelimination/PrioritizedStateEliminator.h"
#include "storm/storage/FlexibleSparseMatrix.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace transformer {

        template<typename SparseModelType>
        SparseParametricModelSimplifier<SparseModelType>::SparseParametricModelSimplifier(SparseModelType const& model) : originalModel(model) {
            // intentionally left empty
        }
        
        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplify(storm::logic::Formula const& formula) {
            // Make sure that there is no old result from a previous call
            simplifiedModel = nullptr;
            simplifiedFormula = nullptr;
            if (formula.isProbabilityOperatorFormula()) {
                storm::logic::ProbabilityOperatorFormula const& probOpForm = formula.asProbabilityOperatorFormula();
                if (probOpForm.getSubformula().isUntilFormula()) {
                    return simplifyForUntilProbabilities(probOpForm);
                } else if (probOpForm.getSubformula().isReachabilityProbabilityFormula()) {
                    return simplifyForReachabilityProbabilities(probOpForm);
                } else if (probOpForm.getSubformula().isBoundedUntilFormula()) {
                    return simplifyForBoundedUntilProbabilities(probOpForm);
                }
            } else if (formula.isRewardOperatorFormula()) {
                storm::logic::RewardOperatorFormula rewOpForm = formula.asRewardOperatorFormula();
                STORM_LOG_THROW((rewOpForm.hasRewardModelName() && originalModel.hasRewardModel(rewOpForm.getRewardModelName())) || (!rewOpForm.hasRewardModelName() && originalModel.hasUniqueRewardModel()), storm::exceptions::InvalidPropertyException, "The reward model specified by formula " << formula << " is not available in the given model.");
                if (rewOpForm.getSubformula().isReachabilityRewardFormula()) {
                    return simplifyForReachabilityRewards(rewOpForm);
                } else if (rewOpForm.getSubformula().isCumulativeRewardFormula()) {
                    return simplifyForCumulativeRewards(rewOpForm);
                }
            }
            // reaching this point means that the provided formula is not supported. Thus, no simplification is possible.
            STORM_LOG_DEBUG("Simplification not possible because the formula is not suppoerted. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
        std::shared_ptr<SparseModelType> SparseParametricModelSimplifier<SparseModelType>::getSimplifiedModel() const {
            STORM_LOG_THROW(simplifiedModel, storm::exceptions::InvalidStateException, "Tried to get the simplified model but simplification was not invoked before.");
            return simplifiedModel;
        }
        
        template<typename SparseModelType>
        std::shared_ptr<storm::logic::Formula const> SparseParametricModelSimplifier<SparseModelType>::getSimplifiedFormula() const {
            STORM_LOG_THROW(simplifiedFormula, storm::exceptions::InvalidStateException, "Tried to get the simplified formula but simplification was not invoked before.");
            return simplifiedFormula;
        }

        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
            // If this method was not overriden by any subclass, simplification is not possible
            STORM_LOG_DEBUG("Simplification not possible because the formula is not suppoerted. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForReachabilityProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
            // transform to until formula
            auto untilFormula = std::make_shared<storm::logic::UntilFormula const>(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asEventuallyFormula().getSubformula().asSharedPointer());
            return simplifyForUntilProbabilities(storm::logic::ProbabilityOperatorFormula(untilFormula, formula.getOperatorInformation()));
        }

        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForBoundedUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
            // If this method was not overriden by any subclass, simplification is not possible
            STORM_LOG_DEBUG("Simplification not possible because the formula is not suppoerted. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForReachabilityRewards(storm::logic::RewardOperatorFormula const& formula) {
            // If this method was not overriden by any subclass, simplification is not possible
            STORM_LOG_DEBUG("Simplification not possible because the formula is not suppoerted. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForCumulativeRewards(storm::logic::RewardOperatorFormula const& formula) {
            // If this method was not overriden by any subclass, simplification is not possible
            STORM_LOG_DEBUG("Simplification not possible because the formula is not suppoerted. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
         std::shared_ptr<SparseModelType> SparseParametricModelSimplifier<SparseModelType>::eliminateConstantDeterministicStates(SparseModelType const& model, boost::optional<std::string> const& rewardModelName) {
            storm::storage::SparseMatrix<typename SparseModelType::ValueType> const& sparseMatrix = model.getTransitionMatrix();
            
            // Get the states without any label
            storm::storage::BitVector considerForElimination(model.getNumberOfStates(), false);
            for(auto const& label : model.getStateLabeling().getLabels()) {
                considerForElimination |= model.getStateLabeling().getStates(label);
            }
            considerForElimination.complement();
                            
            // get the action-based reward values
            std::vector<typename SparseModelType::ValueType> actionRewards;
            if(rewardModelName) {
                actionRewards = model.getRewardModel(*rewardModelName).getTotalRewardVector(sparseMatrix);
            }
            
            // Find the states that are to be eliminated
            std::vector<uint_fast64_t> statesToEliminate;
            storm::storage::BitVector keptStates(sparseMatrix.getRowGroupCount(), true);
            storm::storage::BitVector keptRows(sparseMatrix.getRowCount(), true);
            for (auto state : considerForElimination) {
                if (sparseMatrix.getRowGroupSize(state) == 1 && (!rewardModelName.is_initialized() || storm::utility::isConstant(actionRewards[sparseMatrix.getRowGroupIndices()[state]]))) {
                    bool hasOnlyConstEntries = true;
                    for (auto const& entry : sparseMatrix.getRowGroup(state)) {
                        if(!storm::utility::isConstant(entry.getValue())) {
                            hasOnlyConstEntries = false;
                            break;
                        }
                    }
                    if (hasOnlyConstEntries) {
                        statesToEliminate.push_back(state);
                        keptStates.set(state, false);
                        keptRows.set(sparseMatrix.getRowGroupIndices()[state], false);
                    }
                }
            }
            
            // only keep the relevant reward values and translate them to state-based rewards
            std::vector<typename SparseModelType::ValueType> rewardsOfEliminatedStates;
            if(rewardModelName) {
                rewardsOfEliminatedStates.reserve(model.getNumberOfStates());
                for(uint_fast64_t state = 0; state < model.getNumberOfStates(); ++state) {
                    if(keptStates.get(state)) {
                        // The current state will not be eliminated. Hence we do not need to consider its reward during elimination
                        rewardsOfEliminatedStates.push_back(storm::utility::zero<typename SparseModelType::ValueType>());
                    } else {
                        // We need to keep track of the reward of this state during elimination
                        rewardsOfEliminatedStates.push_back(std::move(actionRewards[sparseMatrix.getRowGroupIndices()[state]]));
                    }
                }
            } else {
                rewardsOfEliminatedStates.resize(model.getNumberOfStates(), storm::utility::zero<typename SparseModelType::ValueType>());
            }
            
            // invoke elimination and obtain resulting transition matrix
            storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrix(sparseMatrix);
            storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitions(sparseMatrix.transpose(true), true);
            storm::solver::stateelimination::PrioritizedStateEliminator<typename SparseModelType::ValueType> stateEliminator(flexibleMatrix, flexibleBackwardTransitions, statesToEliminate, rewardsOfEliminatedStates);
            stateEliminator.eliminateAll();
            storm::storage::SparseMatrix<typename  SparseModelType::ValueType> newTransitionMatrix = flexibleMatrix.createSparseMatrix(keptRows, keptStates);
            
            // obtain the reward model for the resulting system
            std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModels;
            if(rewardModelName) {
                actionRewards = storm::utility::vector::filterVector(actionRewards, keptRows);
                rewardsOfEliminatedStates = storm::utility::vector::filterVector(rewardsOfEliminatedStates, keptStates);
                storm::utility::vector::addVectorToGroupedVector(actionRewards, rewardsOfEliminatedStates, newTransitionMatrix.getRowGroupIndices());
                rewardModels.insert(std::make_pair(*rewardModelName, typename SparseModelType::RewardModelType(boost::none, std::move(actionRewards))));
            }
                
            return std::make_shared<SparseModelType>(std::move(newTransitionMatrix), model.getStateLabeling().getSubLabeling(keptStates), std::move(rewardModels));
        }


        template class SparseParametricModelSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>;
        template class SparseParametricModelSimplifier<storm::models::sparse::Mdp<storm::RationalFunction>>;
    }
}