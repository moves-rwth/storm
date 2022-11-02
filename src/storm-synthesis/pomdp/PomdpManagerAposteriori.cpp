#include "storm-synthesis/pomdp/PomdpManagerAposteriori.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/storage/sparse/ModelComponents.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace synthesis {

            
            template<typename ValueType>
            PomdpManagerAposteriori<ValueType>::PomdpManagerAposteriori(storm::models::sparse::Pomdp<ValueType> const& pomdp)
            : pomdp(pomdp) {
                STORM_LOG_THROW(pomdp.isCanonic(), storm::exceptions::InvalidArgumentException, "POMDP must be canonic");

                auto num_observations = pomdp.getNrObservations();
                this->observation_memory_size.resize(num_observations, 1);

            }
            
            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::setObservationMemorySize(uint64_t obs, uint64_t memory_size) {
                assert(obs < this->pomdp.getNrObservations());
                this->observation_memory_size[obs] = memory_size;
            }

            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::setGlobalMemorySize(uint64_t memory_size) {
                for(uint64_t obs = 0; obs < this->pomdp.getNrObservations(); obs++) {
                    this->observation_memory_size[obs] = memory_size;
                }
            }

            template<typename ValueType>
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> PomdpManagerAposteriori<ValueType>::constructMdp() {
                // TODO
                return NULL;
            }

            template class PomdpManagerAposteriori<double>;

    }
}