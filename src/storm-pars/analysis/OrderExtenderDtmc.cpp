#include "storm-pars/analysis/OrderExtenderDtmc.h"


namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        OrderExtenderDtmc<ValueType, ConstantType>::OrderExtenderDtmc(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, storage::ParameterRegion<ValueType> region) : OrderExtender<ValueType, ConstantType>(model, formula, region) {
            // intentionally left empty
        }

        template<typename ValueType, typename ConstantType>
        OrderExtenderDtmc<ValueType, ConstantType>::OrderExtenderDtmc(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            // intentionally left empty
        }

    }
}