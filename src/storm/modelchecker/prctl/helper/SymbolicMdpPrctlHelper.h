#ifndef STORM_MODELCHECKER_SYMBOLIC_MDP_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SYMBOLIC_MDP_PRCTL_MODELCHECKER_HELPER_H_

#include "storm/models/symbolic/NondeterministicModel.h"

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/solver/SymbolicMinMaxLinearEquationSolver.h"
#include "storm/solver/SolveGoal.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        // Forward-declare result class.
        class CheckResult;
        
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class SymbolicMdpPrctlHelper {
                public:
                typedef typename storm::models::symbolic::NondeterministicModel<DdType, ValueType>::RewardModelType RewardModelType;
                
                static std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::solver::SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeNextProbabilities(Environment const& env, OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates);
                
                static std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeGloballyProbabilities(Environment const& env, OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeCumulativeRewards(Environment const& env, OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeInstantaneousRewards(Environment const& env, OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::solver::SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SYMBOLIC_MDP_PRCTL_MODELCHECKER_HELPER_H_ */
