#ifndef STORM_MODELCHECKER_HYBRID_MDP_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_HYBRID_MDP_PRCTL_MODELCHECKER_HELPER_H_

#include "src/models/symbolic/NondeterministicModel.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        // Forward-declare result class.
        class CheckResult;
        
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class HybridMdpPrctlHelper {
            public:
                typedef typename storm::models::symbolic::NondeterministicModel<DdType>::RewardModelType RewardModelType;
                
                static std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeNextProbabilities(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates);
                
                static std::unique_ptr<CheckResult> computeUntilProbabilities(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeCumulativeRewards(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeInstantaneousRewards(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeReachabilityRewards(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_HYBRID_MDP_PRCTL_MODELCHECKER_HELPER_H_ */
