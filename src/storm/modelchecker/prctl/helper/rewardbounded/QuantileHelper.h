#pragma once

#include "storm/logic/QuantileFormula.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            namespace rewardbounded {
                
                template<typename ModelType>
                class QuantileHelper {
                    typedef typename ModelType::ValueType ValueType;
                public:
                    QuantileHelper(ModelType const& model, storm::logic::QuantileFormula const& formula) {}
                    
                    std::vector<std::vector<ValueType>> computeMultiDimensionalQuantile() { return {{27}};}
                };
            }
        }
    }
}
