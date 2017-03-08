#pragma once

#include <memory>

#include "storm/modelchecker/parametric/RegionCheckResult.h"
#include "storm/modelchecker/parametric/SparseInstantiationModelChecker.h"
#include "storm/modelchecker/parametric/SparseParameterLiftingModelChecker.h"
#include "storm/storage/ParameterRegion.h"

#include "storm/modelchecker/CheckTask.h"

namespace storm {
    namespace modelchecker{
        namespace parametric{

            template<typename SparseModelType, typename ConstantType>
            class ParameterLifting {

            public:

                ParameterLifting(SparseModelType const& parametricModel);

                ~ParameterLifting() = default;
                
                void specifyFormula(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask);
                
                /*!
                 * Analyzes the given region by applying parameter lifting.
                 * We first check whether all points in the region violate/satisfy the property
                 * If this does not yield a conclusive result and if the given flag is true, we also sample the vertices of the region
                 */
                RegionCheckResult analyzeRegion(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, bool sampleVerticesOfRegion) const;
                
                /*!
                 * Iteratively refines the region until parameter lifting yields a conclusive result (AllSat or AllViolated).
                 * The refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then the given threshold
                 */
                std::vector<std::pair<storm::storage::ParameterRegion<typename SparseModelType::ValueType>, RegionCheckResult>> performRegionRefinement(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::CoefficientType const& threshold) const;
                
                SparseParameterLiftingModelChecker<SparseModelType, ConstantType> const& getParameterLiftingChecker() const;
                SparseInstantiationModelChecker<SparseModelType, ConstantType> const& getInstantiationChecker() const;
                
            private:
                SparseModelType const& getConsideredParametricModel() const;
                
                void initializeUnderlyingCheckers();
                void simplifyParametricModel(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask);
                
                
                SparseModelType const& parametricModel;
                std::unique_ptr<CheckTask<storm::logic::Formula, typename SparseModelType::ValueType>> currentCheckTask;
                std::shared_ptr<storm::logic::Formula const> currentFormula;
                std::shared_ptr<SparseModelType> simplifiedModel;
                

                std::unique_ptr<SparseParameterLiftingModelChecker<SparseModelType, ConstantType>> parameterLiftingChecker;
                std::unique_ptr<SparseInstantiationModelChecker<SparseModelType, ConstantType>> instantiationChecker;
                
                
            };
    
        } //namespace parametric
    } //namespace modelchecker
} //namespace storm
