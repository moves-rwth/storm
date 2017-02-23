#include "SparseDtmcInstantiationModelChecker.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            
            template <typename SparseModelType, typename ConstantType>
            SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>::SparseDtmcInstantiationModelChecker(SparseModelType const& parametricModel) : SparseInstantiationModelChecker<SparseModelType, ConstantType>(parametricModel), modelInstantiator(parametricModel) {
                //Intentionally left empty
            }
    
            template <typename SparseModelType, typename ConstantType>
            bool SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>::canHandle(storm::modelchecker::CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const {
                storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ConstantType>> mc(this->parametricModel);
                return mc.canHandle(checkTask);
            }
    
            template <typename SparseModelType, typename ConstantType>
            std::unique_ptr<CheckResult> SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>::check(storm::utility::parametric::Valuation<typename SparseModelType::ValueType> const& valuation) {
                auto const& instantiatedModel = modelInstantiator.instantiate(valuation);
                storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ConstantType>> mc(instantiatedModel);
                
                // todo check whether the model checker supports hints on the specified property and if this is the case,
                
                return mc.check(*this->currentCheckTask);
            }
            
    
        }
    }
}