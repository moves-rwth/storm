#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPOSTPROCESSOR_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPOSTPROCESSOR_H_


#include <memory>
#include <boost/optional.hpp>

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessorData.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveResultData.h"
#include "src/modelchecker/results/CheckResult.h"
#include "src/utility/Stopwatch.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            /*
             * Helper Class to invoke the necessary preprocessing for multi objective model checking
             */
            template <class SparseModelType, typename RationalNumberType>
            class SparseMultiObjectivePostprocessor {
            public:
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                
                typedef SparseMultiObjectiveObjectiveInformation<ValueType> ObjectiveInformation;
                typedef SparseMultiObjectivePreprocessorData<SparseModelType> PreprocessorData;
                typedef SparseMultiObjectiveResultData<RationalNumberType> ResultData;
                
                /*!
                 * Postprocesses the multi objective model checking result.
                 *
                 * @param preprocessorData the data of the preprocessing
                 * @param resultData the data of the model checking
                 */
                static std::unique_ptr<CheckResult> postprocess(PreprocessorData const& preprocessorData, ResultData const& resultData, boost::optional<storm::utility::Stopwatch> const& preprocessorStopwatch = boost::none, boost::optional<storm::utility::Stopwatch> const& helperStopwatch = boost::none, boost::optional<storm::utility::Stopwatch> const& postprocessorStopwatch = boost::none);
                
            private:
                static std::unique_ptr<CheckResult> postprocessAchievabilityQuery(PreprocessorData const& preprocessorData, ResultData const& resultData);
                static std::unique_ptr<CheckResult> postprocessNumericalQuery(PreprocessorData const& preprocessorData, ResultData const& resultData);
                static std::unique_ptr<CheckResult> postprocessParetoQuery(PreprocessorData const& preprocessorData, ResultData const& resultData);
                
                static std::vector<RationalNumberType> transformToOriginalValues(std::vector<RationalNumberType> const& vector, PreprocessorData const& preprocessorData);
                static std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> transformToOriginalValues(std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& polytope, PreprocessorData const& preprocessorData);
                
                static void exportPlot(std::unique_ptr<CheckResult> const& checkResult, PreprocessorData const& preprocessorData, ResultData const& resultData, boost::optional<storm::utility::Stopwatch> const& preprocessorStopwatch, boost::optional<storm::utility::Stopwatch> const& helperStopwatch, boost::optional<storm::utility::Stopwatch> const& postprocessorStopwatch);
                
                static std::string getInfo(std::unique_ptr<CheckResult> const& checkResult, PreprocessorData const& preprocessorData, ResultData const& resultData, boost::optional<storm::utility::Stopwatch> const& preprocessorStopwatch, boost::optional<storm::utility::Stopwatch> const& helperStopwatch, boost::optional<storm::utility::Stopwatch> const& postprocessorStopwatch);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPOSTPROCESSOR_H_ */
