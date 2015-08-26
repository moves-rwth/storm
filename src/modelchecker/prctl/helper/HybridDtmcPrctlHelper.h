#ifndef STORM_MODELCHECKER_HYBRID_DTMC_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_HYBRID_DTMC_PRCTL_MODELCHECKER_HELPER_H_

#include "src/models/symbolic/Model.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        // Forward-declare result class.
        class CheckResult;
        
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class HybridDtmcPrctlHelper {
            public:
                typedef typename storm::models::symbolic::Model<DdType>::RewardModelType RewardModelType;
                
                static std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeNextProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates);
                
                static std::unique_ptr<CheckResult> computeUntilProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeCumulativeRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeReachabilityRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_HYBRID_DTMC_PRCTL_MODELCHECKER_HELPER_H_ */
