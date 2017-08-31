#include "storm/modelchecker/multiobjective/pcaa/PcaaWeightVectorChecker.h"

#include "storm/modelchecker/multiobjective/pcaa/SparseMaPcaaWeightVectorChecker.h"
#include "storm/modelchecker/multiobjective/pcaa/SparseMdpPcaaWeightVectorChecker.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <typename ModelType>
            PcaaWeightVectorChecker<ModelType>::PcaaWeightVectorChecker(ModelType const& model, std::vector<Objective<ValueType>> const& objectives) : model(model), objectives(objectives) {
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
            std::unique_ptr<PcaaWeightVectorChecker<ModelType>>  WeightVectorCheckerFactory<ModelType>::create(ModelType const& model,
                                                   std::vector<Objective<typename ModelType::ValueType>> const& objectives,
                                                   storm::storage::BitVector const& possibleECActions,
                                                   storm::storage::BitVector const& possibleBottomStates) {
                return std::make_unique<SparseMdpPcaaWeightVectorChecker<ModelType>>(model, objectives, possibleECActions, possibleBottomStates);
            }
            
            template <typename ModelType>
            template<typename VT, typename std::enable_if<std::is_same<ModelType, storm::models::sparse::MarkovAutomaton<VT>>::value, int>::type>
            std::unique_ptr<PcaaWeightVectorChecker<ModelType>>  WeightVectorCheckerFactory<ModelType>::create(ModelType const& model,
                                                   std::vector<Objective<typename ModelType::ValueType>> const& objectives,
                                                   storm::storage::BitVector const& possibleECActions,
                                                   storm::storage::BitVector const& possibleBottomStates) {
            
                return std::make_unique<SparseMaPcaaWeightVectorChecker<ModelType>>(model, objectives, possibleECActions, possibleBottomStates);
            }
            
            template class PcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
            template class PcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
            template class PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;

            template class WeightVectorCheckerFactory<storm::models::sparse::Mdp<double>>;
            template class WeightVectorCheckerFactory<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<double>>;
            template class WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
            
            template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>>  WeightVectorCheckerFactory<storm::models::sparse::Mdp<double>>::create(storm::models::sparse::Mdp<double> const& model, std::vector<Objective<double>> const& objectives, storm::storage::BitVector const& possibleECActions, storm::storage::BitVector const& possibleBottomStates);
            template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>>  WeightVectorCheckerFactory<storm::models::sparse::Mdp<storm::RationalNumber>>::create(storm::models::sparse::Mdp<storm::RationalNumber> const& model, std::vector<Objective<storm::RationalNumber>> const& objectives, storm::storage::BitVector const& possibleECActions, storm::storage::BitVector const& possibleBottomStates);
            template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>>  WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<double>>::create(storm::models::sparse::MarkovAutomaton<double> const& model, std::vector<Objective<double>> const& objectives, storm::storage::BitVector const& possibleECActions, storm::storage::BitVector const& possibleBottomStates);
            template std::unique_ptr<PcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>>  WeightVectorCheckerFactory<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::create(storm::models::sparse::MarkovAutomaton<storm::RationalNumber> const& model, std::vector<Objective<storm::RationalNumber>> const& objectives, storm::storage::BitVector const& possibleECActions, storm::storage::BitVector const& possibleBottomStates);
            

        }
    }
}

