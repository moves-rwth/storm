#ifndef STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_

#include "storm/models/symbolic/Model.h"

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/solver/SymbolicLinearEquationSolver.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class SymbolicDtmcPrctlHelper {
            public:
                typedef typename storm::models::symbolic::Model<DdType, ValueType>::RewardModelType RewardModelType;
                
                static storm::dd::Add<DdType, ValueType> computeBoundedUntilProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType, ValueType> computeNextProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates);
                
                static storm::dd::Add<DdType, ValueType> computeUntilProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType, ValueType> computeGloballyProbabilities(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType, ValueType> computeCumulativeRewards(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType, ValueType> computeInstantaneousRewards(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType, ValueType> computeReachabilityRewards(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_ */
