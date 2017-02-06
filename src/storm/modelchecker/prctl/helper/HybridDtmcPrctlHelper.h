#ifndef STORM_MODELCHECKER_HYBRID_DTMC_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_HYBRID_DTMC_PRCTL_MODELCHECKER_HELPER_H_

#include "storm/models/symbolic/Model.h"

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/solver/LinearEquationSolver.h"

namespace storm {
    namespace modelchecker {
        // Forward-declare result class.
        class CheckResult;
        
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class HybridDtmcPrctlHelper {
            public:
                typedef typename storm::models::symbolic::Model<DdType, ValueType>::RewardModelType RewardModelType;
                
                static std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeNextProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates);
                
                static std::unique_ptr<CheckResult> computeUntilProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeGloballyProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeCumulativeRewards(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeReachabilityRewards(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);

                static std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& targetStates, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);

                static std::unique_ptr<CheckResult> computeLongRunAverageRewards(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory);

            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_HYBRID_DTMC_PRCTL_MODELCHECKER_HELPER_H_ */
