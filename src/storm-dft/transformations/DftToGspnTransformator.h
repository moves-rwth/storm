#pragma once

#include "storm-dft/storage/dft/DFT.h"
#include "storm-gspn/storage/gspn/GSPN.h"
#include "storm-gspn/storage/gspn/GspnBuilder.h"

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
                void transform(bool smart = true);

                /*!
                 * Extract Gspn by building
                 *
                 */
                gspn::GSPN* obtainGSPN();
                
                
                uint64_t toplevelFailedPlaceId();
                
            private:
                /*
				 * Draw all elements of the GSPN.
				 */
				void drawGSPNElements();

				/*
				 * Draw restrictions between the elements of the GSPN (i.e. SEQ or MUTEX).
				 */
				void drawGSPNRestrictions();
				
				/*
				 * Draw a Petri net Basic Event.
				 * 
				 * @param dftBE The Basic Event.
				 */
				void drawBE(std::shared_ptr<storm::storage::DFTBE<ValueType> const> dftBE, bool isRepresentative);
				
				/*
				 * Draw a Petri net AND.
				 * 
				 * @param dftAnd The AND gate.
				 */
				void drawAND(std::shared_ptr<storm::storage::DFTAnd<ValueType> const> dftAnd, bool isRepresentative);
				
				/*
				 * Draw a Petri net OR.
				 * 
				 * @param dftOr The OR gate.
				 */
				void drawOR(std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr, bool isRepresentative);
				
				/*
				 * Draw a Petri net VOT.
				 * 
				 * @param dftVot The VOT gate.
				 */
				void drawVOT(std::shared_ptr<storm::storage::DFTVot<ValueType> const> dftVot, bool isRepresentative);
				
				/*
				 * Draw a Petri net PAND. 
				 * This PAND is inklusive (children are allowed to fail simultaneously and the PAND will fail nevertheless).
				 * 
				 * @param dftPand The PAND gate.
				 */
				void drawPAND(std::shared_ptr<storm::storage::DFTPand<ValueType> const> dftPand, bool isRepresentative);
				
				/*
				 * Draw a Petri net SPARE.
				 * 
				 * @param dftSpare The SPARE gate.
				 */
				void drawSPARE(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare, bool isRepresentative);
				
				/*
				 * Draw a Petri net POR.
				 * This POR is inklusive (children are allowed to fail simultaneously and the POR will fail nevertheless).
				 * 
				 * @param dftPor The POR gate.
				 */
				void drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor, bool isRepresentative);
				
				/*
				 * Draw a Petri net CONSTF (Constant Failure, a Basic Event that has already failed).
				 * 
				 * @param dftPor The CONSTF Basic Event.
				 */
				void drawCONSTF(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstF, bool isRepresentative);
				
				/*
				 * Draw a Petri net CONSTS (Constant Save, a Basic Event that cannot fail).
				 * 
				 * @param dftPor The CONSTS Basic Event.
				 */
				void drawCONSTS(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstS, bool isRepresentative);
				
				/*
				 * Draw a Petri net PDEP (FDEP is included with a firerate of 1).
				 */
				void drawPDEP(std::shared_ptr<storm::storage::DFTDependency<ValueType> const> dftDependency);
				
                
                void drawSeq(std::shared_ptr<storm::storage::DFTSeq<ValueType> const> dftSeq);

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
                uint64_t getFailPriority(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dFTElement);

                uint64_t addUnavailableNode(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement, storm::gspn::LayoutInfo const& layoutInfo, bool initialAvailable = true);
                
                uint64_t addDisabledPlace(std::shared_ptr<storm::storage::DFTBE<ValueType> const> dftBe);
                
                storm::storage::DFT<ValueType> const& mDft;
                storm::gspn::GspnBuilder builder;
                std::vector<uint64_t> failedNodes;
                std::map<uint64_t, uint64_t> unavailableNodes;
                std::map<uint64_t, uint64_t> activeNodes;
                std::map<uint64_t, uint64_t> disabledNodes;
                bool smart;
                
                static constexpr const char* STR_FAILING = "_failing";			// Name standard for transitions that point towards a place, which in turn indicates the failure of a gate.
                static constexpr const char* STR_FAILED = "_failed";			// Name standard for place which indicates the failure of a gate.
                static constexpr const char* STR_FAILSAVING = "_failsaving";	// Name standard for transition that point towards a place, which in turn indicates the failsave state of a gate.
                static constexpr const char* STR_FAILSAVE = "_failsave";		// Name standard for place which indicates the failsave state of a gate.
                static constexpr const char* STR_ACTIVATING = "_activating";	// Name standard for transition that point towards a place, which in turn indicates its activity.
                static constexpr const char* STR_ACTIVATED = "_active";         // Name standard for place which indicates the activity.
            };
        }
    }
}

