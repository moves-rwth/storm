#include "src/transformations/dft/DftToGspnTransformator.h"
#include "src/exceptions/NotImplementedException.h"
#include <memory>

namespace storm {
    namespace transformations {
        namespace dft {

            template <typename ValueType>
            DftToGspnTransformator<ValueType>::DftToGspnTransformator(storm::storage::DFT<ValueType> const& dft) : mDft(dft) {
                // Intentionally left empty.
            }

            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::transform() {
                mGspn = storm::gspn::GSPN();
				mGspn.setName("DftToGspnTransformation");
				
				// Loop through every DFT element and draw them as a GSPN.
				drawGSPNElements();
				
				// When all DFT elements are drawn, draw the connections between them.
				drawGSPNConnections();
				
				// Draw restrictions into the GSPN (i.e. SEQ).
				drawGSPNRestrictions(); 
		
				// Write GSPN to file.
				writeGspn(true);
            }
            
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNElements() {
				// Loop through every DFT element and draw them as a GSPN.
				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
					auto dftElement = mDft.getElement(i);

					// Check which type the element is and call the corresponding drawing-function.
					switch (dftElement->type()) {
						case storm::storage::DFTElementType::AND:
							drawAND(std::static_pointer_cast<storm::storage::DFTAnd<ValueType> const>(dftElement));
							break;
						case storm::storage::DFTElementType::OR:
							drawOR(std::static_pointer_cast<storm::storage::DFTOr<ValueType> const>(dftElement));
							break;
						case storm::storage::DFTElementType::VOT:
							drawVOT(std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(dftElement));
							break;
						case storm::storage::DFTElementType::PAND:
							drawPAND(std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(dftElement));
							break;
						case storm::storage::DFTElementType::SPARE:
							drawSPARE(std::static_pointer_cast<storm::storage::DFTSpare<ValueType> const>(dftElement));
							break;
						case storm::storage::DFTElementType::POR:
							drawPOR(std::static_pointer_cast<storm::storage::DFTPor<ValueType> const>(dftElement));
							break;
						case storm::storage::DFTElementType::SEQ:
							// No method call needed here. SEQ only consists of restrictions, which are handled later.
							break;
						case storm::storage::DFTElementType::MUTEX:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a MUTEX is not yet implemented.");
							break;
						case storm::storage::DFTElementType::BE:
							drawBE(std::static_pointer_cast<storm::storage::DFTBE<ValueType> const>(dftElement));
							break;
						case storm::storage::DFTElementType::CONSTF:
							drawCONSTF(dftElement);
							break;
						case storm::storage::DFTElementType::CONSTS:
							drawCONSTS(dftElement);
							break;
						case storm::storage::DFTElementType::PDEP:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a PDEP is not yet implemented.");
							break;
						default:
							STORM_LOG_ASSERT(false, "DFT type unknown.");
							break;
					}
				}
			}
            
            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawBE(std::shared_ptr<storm::storage::DFTBE<ValueType> const> dftBE) {
				storm::gspn::Place placeBEActivated;
				placeBEActivated.setName(dftBE->name() + "_activated");
				placeBEActivated.setNumberOfInitialTokens(false ? 1 : 0); // TODO: Check if BE is spare child of a SPARE.
				mGspn.addPlace(placeBEActivated);
				
				storm::gspn::Place placeBEFailed;
				placeBEFailed.setName(dftBE->name() + STR_FAILED);
				placeBEFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeBEFailed);
				
				storm::gspn::TimedTransition<double> timedTransitionActiveFailure;
				timedTransitionActiveFailure.setName(dftBE->name() + "_activeFailing");
				timedTransitionActiveFailure.setPriority(1);
				timedTransitionActiveFailure.setRate(dftBE->activeFailureRate());
				timedTransitionActiveFailure.setInputArcMultiplicity(placeBEActivated, 1);
				timedTransitionActiveFailure.setInhibitionArcMultiplicity(placeBEFailed, 1);
				timedTransitionActiveFailure.setOutputArcMultiplicity(placeBEActivated, 1);
				timedTransitionActiveFailure.setOutputArcMultiplicity(placeBEFailed, 1);
				mGspn.addTimedTransition(timedTransitionActiveFailure);
				
