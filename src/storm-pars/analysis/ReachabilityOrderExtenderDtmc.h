#pragma once
#include "storm-pars/analysis/ReachabilityOrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class ReachabilityOrderExtenderDtmc : public ReachabilityOrderExtender<ValueType, ConstantType> {
            public:
                typedef typename utility::parametric::VariableType<ValueType>::type VariableType;

                ReachabilityOrderExtenderDtmc(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula);

                ReachabilityOrderExtenderDtmc(storm::storage::BitVector& topStates,  storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix);

               protected:
                bool findBestAction(std::shared_ptr<Order> order, storage::ParameterRegion<ValueType>& region, uint_fast64_t state) override;

        };
    }
}