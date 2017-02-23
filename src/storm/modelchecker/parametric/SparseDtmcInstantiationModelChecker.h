#pragma once

#include "storm/logic/Formulas.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/utility/parametric.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            template <typename SparseModelType, typename ConstantType>
            
            
            /*!
             * Class to efficiently check a formula on a parametric model on different parameter instantiations
             */
            class SparseInstantiationModelChecker {
            public:
                SparseInstantiationModelChecker(SparseModelType const& parametricModel);
                
                virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) = 0;
                
                virtual void specifyFormula(CheckTask<storm::logic::Formula, ValueType> const& checkTask) = 0;
                
                virtual std::unique_ptr<CheckResult> check(storm::utility::parametric::Valuation<typename SparseModelType::ValueType> const& valuation) = 0;

            private:
                
                SparseModelType const& parametricModel;
                
            };
        }
    }
}


#endif //STORM_SPARSEINSTANTIATIONMODELCHECKER_H
