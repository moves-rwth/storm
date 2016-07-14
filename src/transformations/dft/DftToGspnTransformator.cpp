#include "src/transformations/dft/DftToGspnTransformator.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace transformations {
        namespace dft {

            template <typename ValueType>
            DftToGspnTransformator<ValueType>::DftToGspnTransformator(storm::storage::DFT<ValueType> const& dft) : mDft(dft) {
                // Intentionally left empty
            }

            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::transform() {
                mGspn = storm::gspn::GSPN();
                
                // For debugging purposes
                writeGspn(false);

                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation to GSPN is not yet implemented.");
            }

            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::writeGspn(bool toFile) {
                if (toFile) {
                    // Writing to file
                    std::ofstream file;
                    file.open("gspn.dot");
                    mGspn.writeDotToStream(file);
                    file.close();
                } else {
                    // Writing to console
                    mGspn.writeDotToStream(std::cout);
                }
            }

            // Explicitly instantiate the class.
            template class DftToGspnTransformator<double>;

    #ifdef STORM_HAVE_CARL
            // template class DftToGspnTransformator<storm::RationalFunction>;
    #endif

        } // namespace dft
    } // namespace transformations
} // namespace storm


