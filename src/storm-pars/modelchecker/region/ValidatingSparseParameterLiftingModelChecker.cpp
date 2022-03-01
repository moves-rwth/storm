#include "storm-pars/modelchecker/region/ValidatingSparseParameterLiftingModelChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

namespace storm {
    namespace modelchecker {
       
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        ValidatingSparseParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::ValidatingSparseParameterLiftingModelChecker() : numOfWrongRegions(0) {
            // Intentionally left empty
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        ValidatingSparseParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::~ValidatingSparseParameterLiftingModelChecker() {
            if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                STORM_PRINT_AND_LOG("Validating Parameter Lifting Model Checker detected " << numOfWrongRegions << " regions where the imprecise method was wrong.\n");
            }
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        bool ValidatingSparseParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const {
            return getImpreciseChecker().canHandle(parametricModel, checkTask) && getPreciseChecker().canHandle(parametricModel, checkTask);
        }
 
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        RegionResult ValidatingSparseParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::analyzeRegion(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, RegionResultHypothesis const& hypothesis, RegionResult const& initialResult, bool sampleVerticesOfRegion, std::shared_ptr<storm::analysis::LocalMonotonicityResult<typename RegionModelChecker<typename SparseModelType::ValueType>::VariableType>> localMonotonicityResult) {


            RegionResult currentResult = getImpreciseChecker().analyzeRegion(env, region, hypothesis, initialResult, false);
            
            if (currentResult == RegionResult::AllSat || currentResult == RegionResult::AllViolated) {
                applyHintsToPreciseChecker();
                
                storm::solver::OptimizationDirection parameterOptDir = getPreciseChecker().getCurrentCheckTask().getOptimizationDirection();
                if (currentResult == RegionResult::AllViolated) {
                    parameterOptDir = storm::solver::invert(parameterOptDir);
                }
                
                bool preciseResult = getPreciseChecker().check(env, region, parameterOptDir)->asExplicitQualitativeCheckResult()[*getPreciseChecker().getConsideredParametricModel().getInitialStates().begin()];
                bool preciseResultAgrees = preciseResult == (currentResult == RegionResult::AllSat);
                
                if (!preciseResultAgrees) {
                    // Imprecise result is wrong!
                    currentResult = RegionResult::Unknown;
                    ++numOfWrongRegions;
                    
                    // Check the other direction in case no hypothesis was given
                    if (hypothesis == RegionResultHypothesis::Unknown) {
                        parameterOptDir = storm::solver::invert(parameterOptDir);
                        preciseResult = getPreciseChecker().check(env, region, parameterOptDir)->asExplicitQualitativeCheckResult()[*getPreciseChecker().getConsideredParametricModel().getInitialStates().begin()];
                        if (preciseResult && parameterOptDir == getPreciseChecker().getCurrentCheckTask().getOptimizationDirection()) {
                            currentResult = RegionResult::AllSat;
                        } else if (!preciseResult && parameterOptDir == storm::solver::invert(getPreciseChecker().getCurrentCheckTask().getOptimizationDirection())) {
                            currentResult = RegionResult::AllViolated;
                        }
                    }
                }
            }
            
            if (sampleVerticesOfRegion && currentResult != RegionResult::AllSat && currentResult != RegionResult::AllViolated) {
                currentResult = getPreciseChecker().sampleVertices(env, region, currentResult);
            }
    
            return currentResult;
        }

        template class ValidatingSparseParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double, storm::RationalNumber>;
        template class ValidatingSparseParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber>;

    }
}
