#pragma once

#include <memory>

#include "storm/modelchecker/parametric/RegionChecker.h"

namespace storm {
    namespace modelchecker{
        namespace parametric{
            

            template <typename SparseModelType, typename ConstantType, typename ExactConstantType = ConstantType>
            class SparseDtmcRegionChecker : public RegionChecker<SparseModelType, ConstantType, ExactConstantType> {

            public:
                SparseDtmcRegionChecker(SparseModelType const& parametricModel);
                
            protected:
                virtual void simplifyParametricModel(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) override;
                virtual void initializeUnderlyingCheckers() override;
                virtual void applyHintsToExactChecker() override;
                
            };
    
        } //namespace parametric
    } //namespace modelchecker
} //namespace storm
