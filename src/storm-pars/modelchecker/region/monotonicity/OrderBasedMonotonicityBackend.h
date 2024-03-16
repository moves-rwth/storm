#pragma once

#include <map>
#include <optional>
#include <set>
#include <vector>

#include "storm/solver/OptimizationDirection.h"

#include "storm-pars/analysis/MonotonicityChecker.h"
#include "storm-pars/analysis/OrderExtender.h"
#include "storm-pars/modelchecker/region/monotonicity/MonotonicityBackend.h"

namespace storm::transformer {
template<typename ParametricType, typename ConstantType>
class ParameterLifter;
}  // namespace storm::transformer

namespace storm::storage {
class BitVector;
template<typename T>
class SparseMatrix;
}  // namespace storm::storage

namespace storm::modelchecker {

template<typename ParametricType, typename ConstantType>
class SparseDtmcParameterLiftingModelChecker;

template<typename ParametricType, typename ConstantType>
class OrderBasedMonotonicityBackend : public MonotonicityBackend<ParametricType> {
   public:
    using CoefficientType = MonotonicityBackend<ParametricType>::CoefficientType;
    using VariableType = MonotonicityBackend<ParametricType>::VariableType;
    using Valuation = MonotonicityBackend<ParametricType>::Valuation;
    using MonotonicityKind = MonotonicityBackend<ParametricType>::MonotonicityKind;

    friend class SparseDtmcParameterLiftingModelChecker<ParametricType, ConstantType>;

    OrderBasedMonotonicityBackend(bool useOnlyGlobal = false, bool useBounds = false);
    virtual ~OrderBasedMonotonicityBackend() = default;

    /*!
     * Returns true, since a region model checker needs to implement specific methods to properly use this backend.
     */
    virtual bool requiresInteractionWithRegionModelChecker() const override;

    /*!
     * Initializes the monotonicity information for the given region.
     * Overwrites all present monotonicity annotations in the given region.
     */
    virtual void initializeMonotonicity(detail::AnnotatedRegion<ParametricType>& region) override;

    /*!
     * Updates the monotonicity information for the given region.
     * Assumes that some monotonicity information is already present (potentially inherited from a parent region) and potentially sharpens the results for the
     * given region.
     */
    virtual void updateMonotonicity(detail::AnnotatedRegion<ParametricType>& region) override;

    /*!
     * Returns an optimistic approximation of the monotonicity of the parameters in this region.
     * This means that the returned monotonicity does not necessarily hold, but there is "sufficient hope" that it does.
     */
    virtual std::map<VariableType, MonotonicityKind> getOptimisticMonotonicityApproximation(detail::AnnotatedRegion<ParametricType> const& region) override;

   private:
    // Interaction with SparseDtmcParameterLiftingModelChecker:
    void initializeMonotonicityChecker(storm::storage::SparseMatrix<ParametricType> const& parametricTransitionMatrix);
    void initializeOrderExtender(storm::storage::BitVector const& topStates, storm::storage::BitVector const& bottomStates,
                                 storm::storage::SparseMatrix<ParametricType> const& parametricTransitionMatrix, );
    storm::storage::BitVector getChoicesToFixForPLASolver(detail::AnnotatedRegion<ParametricType> const& region,
                                                          storm::transformer::ParameterLifter<ParametricType, ConstantType> const& parameterLifter,
                                                          storm::OptimizationDirection dir, std::vector<uint64_t>& schedulerChoices);

   private:
    bool const useOnlyGlobal;
    bool const useBounds;

    std::optional<storm::analysis::OrderExtender<ParametricType, ConstantType>> orderExtender;
    std::optional<storm::analysis::MonotonicityChecker<ParametricType>> monotonicityChecker;
    std::set<VariableType> possibleMonotoneParameters;
};

}  // namespace storm::modelchecker