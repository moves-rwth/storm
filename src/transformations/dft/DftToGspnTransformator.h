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
				
				std::map<int, std::vector<int> > mVoteAssociations; 			// Used to avoid multiple calculations for the same VOTE. 
				
				static constexpr const char* STR_FAILING = "_failing";			// Name standard for transitions that point towards a place, which in turn indicates the failure of a gate.
				static constexpr const char* STR_FAILED = "_failed";			// Name standard for place which indicates the failure of a gate.
				static constexpr const char* STR_FAILSAVING = "_failsaving";	// Name standard for transition that point towards a place, which in turn indicates the failsave state of a gate.
				static constexpr const char* STR_FAILSAVE = "_failsave";		// Name standard for place which indicates the failsave state of a gate.
                static constexpr const char* STR_ACTIVATED = "_activated";		// Name standard for place which indicates the activity.
				static constexpr const char* STR_ACTIVATING = "_activating";	// Name standard for transition that point towards a place, which in turn indicates its activity.
				
                /*!
                 * Write Gspn to file or console.
                 *
                 * @param toFile If true, the GSPN will be written to a file, otherwise it will
                                 be written to the console.
                 */
                void writeGspn(bool toFile);
				
				/*
				 * Draw all elements of the GSPN.
				 */
				void drawGSPNElements();
				
				/*
				 * Draw the connections between the elements of the GSPN.
				 */
				void drawGSPNConnections();
				
				/*
				 * Draw functional/probability dependencies into the GSPN.
				 */
				void drawGSPNDependencies();
				
				/*
				 * Draw restrictions between the elements of the GSPN (i.e. SEQ or MUTEX).
				 */
				void drawGSPNRestrictions();
				
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
				 * This PAND is inklusive (children are allowed to fail simultaneously and the PAND will fail nevertheless).
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
				 * This POR is inklusive (children are allowed to fail simultaneously and the POR will fail nevertheless).
				 * 
				 * @param dftPor The POR gate.
				 */
				void drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor);
				
				/*
				 * Draw a Petri net CONSTF (Constant Failure, a Basic Event that has already failed).
				 * 
				 * @param dftPor The CONSTF Basic Event.
				 */
				void drawCONSTF(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstF);
				
				/*
				 * Draw a Petri net CONSTS (Constant Save, a Basic Event that cannot fail).
				 * 
				 * @param dftPor The CONSTS Basic Event.
				 */
				void drawCONSTS(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstS);
				
				/*
				 * Draw a Petri net PDEP (FDEP is included with a firerate of 1).
				 */
				void drawPDEP(std::shared_ptr<storm::storage::DFTDependency<ValueType> const>(dftDependency));
				
				 /*
				  * Return true if BE is active (corresponding place contains one initial token) or false if BE is inactive (corresponding place contains no initial token).
				  * 
				  * @param dFTElement DFT element.
				  */
				 bool isBEActive(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dFTElement);
				 
				 /*
				  * Get the priority of the element.
				  * The priority is two times the length of the shortest path to the top event.
				  * 
				  * @param priority The priority of the gate. Top Event has priority 0,  its children 2, its grandchildren 4, ...
				  * 
				  * @param dftElement The element whose priority shall be determined.
				  */
				 uint_fast64_t getPriority(uint_fast64_t priority, std::shared_ptr<storm::storage::DFTElement<ValueType> const> dFTElement);
				 
				 /*
				  * Return all ids of BEs, that are successors of the given element and that are not the spare childs of a SPARE.
				  * 
				  * @param dftElement The element which 
				  */
				 std::vector<int> getAllBEIDsOfElement(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement);
            };
        }
    }
}

#endif /* DFTTOGSPNTRANSFORMATOR_H*/
