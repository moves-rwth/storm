#ifndef DFTTOGSPNTRANSFORMATOR_H
#define DFTTOGSPNTRANSFORMATOR_H

#include <src/storage/dft/DFT.h>
#include "src/storage/gspn/GSPN.h"

namespace storm {
    namespace transformations {
        namespace dft {

            /*!
             * Transformator for DFT -> GSPN.
             */
            template<typename ValueType>
            class DftToGspnTransformator {

            public:
                /*!
                 * Constructor.
                 *
                 * @param dft DFT
                 */
                DftToGspnTransformator(storm::storage::DFT<ValueType> const& dft);

                /*!
                 * Transform the DFT to a GSPN.
                 */
                void transform();

            private:

                storm::storage::DFT<ValueType> const& mDft;
                storm::gspn::GSPN mGspn;
                
                /*!
                 * Write Gspn to file or console.
                 *
                 * @param toFile If true, the GSPN will be written to a file, otherwise it will
                                 be written to the console.
                 */
                void writeGspn(bool toFile);
            };
        }
    }
}

#endif /* DFTTOGSPNTRANSFORMATOR_H*/
