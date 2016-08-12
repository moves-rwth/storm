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
				
				static constexpr const char* STR_FAILING = "_failing";			// Name standard for transitions that point towards a place, which in turn indicates the failure of a gate.
				static constexpr const char* STR_FAILED = "_failed";			// Name standard for place which indicates the failure of a gate.
                
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
				 * Calculate the binomial coefficient:
				 * n! / ( (n - k)! * k!)
				 * 
				 * @param n First parameter of binomial coefficient.
				 * 
				 * @ param k Second parameter of binomial coefficient.
				 * 
				 */
				int calculateBinomialCoefficient(int n, int k);
				
				/*
				 * Calculate the factorial function of n.
				 * 
				 * @param n The parameter for the factorial function, i.e. n!.
				 * 
				 */
				int factorialFunction(int n);
				
				/*
				 * Return the immediate Transition numbers of the VOTE with which the child has to be connected.
				 * 
				 * Example: A VOTE2/3 gate has three children BEs: {A, B, C}.
				 * The subsets of size 2 of {A, B, C} are {{A, B}, {A, C}, {B, C}}. 
				 * 'A' is contained in subset 0 and subset 1, so this method returns {0, 1}. 
				 * This means that BE 'A' needs to be connected with the immediate transitions 'VOTE_0_failing' and 'VOTE_1_failing'.
				 * 
				 * @param childId Id of the child.
				 * 
				 * @param threshold The threshold of the VOTE.
				 * 
				 * @param children All children of the VOTE.
				 */
				 std::vector<int> getVOTEEntryAssociation(int childId, int threshold, std::vector<std::shared_ptr<storm::storage::DFTElement<ValueType>>> children);
				 
				 /*
				  * Helper-method for getVOTEEntryAssociation().
				  * Obtained from / more info at:
				  * http://www.geeksforgeeks.org/print-all-possible-combinations-of-r-elements-in-a-given-array-of-size-n/
				  */
				 void combinationUtil(std::vector<int> &output, std::vector<int> childrenIds, std::vector<int> subsets, int start, int end, int index, int threshold);
            };
        }
    }
}

#endif /* DFTTOGSPNTRANSFORMATOR_H*/
