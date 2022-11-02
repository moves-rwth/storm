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

            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> PomdpManagerAposteriori<ValueType>::constructMdp() {
                // TODO
                return NULL;
            }

            template class PomdpManagerAposteriori<double>;

    }
}