				storm::gspn::TimedTransition<double> timedTransitionPassiveFailure;
				timedTransitionPassiveFailure.setName(dftBE->name() + "_passiveFailing");
				timedTransitionPassiveFailure.setPriority(1);
				timedTransitionPassiveFailure.setRate(dftBE->passiveFailureRate());
				timedTransitionPassiveFailure.setInhibitionArcMultiplicity(placeBEActivated, 1);
				timedTransitionPassiveFailure.setInhibitionArcMultiplicity(placeBEFailed, 1);
				timedTransitionPassiveFailure.setOutputArcMultiplicity(placeBEFailed, 1);
				mGspn.addTimedTransition(timedTransitionPassiveFailure);
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawAND(std::shared_ptr<storm::storage::DFTAnd<ValueType> const> dftAnd) {
				storm::gspn::Place placeANDFailed;
				placeANDFailed.setName(dftAnd->name() + STR_FAILED);
				placeANDFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeANDFailed);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionANDFailing;
				immediateTransitionANDFailing.setName(dftAnd->name() + STR_FAILING);
				immediateTransitionANDFailing.setPriority(1);
				immediateTransitionANDFailing.setWeight(0.0);
				immediateTransitionANDFailing.setInhibitionArcMultiplicity(placeANDFailed, 1);
				immediateTransitionANDFailing.setOutputArcMultiplicity(placeANDFailed, 1);
				mGspn.addImmediateTransition(immediateTransitionANDFailing);
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawOR(std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr) {
				storm::gspn::Place placeORFailed;
				placeORFailed.setName(dftOr->name() + STR_FAILED);
				placeORFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeORFailed);
				
				auto children = dftOr->children();
				for (std::size_t i = 0; i < dftOr->nrChildren(); i++) {
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionORFailing;
					immediateTransitionORFailing.setName(dftOr->name() + "_" + children[i]->name() + STR_FAILING);
					immediateTransitionORFailing.setPriority(1);
					immediateTransitionORFailing.setWeight(0.0);
					immediateTransitionORFailing.setInhibitionArcMultiplicity(placeORFailed, 1);
					immediateTransitionORFailing.setOutputArcMultiplicity(placeORFailed, 1);
					mGspn.addImmediateTransition(immediateTransitionORFailing);
				}
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawVOT(std::shared_ptr<storm::storage::DFTVot<ValueType> const> dftVot) {
				storm::gspn::Place placeVOTFailed;
				placeVOTFailed.setName(dftVot->name() + STR_FAILED);
				placeVOTFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeVOTFailed);
				
				// Calculate, how many immediate transitions are necessary and draw the needed number.
				for (int i = 0; i < calculateBinomialCoefficient(dftVot->nrChildren(), dftVot->threshold()); i++) {
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionVOTFailing;
					immediateTransitionVOTFailing.setName(dftVot->name() + "_" + std::to_string(i) + STR_FAILING);
					immediateTransitionVOTFailing.setPriority(1);
					immediateTransitionVOTFailing.setWeight(0.0);
					immediateTransitionVOTFailing.setInhibitionArcMultiplicity(placeVOTFailed, 1);
					immediateTransitionVOTFailing.setOutputArcMultiplicity(placeVOTFailed, 1);
					mGspn.addImmediateTransition(immediateTransitionVOTFailing);
				}
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPAND(std::shared_ptr<storm::storage::DFTPand<ValueType> const> dftPand) {				
				storm::gspn::Place placePANDFailed;
				placePANDFailed.setName(dftPand->name() + STR_FAILED);
				placePANDFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placePANDFailed);
				
