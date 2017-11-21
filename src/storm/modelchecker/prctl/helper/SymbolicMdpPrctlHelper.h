#ifndef STORM_MODELCHECKER_SYMBOLIC_MDP_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SYMBOLIC_MDP_PRCTL_MODELCHECKER_HELPER_H_

#include "storm/models/symbolic/NondeterministicModel.h"

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/solver/SymbolicMinMaxLinearEquationSolver.h"
#include "storm/solver/SolveGoal.h"

namespace storm {
    namespace modelchecker {
        // Forward-declare result class.
        class CheckResult;
        
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class SymbolicMdpPrctlHelper {
                public:
                typedef typename storm::models::symbolic::NondeterministicModel<DdType, ValueType>::RewardModelType RewardModelType;
                
                static std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeNextProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates);
                
                static std::unique_ptr<CheckResult> computeUntilProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory, boost::optional<storm::dd::Add<DdType, ValueType>> const& startValues = boost::none);

                static std::unique_ptr<CheckResult> computeUntilProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& maybeStates, storm::dd::Bdd<DdType> const& statesWithProbability1, storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory, boost::optional<storm::dd::Add<DdType, ValueType>> const& startValues = boost::none);

                static std::unique_ptr<CheckResult> computeGloballyProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeCumulativeRewards(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeInstantaneousRewards(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeReachabilityRewards(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory, boost::optional<storm::dd::Add<DdType, ValueType>> const& startValues = boost::none);

                static std::unique_ptr<CheckResult> computeReachabilityRewards(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& transitionMatrixBdd, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& maybeStates, storm::dd::Bdd<DdType> const& targetStates, storm::dd::Bdd<DdType> const& infinityStates, storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory, boost::optional<storm::dd::Add<DdType, ValueType>> const& startValues = boost::none);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SYMBOLIC_MDP_PRCTL_MODELCHECKER_HELPER_H_ */
