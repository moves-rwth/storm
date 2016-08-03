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
				
				/*
				 * Draw a Petri net Basic Event.
				 * 
				 * @param name The name of the Basic Event.
				 * 
				 * @param activated If true, the Basic Event is activated. If false, the Basic Event
				 * 				is inactive, i.e. in a nested SPARE.
				 * 
				 * @param activeFailureRate The failure rate of the Basic Event, if it is activated.
				 * 
				 * @pparam passiveFailureRate The failure rate of the Basic Event, if it is not activated.
				 */
				void drawBE(std::string name, bool activated, double activeFailureRate, double passiveFailureRate);
				
				/*
				 * Draw a Petri net AND.
				 * 
				 * @param name The name of the AND.
				 */
				void drawAND(std::string name);
				
				/*
				 * Draw a Petri net OR.
				 * 
				 * @param name The name of the OR.
				 */
				void drawOR(std::string name, std::size_t numberOfChildren);
            };
        }
    }
}

#endif /* DFTTOGSPNTRANSFORMATOR_H*/
