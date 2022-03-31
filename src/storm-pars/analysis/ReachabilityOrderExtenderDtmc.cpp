#include "storm-pars/analysis/ReachabilityOrderExtenderDtmc.h"

namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
    ReachabilityOrderExtenderDtmc<ValueType, ConstantType>::ReachabilityOrderExtenderDtmc(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula) : ReachabilityOrderExtender<ValueType, ConstantType>(model, formula) {
            // intentionally left empty
        }

        template<typename ValueType, typename ConstantType>
        ReachabilityOrderExtenderDtmc<ValueType, ConstantType>::ReachabilityOrderExtenderDtmc(storm::storage::BitVector& topStates,  storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : ReachabilityOrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            // intentionally left empty
        }

        template<typename ValueType, typename ConstantType>
        bool ReachabilityOrderExtenderDtmc<ValueType, ConstantType>::findBestAction(std::shared_ptr<Order> order, storage::ParameterRegion<ValueType>& region, uint_fast64_t state) {
            return true;
        }

        template class ReachabilityOrderExtenderDtmc<RationalFunction, double>;
        template class ReachabilityOrderExtenderDtmc<RationalFunction, RationalNumber>;

    }
}