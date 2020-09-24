#pragma once
#include "storm-pars/analysis/OrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class OrderExtenderMdp : public OrderExtender<ValueType, ConstantType> {

        };
    }
}