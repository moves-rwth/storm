#ifndef DFTTOGSPNTRANSFORMATOR_H
#define DFTTOGSPNTRANSFORMATOR_H

#include <src/storage/dft/DFT.h>

namespace storm {
    namespace transformations {
        namespace dft {

            template<typename ValueType>
            class DftToGspnTransformator {

            public:
                DftToGspnTransformator(storm::storage::DFT<ValueType> const& dft);

            private:

                storm::storage::DFT<ValueType> const& mDft;

            };
        }
    }
}

#endif /* DFTTOGSPNTRANSFORMATOR_H*/
