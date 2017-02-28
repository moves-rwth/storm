#pragma once

#include <memory>
#include <boost/optional.hpp>

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/parametric/SparseInstantiationModelChecker.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/utility/ModelInstantiator.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            
            /*!
             * Class to efficiently check a formula on a parametric model with different parameter instantiations
             */
            template <typename SparseModelType, typename ConstantType>
            class SparseDtmcInstantiationModelChecker : public SparseInstantiationModelChecker<SparseModelType, ConstantType> {
            public:
                SparseDtmcInstantiationModelChecker(SparseModelType const& parametricModel);
                
                virtual std::unique_ptr<CheckResult> check(storm::utility::parametric::Valuation<typename SparseModelType::ValueType> const& valuation) override;

            protected:
                
                // Considers the result of the last check as a hint for the current check
                std::unique_ptr<CheckResult> checkWithHint(storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ConstantType>>& modelChecker);
                
                storm::utility::ModelInstantiator<SparseModelType, storm::models::sparse::Dtmc<ConstantType>> modelInstantiator;
            };
        }
    }
}
