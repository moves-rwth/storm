#pragma once

#include <memory>

#include "storm/modelchecker/parametric/ParameterLifting.h"

namespace storm {
    namespace modelchecker{
        namespace parametric{
            

            template<typename SparseModelType, typename ConstantType>
            class SparseMdpParameterLifting : public ParameterLifting<SparseModelType, ConstantType> {
                
            public:
                SparseMdpParameterLifting(SparseModelType const& parametricModel);
                
            protected:

                virtual void initializeUnderlyingCheckers() override;
                virtual void simplifyParametricModel(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) override;
                virtual void applyHintsToExactChecker() override;
                
            };
    
        } //namespace parametric
    } //namespace modelchecker
} //namespace storm
