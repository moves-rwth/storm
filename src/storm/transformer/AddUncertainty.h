#include "storm/models/sparse/Model.h"

namespace storm::transformer {

/**
 * This class is a convenience transformer to add uncertainty.
 * We currently support only one type of self-defined uncertainty, although additional types of uncertainty are imaginable.
 * The transformer does maintain reward models, state labels, state valuations, choice labels and choice origins.
 *
 * @tparam ValueType
 */
template<typename ValueType>
class AddUncertainty {
   public:
    AddUncertainty(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& originalModel);
    std::shared_ptr<storm::models::sparse::Model<Interval>> transform(double additiveUncertainty, double minimalValue = 0.0001);

   private:
    storm::Interval addUncertainty(ValueType const& vt, double additiveUncertainty, double minimalValue);
    std::shared_ptr<storm::models::sparse::Model<ValueType>> origModel;
};

}  // namespace storm::transformer