				storm::gspn::Place placePANDFailsave;
				placePANDFailsave.setName(dftPand->name() + STR_FAILSAVE);
				placePANDFailsave.setNumberOfInitialTokens(0);
				mGspn.addPlace(placePANDFailsave);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPANDFailing;
				immediateTransitionPANDFailing.setName(dftPand->name() + STR_FAILING);
				immediateTransitionPANDFailing.setPriority(1);
				immediateTransitionPANDFailing.setWeight(0.0);
				immediateTransitionPANDFailing.setInhibitionArcMultiplicity(placePANDFailed, 1);
				immediateTransitionPANDFailing.setInhibitionArcMultiplicity(placePANDFailsave, 1);
				immediateTransitionPANDFailing.setOutputArcMultiplicity(placePANDFailed, 1);
				mGspn.addImmediateTransition(immediateTransitionPANDFailing);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPANDFailsave;
				immediateTransitionPANDFailsave.setName(dftPand->name() + STR_FAILSAVING);
				immediateTransitionPANDFailsave.setPriority(1);
				immediateTransitionPANDFailsave.setWeight(0.0);
				immediateTransitionPANDFailsave.setInhibitionArcMultiplicity(placePANDFailsave, 1);
				immediateTransitionPANDFailsave.setOutputArcMultiplicity(placePANDFailsave, 1);
				mGspn.addImmediateTransition(immediateTransitionPANDFailsave);
				
				// TODO: Extend for more than 2 children.
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawSPARE(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare) {
				STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a SPARE is not yet implemented.");
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor) {
				storm::gspn::Place placePORFailed;
				placePORFailed.setName(dftPor->name() + STR_FAILED);
				placePORFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placePORFailed);
				
				storm::gspn::Place placePORFailsave;
				placePORFailsave.setName(dftPor->name() + STR_FAILSAVE);
				placePORFailsave.setNumberOfInitialTokens(0);
				mGspn.addPlace(placePORFailsave);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPORFailing;
				immediateTransitionPORFailing.setName(dftPor->name() + STR_FAILING);
				immediateTransitionPORFailing.setPriority(1);
				immediateTransitionPORFailing.setWeight(0.0);
				immediateTransitionPORFailing.setInhibitionArcMultiplicity(placePORFailed, 1);
				immediateTransitionPORFailing.setInhibitionArcMultiplicity(placePORFailsave, 1);
				immediateTransitionPORFailing.setOutputArcMultiplicity(placePORFailed, 1);
				mGspn.addImmediateTransition(immediateTransitionPORFailing);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPORFailsave;
				immediateTransitionPORFailsave.setName(dftPor->name() + STR_FAILSAVING);
				immediateTransitionPORFailsave.setPriority(1);
				immediateTransitionPORFailsave.setWeight(0.0);
				immediateTransitionPORFailsave.setInhibitionArcMultiplicity(placePORFailsave, 1);
				immediateTransitionPORFailsave.setOutputArcMultiplicity(placePORFailsave, 1);
				mGspn.addImmediateTransition(immediateTransitionPORFailsave);
				
