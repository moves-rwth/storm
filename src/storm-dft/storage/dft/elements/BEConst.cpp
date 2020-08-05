#include "BEConst.h"

namespace storm {
    namespace storage {

        template <typename ValueType>
        ValueType BEConst<ValueType>::getUnreliability(ValueType time) const {
            return failed() ? storm::utility::one<ValueType>() : storm::utility::zero<ValueType>();
        }

        // Explicitly instantiate the class.
        template class BEConst<double>;
        template class BEConst<RationalFunction>;

    } // namespace storage
} // namespace storm
