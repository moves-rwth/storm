#include "storm/modelchecker/multiobjective/pcaa.h"

#include "storm/utility/macros.h"

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaPreprocessor.h"
#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaAchievabilityQuery.h"
#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaQuantitativeQuery.h"
#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaParetoQuery.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/utility/Stopwatch.h"

#include "storm/exceptions/InvalidArgumentException.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template<typename SparseModelType>
            std::unique_ptr<CheckResult> performPcaa(SparseModelType const& model, storm::logic::MultiObjectiveFormula const& formula) {
                storm::utility::Stopwatch swTotal(true);
                storm::utility::Stopwatch swPreprocessing(true);
                STORM_LOG_ASSERT(model.getInitialStates().getNumberOfSetBits() == 1, "Multi-objective Model checking on model with multiple initial states is not supported.");
                
#ifdef STORM_HAVE_CARL
                
                // If we consider an MA, ensure that it is closed
                if(model.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    STORM_LOG_THROW(dynamic_cast<storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType> const *>(&model)->isClosed(), storm::exceptions::InvalidArgumentException, "Unable to check multi-objective formula on non-closed Markov automaton.");
                }
                
                auto preprocessorResult = SparsePcaaPreprocessor<SparseModelType>::preprocess(model, formula);
                swPreprocessing.stop();
                if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                    STORM_PRINT_AND_LOG("Preprocessing done in " << swPreprocessing << " seconds." << std::endl << " Result: " << preprocessorResult << std::endl);
                } else {
                    STORM_LOG_INFO("Preprocessing done in " << swPreprocessing << " seconds." << std::endl << " Result: " << preprocessorResult << std::endl);
                }
                storm::utility::Stopwatch swValueIterations(true);
                std::unique_ptr<SparsePcaaQuery<SparseModelType, storm::RationalNumber>> query;
                switch (preprocessorResult.queryType) {
                    case SparsePcaaPreprocessorReturnType<SparseModelType>::QueryType::Achievability:
                        query = std::unique_ptr<SparsePcaaQuery<SparseModelType, storm::RationalNumber>> (new SparsePcaaAchievabilityQuery<SparseModelType, storm::RationalNumber>(preprocessorResult));
                        break;
                    case SparsePcaaPreprocessorReturnType<SparseModelType>::QueryType::Quantitative:
                        query = std::unique_ptr<SparsePcaaQuery<SparseModelType, storm::RationalNumber>> (new SparsePcaaQuantitativeQuery<SparseModelType, storm::RationalNumber>(preprocessorResult));
                        break;
                    case SparsePcaaPreprocessorReturnType<SparseModelType>::QueryType::Pareto:
                        query = std::unique_ptr<SparsePcaaQuery<SparseModelType, storm::RationalNumber>> (new SparsePcaaParetoQuery<SparseModelType, storm::RationalNumber>(preprocessorResult));
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Unsupported multi-objective Query Type.");
                        break;
                }

                auto result = query->check();
                swValueIterations.stop();
                swTotal.stop();
                if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                    STORM_PRINT_AND_LOG("Solving multi-objective query took " << swTotal << " seconds (consisting of " << swPreprocessing << " seconds for preprocessing and " << swValueIterations << " seconds for value iteration-based exploration of achievable points)." << std::endl);
                }

                
                if(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isExportPlotSet()) {
                    query->exportPlotOfCurrentApproximation(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getExportPlotDirectory());
                }
                return result;
#else
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Multi-objective model checking requires carl.");
                return nullptr;
#endif
            }
            
            template std::unique_ptr<CheckResult> performPcaa<storm::models::sparse::Mdp<double>>(storm::models::sparse::Mdp<double> const& model, storm::logic::MultiObjectiveFormula const& formula);
            template std::unique_ptr<CheckResult> performPcaa<storm::models::sparse::MarkovAutomaton<double>>(storm::models::sparse::MarkovAutomaton<double> const& model, storm::logic::MultiObjectiveFormula const& formula);
#ifdef STORM_HAVE_CARL
            template std::unique_ptr<CheckResult> performPcaa<storm::models::sparse::Mdp<storm::RationalNumber>>(storm::models::sparse::Mdp<storm::RationalNumber> const& model, storm::logic::MultiObjectiveFormula const& formula);
           // template std::unique_ptr<CheckResult> performPcaa<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>(storm::models::sparse::MarkovAutomaton<storm::RationalNumber> const& model, storm::logic::MultiObjectiveFormula const& formula);
#endif

        }
    }
}
