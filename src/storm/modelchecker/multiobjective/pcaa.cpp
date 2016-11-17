#include "src/storm/modelchecker/multiobjective/pcaa.h"

#include "src/storm/utility/macros.h"

#include "src/storm/models/sparse/Mdp.h"
#include "src/storm/models/sparse/MarkovAutomaton.h"
#include "src/storm/models/sparse/StandardRewardModel.h"
#include "src/storm/modelchecker/multiobjective/pcaa/SparsePcaaPreprocessor.h"
#include "src/storm/modelchecker/multiobjective/pcaa/SparsePcaaAchievabilityQuery.h"
#include "src/storm/modelchecker/multiobjective/pcaa/SparsePcaaQuantitativeQuery.h"
#include "src/storm/modelchecker/multiobjective/pcaa/SparsePcaaParetoQuery.h"
#include "src/storm/settings//SettingsManager.h"
#include "src/storm/settings/modules/MultiObjectiveSettings.h"

#include "src/storm/exceptions/InvalidArgumentException.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template<typename SparseModelType>
            std::unique_ptr<CheckResult> performPcaa(SparseModelType const& model, storm::logic::MultiObjectiveFormula const& formula) {
                STORM_LOG_ASSERT(model.getInitialStates().getNumberOfSetBits() == 1, "Multi-objective Model checking on model with multiple initial states is not supported.");
                
#ifdef STORM_HAVE_CARL
                
                // If we consider an MA, ensure that it is closed
                if(model.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    STORM_LOG_THROW(dynamic_cast<storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType> const *>(&model)->isClosed(), storm::exceptions::InvalidArgumentException, "Unable to check multi-objective formula on non-closed Markov automaton.");
                }
                
                auto preprocessorResult = SparsePcaaPreprocessor<SparseModelType>::preprocess(model, formula);
                STORM_LOG_DEBUG("Preprocessing done. Result: " << preprocessorResult);
                
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
                
                if(settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isExportPlotSet()) {
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
