#include "storm/modelchecker/multiobjective/pcaa/PcaaWeightVectorChecker.h"

#include "storm/modelchecker/multiobjective/pcaa/SparseMaPcaaWeightVectorChecker.h"
#include "storm/modelchecker/multiobjective/pcaa/SparseMdpPcaaWeightVectorChecker.h"
#include "storm/modelchecker/multiobjective/pcaa/SparseMdpRewardBoundedPcaaWeightVectorChecker.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <typename ModelType>
            PcaaWeightVectorChecker<ModelType>::PcaaWeightVectorChecker(std::vector<Objective<ValueType>> const& objectives) : objectives(objectives) {
                // Intentionally left empty
            }
            
            template <typename ModelType>
            void PcaaWeightVectorChecker<ModelType>::setWeightedPrecision(ValueType const& value) {
                weightedPrecision = value;
            }
            
            template <typename ModelType>
            typename PcaaWeightVectorChecker<ModelType>::ValueType const& PcaaWeightVectorChecker<ModelType>::getWeightedPrecision() const {
                return weightedPrecision;
            }
            
            template <typename ModelType>
            storm::storage::Scheduler<typename PcaaWeightVectorChecker<ModelType>::ValueType> PcaaWeightVectorChecker<ModelType>::computeScheduler() const {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Scheduler generation is not supported in this setting.");
            }

            template <typename ModelType>
            template<typename VT, typename std::enable_if<std::is_same<ModelType, storm::models::sparse::Mdp<VT>>::value, int>::type>
            std::unique_ptr<PcaaWeightVectorChecker<ModelType>>  WeightVectorCheckerFactory<ModelType>::create(SparseMultiObjectivePreprocessorResult<ModelType> const& preprocessorResult) {
                if (preprocessorResult.containsOnlyRewardObjectives()) {
                    return std::make_unique<SparseMdpPcaaWeightVectorChecker<ModelType>>(preprocessorResult);
                } else {
                    return std::make_unique<SparseMdpRewardBoundedPcaaWeightVectorChecker<ModelType>>(preprocessorResult);
                }
            }
            
            template <typename ModelType>
            template<typename VT, typename std::enable_if<std::is_same<ModelType, storm::models::sparse::MarkovAutomaton<VT>>::value, int>::type>
            std::unique_ptr<PcaaWeightVectorChecker<ModelType>>  WeightVectorCheckerFactory<ModelType>::create(SparseMultiObjectivePreprocessorResult<ModelType> const& preprocessorResult) {
                return std::make_unique<SparseMaPcaaWeightVectorChecker<ModelType>>(preprocessorResult);
            }
            
            template class PcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
            template class PcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
            template class PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;

            template class WeightVectorCheckerFactory<storm::models::sparse::Mdp<double>>;
            template class WeightVectorCheckerFactory<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<double>>;
            template class WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
            
            template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>>  WeightVectorCheckerFactory<storm::models::sparse::Mdp<double>>::create(SparseMultiObjectivePreprocessorResult<storm::models::sparse::Mdp<double>> const& preprocessorResult);
            template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>>  WeightVectorCheckerFactory<storm::models::sparse::Mdp<storm::RationalNumber>>::create(SparseMultiObjectivePreprocessorResult<storm::models::sparse::Mdp<storm::RationalNumber>> const& preprocessorResult);
            template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>>  WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<double>>::create(SparseMultiObjectivePreprocessorResult<storm::models::sparse::MarkovAutomaton<double>> const& preprocessorResult);
            template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>>  WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::create(SparseMultiObjectivePreprocessorResult<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>> const& preprocessorResult);
            

        }
    }
}

