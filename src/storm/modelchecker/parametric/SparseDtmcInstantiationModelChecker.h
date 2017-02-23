#pragma once

#include <memory>

#include "storm/models/sparse/Dtmc.h"
#include "storm/modelchecker/parametric/SparseInstantiationModelChecker.h"
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
                
                virtual bool canHandle(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const override;
                
                virtual void specifyFormula(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) override;
                
                virtual std::unique_ptr<CheckResult> check(storm::utility::parametric::Valuation<typename SparseModelType::ValueType> const& valuation) override;

            protected:
                storm::utility::ModelInstantiator<SparseModelType, storm::models::sparse::Dtmc<ConstantType>> modelInstantiator;
            };
        }
    }
}