				// TODO: Extend for more than 2 children.
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTF(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstF) {
				storm::gspn::Place placeCONSTFFailed;
				placeCONSTFFailed.setName(dftConstF->name() + STR_FAILED);
				placeCONSTFFailed.setNumberOfInitialTokens(1);
				mGspn.addPlace(placeCONSTFFailed);
				
				// TODO: Not tested because there is no corresponding DFT element yet.
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTS(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstS) {
				storm::gspn::Place placeCONSTSFailed;
				placeCONSTSFailed.setName(dftConstS->name() + STR_FAILED);
				placeCONSTSFailed.setNumberOfInitialTokens(0);
				placeCONSTSFailed.setCapacity(0); // It cannot contain a token, because it cannot fail.
				mGspn.addPlace(placeCONSTSFailed);
				
				// TODO: Not tested because there is no corresponding DFT element yet.
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNConnections() {
				// Check for every element, if they have parents (all will have at least 1, except the top event). 
				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
					auto child = mDft.getElement(i);
					auto parents = child->parentIds();
					
					// Draw a connection to every parent.
					for (std::size_t j = 0; j < parents.size(); j++) {	
						// Check the type of the parent and act accordingly (every parent gate has different entry points...).
						switch (mDft.getElement(parents[j])->type()) {
							case storm::storage::DFTElementType::AND:
							{
								auto andEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILING);
								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
								if (andEntry.first && childExit.first) { // Only add arcs if the objects have been found.
									andEntry.second->setInputArcMultiplicity(childExit.second, 1);
									andEntry.second->setOutputArcMultiplicity(childExit.second, 1);
								}
								break;
							}
							case storm::storage::DFTElementType::OR:
							{
								auto orEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + child->name() + STR_FAILING);
								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
								if (orEntry.first && childExit.first) { // Only add arcs if the objects have been found.
									orEntry.second->setInputArcMultiplicity(childExit.second, 1);
									orEntry.second->setOutputArcMultiplicity(childExit.second, 1);
								}
								break;
							}
							case storm::storage::DFTElementType::VOT:
							{
								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
								if (childExit.first) {
									// Get all associations of the child to all immediate transitions of the VOTE.
									auto children = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(mDft.getElement(parents[j]))->children();
									auto associations = getVOTEEntryAssociation(parents[j], child->id(), 
										std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(mDft.getElement(parents[j]))->threshold(), children);
									
									// Draw.
									for (std::size_t k = 0; k < associations.size(); k++) {
										auto voteEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + std::to_string(associations[k]) + STR_FAILING);
										if (voteEntry.first) {
											voteEntry.second->setInputArcMultiplicity(childExit.second, 1);
											voteEntry.second->setOutputArcMultiplicity(childExit.second, 1);
										}
									}
								}
								break;
							}
							case storm::storage::DFTElementType::PAND:
							{
								auto children =  std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(mDft.getElement(parents[j]))->children();
								auto pandEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILING);
								auto pandEntry2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILSAVING);
								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
								
								if (pandEntry.first && pandEntry2.first && childExit.first) { // Only add arcs if the objects have been found.
									if (children[0] == child) { // Current element is primary child.
										pandEntry.second->setInputArcMultiplicity(childExit.second, 1);
										pandEntry.second->setOutputArcMultiplicity(childExit.second, 1);
										pandEntry2.second->setInhibitionArcMultiplicity(childExit.second, 1);
										
									}
									else if (children[1] == child) { // Current element is secondary child.
										pandEntry.second->setInputArcMultiplicity(childExit.second, 1);
										pandEntry.second->setOutputArcMultiplicity(childExit.second, 1);
										pandEntry2.second->setInputArcMultiplicity(childExit.second, 1);
										pandEntry2.second->setOutputArcMultiplicity(childExit.second, 1);
									}
								}
								
								break;
							}
							case storm::storage::DFTElementType::SPARE:
								break;
							case storm::storage::DFTElementType::POR:
							{
								auto children =  std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(mDft.getElement(parents[j]))->children();
								auto porEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILING);
								auto porEntry2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILSAVING);
								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
								
								if (porEntry.first && porEntry2.first && childExit.first) { // Only add arcs if the objects have been found.
									if (children[0] == child) { // Current element is primary child.
										porEntry.second->setInputArcMultiplicity(childExit.second, 1);
										porEntry.second->setOutputArcMultiplicity(childExit.second, 1);
										porEntry2.second->setInhibitionArcMultiplicity(childExit.second, 1);
										
									}
									else if (children[1] == child) { // Current element is secondary child.
										porEntry2.second->setInputArcMultiplicity(childExit.second, 1);
										porEntry2.second->setOutputArcMultiplicity(childExit.second, 1);
									}
								}
								
