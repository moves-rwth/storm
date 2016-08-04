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
				 * @param dftBE The Basic Event.
				 */
				void drawBE(std::shared_ptr<storm::storage::DFTBE<ValueType> const> dftBE);
				
				/*
				 * Draw a Petri net AND.
				 * 
				 * @param dftAnd The AND gate.
				 */
				void drawAND(std::shared_ptr<storm::storage::DFTAnd<ValueType> const> dftAnd);
				
				/*
				 * Draw a Petri net OR.
				 * 
				 * @param dftOr The OR gate.
				 */
				void drawOR(std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr);
				
				/*
				 * Draw a Petri net VOT.
				 * 
				 * @param dftVot The VOT gate.
				 */
				void drawVOT(std::shared_ptr<storm::storage::DFTVot<ValueType> const> dftVot);
				
				/*
				 * Draw a Petri net PAND.
				 * 
				 * @param dftPand The PAND gate.
				 */
				void drawPAND(std::shared_ptr<storm::storage::DFTPand<ValueType> const> dftPand);
				
				/*
				 * Draw a Petri net SPARE.
				 * 
				 * @param dftSpare The SPARE gate.
				 */
				void drawSPARE(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare);
				
				/*
				 * Draw a Petri net POR.
				 * 
				 * @param dftPor The POR gate.
				 */
				void drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor);
            };
        }
    }
}

#endif /* DFTTOGSPNTRANSFORMATOR_H*/
