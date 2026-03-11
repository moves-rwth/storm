#pragma once

#include <type_traits>

#include "storm/adapters/IntervalForward.h"
#include "storm/adapters/RationalNumberForward.h"
#include "storm/models/sparse/Model.h"

namespace storm::transformer {

/**
 * This class is a convenience transformer to add uncertainty.
 * We currently support only one type of self-defined uncertainty, although additional types of uncertainty are imaginable.
 * The transformer does maintain reward models, state labels, state valuations, choice labels and choice origins.
 *
 * When ValueType is storm::RationalNumber the output model uses storm::RationalInterval (exact arithmetic).
 * For all other ValueTypes (e.g. double) the output uses storm::Interval (double-precision).
 *
 * @tparam ValueType  The value type of the input model.
 */
template<typename ValueType>
class AddUncertainty {
   public:
    using IntervalType = std::conditional_t<std::is_same_v<ValueType, storm::RationalNumber>, storm::RationalInterval, storm::Interval>;

    AddUncertainty(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& originalModel);
    std::shared_ptr<storm::models::sparse::Model<IntervalType>> transform(double additiveUncertainty, double minimalValue = 0.0001,
                                                                          uint64_t maxSuccessors = 10000000);

   private:
    IntervalType addUncertainty(ValueType const& vt, double additiveUncertainty, double minimalValue);
    std::shared_ptr<storm::models::sparse::Model<ValueType>> origModel;
};

}  // namespace storm::transformer