#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace models {
        namespace sparse {

            template <typename ValueType, typename RewardModelType>
            Pomdp<ValueType, RewardModelType>::Pomdp(storm::storage::SparseMatrix<ValueType> const &transitionMatrix, storm::models::sparse::StateLabeling const &stateLabeling, std::unordered_map <std::string, RewardModelType> const &rewardModels) : Mdp<ValueType, RewardModelType>(transitionMatrix, stateLabeling, rewardModels, storm::models::ModelType::Pomdp) {
                computeNrObservations();
            }

            template <typename ValueType, typename RewardModelType>
            Pomdp<ValueType, RewardModelType>::Pomdp(storm::storage::SparseMatrix<ValueType> &&transitionMatrix, storm::models::sparse::StateLabeling &&stateLabeling, std::unordered_map <std::string, RewardModelType> &&rewardModels) : Mdp<ValueType, RewardModelType>(transitionMatrix, stateLabeling, rewardModels, storm::models::ModelType::Pomdp) {
                computeNrObservations();
            }

            template <typename ValueType, typename RewardModelType>
            Pomdp<ValueType, RewardModelType>::Pomdp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const &components, bool canonicFlag) : Mdp<ValueType, RewardModelType>(components, storm::models::ModelType::Pomdp), observations(components.observabilityClasses.get()), canonicFlag(canonicFlag)  {
                computeNrObservations();
            }

            template <typename ValueType, typename RewardModelType>
            Pomdp<ValueType, RewardModelType>::Pomdp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> &&components, bool canonicFlag): Mdp<ValueType, RewardModelType>(components, storm::models::ModelType::Pomdp), observations(components.observabilityClasses.get()), canonicFlag(canonicFlag) {
                computeNrObservations();
            }

            template<typename ValueType, typename RewardModelType>
            void Pomdp<ValueType, RewardModelType>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                out << "Choices: \t" << this->getNumberOfChoices() << std::endl;
                out << "Observations: \t" << this->nrObservations << std::endl;
                this->printModelInformationFooterToStream(out);
            }

            template<typename ValueType, typename RewardModelType>
            void Pomdp<ValueType, RewardModelType>::computeNrObservations() {
                uint64_t highestEntry = 0;
                for (uint32_t entry : observations) {
                    if (entry > highestEntry) {
                        highestEntry = entry;
                    }
                }
                nrObservations = highestEntry + 1; // Smallest entry should be zero.
                // In debug mode, ensure that every observability is used.
            }

            template<typename ValueType, typename RewardModelType>
            uint32_t Pomdp<ValueType, RewardModelType>::getObservation(uint64_t state) const {
                return observations.at(state);
            }

            template<typename ValueType, typename RewardModelType>
            uint64_t Pomdp<ValueType, RewardModelType>::getNrObservations() const {
                return nrObservations;
            }

            template<typename ValueType, typename RewardModelType>
            std::vector<uint32_t> const& Pomdp<ValueType, RewardModelType>::getObservations() const {
                return observations;
            }

            template<typename ValueType, typename RewardModelType>
            bool Pomdp<ValueType, RewardModelType>::isCanonic() const {
                return canonicFlag;
            }





            template class Pomdp<double>;
            template class Pomdp<storm::RationalNumber>;
            template class Pomdp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Pomdp<storm::RationalFunction>;

        }
    }
}