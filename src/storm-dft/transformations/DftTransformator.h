#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/builder/DFTBuilder.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace transformations {
        namespace dft {

            /*!
             * Transformator for DFT -> DFT.
             */
            template<typename ValueType>
            class DftTransformator {

            public:
                /*!
                 * Constructor.
                 *
                 * @param dft DFT
                 */
                DftTransformator(storm::storage::DFT<ValueType> const &dft);

                storm::storage::DFT<ValueType> transformUniqueFailedBe();

            private:
                std::vector<std::string>
                getChildrenVector(std::shared_ptr<storm::storage::DFTElement<ValueType> const> element);

                storm::storage::DFT<ValueType> const &mDft;
            };
        }
    }
}
