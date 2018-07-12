#pragma once

#include <memory>
#include <boost/optional.hpp>

#include "storm-pars/modelchecker/instantiation/SparseInstantiationModelChecker.h"
#include "storm-pars/utility/ModelInstantiator.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"

namespace storm {
    namespace modelchecker {
        
        /*!
         * Class to efficiently check a formula on a parametric model with different parameter instantiations.
         */
        template <typename SparseModelType, typename ConstantType>
        class SparseCtmcInstantiationModelChecker : public SparseInstantiationModelChecker<SparseModelType, ConstantType> {
        public:
            SparseCtmcInstantiationModelChecker(SparseModelType const& parametricModel);
            
            virtual std::unique_ptr<CheckResult> check(Environment const& env, storm::utility::parametric::Valuation<typename SparseModelType::ValueType> const& valuation) override;
            
            storm::utility::ModelInstantiator<SparseModelType, storm::models::sparse::Ctmc<ConstantType>> modelInstantiator;
        };
    }
}
