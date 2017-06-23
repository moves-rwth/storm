#pragma once

#include <memory>

#include "storm/modelchecker/parametric/RegionChecker.h"

namespace storm {
    namespace modelchecker{
        namespace parametric{
            

            template <typename SparseModelType, typename ConstantType, typename ExactConstantType = ConstantType>
            class SparseMdpRegionChecker : public RegionChecker<SparseModelType, ConstantType, ExactConstantType> {
                
            public:
                SparseMdpRegionChecker(SparseModelType const& parametricModel);
                
            protected:

                virtual void initializeUnderlyingCheckers() override;
                virtual void simplifyParametricModel(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) override;
                virtual void applyHintsToExactChecker() override;
                
            };
    
        } //namespace parametric
    } //namespace modelchecker
} //namespace storm
