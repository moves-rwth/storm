#ifndef STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_

#include "src/models/symbolic/Model.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class SymbolicDtmcPrctlHelper {
            public:
                static storm::dd::Add<DdType> computeBoundedUntilProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType> computeNextProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates);
                
                static storm::dd::Add<DdType> computeUntilProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType> computeCumulativeRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, uint_fast64_t stepBound, storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType> computeInstantaneousRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, uint_fast64_t stepBound, storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
                
                static storm::dd::Add<DdType> computeReachabilityRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, boost::optional<storm::dd::Add<DdType>> const& stateRewardVector, boost::optional<storm::dd::Add<DdType>> const& transitionRewardMatrix, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::utility::solver::SymbolicLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_ */