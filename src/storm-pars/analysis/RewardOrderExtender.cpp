#include "storm-pars/analysis/RewardOrderExtender.h"


namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        RewardOrderExtender<ValueType, ConstantType>::RewardOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula) : OrderExtender<ValueType, ConstantType>(model, formula) {
            // intentionally left empty
        }

        template<typename ValueType, typename ConstantType>
        RewardOrderExtender<ValueType, ConstantType>::RewardOrderExtender(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            // intentionally left empty
        }

        template class RewardOrderExtender<RationalFunction, double>;
        template class RewardOrderExtender<RationalFunction, RationalNumber>;

    }
}