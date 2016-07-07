#include "src/transformations/dft/DftToGspnTransformator.h"

namespace storm {
    namespace transformations {
        namespace dft {

            template <typename ValueType>
            DftToGspnTransformator<ValueType>::DftToGspnTransformator(storm::storage::DFT<ValueType> const& dft) : mDft(dft) {
                // Intentionally left empty
            }


            // Explicitly instantiate the class.
            template class DftToGspnTransformator<double>;

    #ifdef STORM_HAVE_CARL
            template class DftToGspnTransformator<storm::RationalFunction>;
    #endif

        } // namespace dft
    } // namespace transformations
} // namespace storm