								break;
							}
							case storm::storage::DFTElementType::SEQ:
							{
								// Sequences are realized with restrictions. Nothing to do here.
								break;
							}
							case storm::storage::DFTElementType::MUTEX:
								break;
							case storm::storage::DFTElementType::BE:
							{
								// The parent is never a Basic Event (Except for SPAREs?).
								break;
							}
							case storm::storage::DFTElementType::CONSTF:
							{
								// The parent is never a Basic Event (Except for SPAREs?).
								break;
							}
							case storm::storage::DFTElementType::CONSTS:
							{
								// The parent is never a Basic Event (Except for SPAREs?).
								break;
							}
							case storm::storage::DFTElementType::PDEP:
								break;
							default:
							{
								STORM_LOG_ASSERT(false, "DFT type unknown.");
								break;
							}
						}
					}
				}
			}
			
			template <typename ValueType>
            int DftToGspnTransformator<ValueType>::calculateBinomialCoefficient(int n, int k) {
				STORM_LOG_ASSERT(n >= k, "k is not allowed to be larger than n.");
				
				return factorialFunction(n) / ( factorialFunction(n - k) * factorialFunction(k) );
            }
            
           	template <typename ValueType>
            int DftToGspnTransformator<ValueType>::factorialFunction(int n) {
				return (n == 1 || n == 0) ? 1 : factorialFunction(n - 1) * n;
            }
            
           	template <typename ValueType>
            std::vector<int> DftToGspnTransformator<ValueType>::getVOTEEntryAssociation(int parentId, int childId, int threshold, std::vector<std::shared_ptr<storm::storage::DFTElement<ValueType>>> children) {
				// Fetch all ids of the children.
				std::vector<int> childrenIds(children.size());
				for (std::size_t i = 0; i < children.size(); i++) {
					childrenIds[i] = children[i]->id();
				}
				
				// Get all subsets of the 'children' of size 'threshold'.
				std::vector<int> subsets(threshold);
				std::vector<int> output;
				
				// Check if output for this VOTE already exists. If yes, use it instead recalculating.
				if (mVoteAssociations.find(parentId) == mVoteAssociations.end()) { // Could not find parentId in map.
					combinationUtil(output, childrenIds, subsets, 0, children.size() - 1, 0, threshold);
					mVoteAssociations.insert ( std::pair<int, std::vector<int> > (parentId, output) );
				}
				else { // Could find parentId in map, use already computed output.
					output = mVoteAssociations.find(parentId)->second;
				}
				
				// Check which subset contains the id 'childId' and add the subset-number to the association.
				std::vector<int> associations;
				for (std::size_t i = 0; i < output.size(); i++) {
					if (childId == output[i]) {
						associations.push_back(i / threshold);
					}
				}
				
				return associations;
            }
            
            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::combinationUtil(std::vector<int> &output, std::vector<int> childrenIds, std::vector<int> subsets, int start, int end, int index, int threshold)
			{
				if (index == threshold)
				{
					for (int j = 0; j < threshold; j++) {
						output.push_back(subsets[j]);
					}
					
					return;
				}
			
				for (int i = start; i <= end && end - i + 1 >= threshold - index; i++)
				{
					subsets[index] = childrenIds[i];
					combinationUtil(output, childrenIds, subsets, i + 1, end, index + 1, threshold);
				}
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNRestrictions() {
				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
					auto dftElement = mDft.getElement(i);
					
					if (dftElement->isRestriction()) {
						switch (dftElement->type()) {
							case storm::storage::DFTElementType::SEQ:
							{
								auto children = mDft.getRestriction(i)->children();
								
								for (std::size_t j = 0; j < children.size() - 1; j++) {
									auto suppressor = mGspn.getPlace(children[j]->name() + STR_FAILED);
									
									switch (children[j + 1]->type()) {
										case storm::storage::DFTElementType::BE: // If suppressed is a BE, add 2 arcs to timed transitions.
										{
											auto suppressedActive = mGspn.getTimedTransition(children[j + 1]->name() + "_activeFailing");
											auto suppressedPassive = mGspn.getTimedTransition(children[j + 1]->name() + "_passiveFailing");
											
											if (suppressor.first && suppressedActive.first && suppressedPassive.first) { // Only add arcs if the objects have been found.
												suppressedActive.second->setInputArcMultiplicity(suppressor.second, 1);
												suppressedActive.second->setOutputArcMultiplicity(suppressor.second, 1);
												suppressedPassive.second->setInputArcMultiplicity(suppressor.second, 1);
												suppressedPassive.second->setOutputArcMultiplicity(suppressor.second, 1);
											}
											break;
										}
										default: // If supressed is not a BE, add single arc to immediate transition.
										{
											auto suppressed = mGspn.getImmediateTransition(children[j + 1]->name() + STR_FAILING);
											
											if (suppressor.first && suppressed.first) { // Only add arcs if the objects have been found.
												suppressed.second->setInputArcMultiplicity(suppressor.second, 1);
												suppressed.second->setOutputArcMultiplicity(suppressor.second, 1);
											}
											break;
										}
									}
								}
								break;
							}
							default:
							{
								// TODO: Are there other restrictions than SEQ?
								break;
							}
						}
					}
				}
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


