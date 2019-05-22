#include "DftTransformator.h"

namespace storm {
    namespace transformations {
        namespace dft {
            template<typename ValueType>
            DftTransformator<ValueType>::DftTransformator(storm::storage::DFT<ValueType> const &dft) : mDft(dft) {}

            template<typename ValueType>
            storm::storage::DFT<ValueType> DftTransformator<ValueType>::transformUniqueFailedBe() {
                // For now, this only creates an empty DFT
                storm::builder::DFTBuilder<ValueType> builder;

                for (size_t i = 0; i < mDft.nrElements(); ++i) {
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> element = mDft.getElement(i);
                    //TODO SWITCH OVER ELEMENTS
                }
                //builder.setTopLevel(mDft.getTopLevelGate()->name());
                return builder.build();
            }

            // Explicitly instantiate the class.
            template
            class DftTransformator<double>;

#ifdef STORM_HAVE_CARL

            template
            class DftTransformator<RationalFunction>;

#endif
        }
    }
}
