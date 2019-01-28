#pragma once

#include "storm/logic/QuantileFormula.h"

namespace storm {
    class Environment;

    namespace modelchecker {
        namespace helper {
            namespace rewardbounded {
                
                template<typename ModelType>
                class QuantileHelper {
                    typedef typename ModelType::ValueType ValueType;
                public:
                    QuantileHelper(ModelType const& model, storm::logic::QuantileFormula const& quantileFormula);

                    std::vector<std::vector<ValueType>> computeMultiDimensionalQuantile(Environment const& env);



                private:
                    bool computeUnboundedValue(Environment const& env);

                    ModelType const& model;
                    storm::logic::QuantileFormula const& quantileFormula;
                };
            }
        }
    }
}
