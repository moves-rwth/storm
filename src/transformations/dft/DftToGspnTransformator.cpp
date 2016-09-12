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
				
				// Draw functional/probability dependencies into the GSPN.
				drawGSPNDependencies();
				
				// Draw restrictions into the GSPN (i.e. SEQ or MUTEX).
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
							// No method call needed here. MUTEX only consists of restrictions, which are handled later.
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
							drawPDEP(std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(dftElement));
							break;
						default:
							STORM_LOG_ASSERT(false, "DFT type unknown.");
							break;
					}
				}
			}
            
            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawBE(std::shared_ptr<storm::storage::DFTBE<ValueType> const> dftBE) {
				uint_fast64_t priority = getPriority(0, dftBE);
				
				storm::gspn::Place placeBEActivated;
				placeBEActivated.setName(dftBE->name() + STR_ACTIVATED);
				placeBEActivated.setNumberOfInitialTokens(isBEActive(dftBE) ? 1 : 0);
				mGspn.addPlace(placeBEActivated);
				
				storm::gspn::Place placeBEFailed;
				placeBEFailed.setName(dftBE->name() + STR_FAILED);
				placeBEFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeBEFailed);
				
				storm::gspn::TimedTransition<double> timedTransitionActiveFailure;
				timedTransitionActiveFailure.setName(dftBE->name() + "_activeFailing");
				timedTransitionActiveFailure.setPriority(priority);
				timedTransitionActiveFailure.setRate(dftBE->activeFailureRate());
				timedTransitionActiveFailure.setInputArcMultiplicity(placeBEActivated, 1);
				timedTransitionActiveFailure.setInhibitionArcMultiplicity(placeBEFailed, 1);
				timedTransitionActiveFailure.setOutputArcMultiplicity(placeBEActivated, 1);
				timedTransitionActiveFailure.setOutputArcMultiplicity(placeBEFailed, 1);
				mGspn.addTimedTransition(timedTransitionActiveFailure);
				
				storm::gspn::TimedTransition<double> timedTransitionPassiveFailure;
				timedTransitionPassiveFailure.setName(dftBE->name() + "_passiveFailing");
				timedTransitionPassiveFailure.setPriority(priority);
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
				immediateTransitionANDFailing.setPriority(getPriority(0, dftAnd));
				immediateTransitionANDFailing.setWeight(0.0);
				immediateTransitionANDFailing.setInhibitionArcMultiplicity(placeANDFailed, 1);
				immediateTransitionANDFailing.setOutputArcMultiplicity(placeANDFailed, 1);
				mGspn.addImmediateTransition(immediateTransitionANDFailing);
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawOR(std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr) {
				uint_fast64_t priority = getPriority(0, dftOr);
				
				storm::gspn::Place placeORFailed;
				placeORFailed.setName(dftOr->name() + STR_FAILED);
				placeORFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeORFailed);
				
				auto children = dftOr->children();
				for (std::size_t i = 0; i < dftOr->nrChildren(); i++) {
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionORFailing;
					immediateTransitionORFailing.setName(dftOr->name() + "_" + children[i]->name() + STR_FAILING);
					immediateTransitionORFailing.setPriority(priority);
					immediateTransitionORFailing.setWeight(0.0);
					immediateTransitionORFailing.setInhibitionArcMultiplicity(placeORFailed, 1);
					immediateTransitionORFailing.setOutputArcMultiplicity(placeORFailed, 1);
					mGspn.addImmediateTransition(immediateTransitionORFailing);
				}
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawVOT(std::shared_ptr<storm::storage::DFTVot<ValueType> const> dftVot) {
				uint_fast64_t priority = getPriority(0, dftVot);
				
				storm::gspn::Place placeVOTFailed;
				placeVOTFailed.setName(dftVot->name() + STR_FAILED);
				placeVOTFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeVOTFailed);
				
				// Calculate, how many immediate transitions are necessary and draw the needed number.
				for (int i = 0; i < calculateBinomialCoefficient(dftVot->nrChildren(), dftVot->threshold()); i++) {
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionVOTFailing;
					immediateTransitionVOTFailing.setName(dftVot->name() + "_" + std::to_string(i) + STR_FAILING);
					immediateTransitionVOTFailing.setPriority(priority);
					immediateTransitionVOTFailing.setWeight(0.0);
					immediateTransitionVOTFailing.setInhibitionArcMultiplicity(placeVOTFailed, 1);
					immediateTransitionVOTFailing.setOutputArcMultiplicity(placeVOTFailed, 1);
					mGspn.addImmediateTransition(immediateTransitionVOTFailing);
				}
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPAND(std::shared_ptr<storm::storage::DFTPand<ValueType> const> dftPand) {
				uint_fast64_t priority = getPriority(0, dftPand);
				
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
				immediateTransitionPANDFailing.setPriority(priority);
				immediateTransitionPANDFailing.setWeight(0.0);
				immediateTransitionPANDFailing.setInhibitionArcMultiplicity(placePANDFailed, 1);
				immediateTransitionPANDFailing.setInhibitionArcMultiplicity(placePANDFailsave, 1);
				immediateTransitionPANDFailing.setOutputArcMultiplicity(placePANDFailed, 1);
				mGspn.addImmediateTransition(immediateTransitionPANDFailing);
				
				for (std::size_t i = 0; i < dftPand->nrChildren() -1; i++) {
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPANDFailsave;
					immediateTransitionPANDFailsave.setName(dftPand->name() + "_" + std::to_string(i) + STR_FAILSAVING);
					immediateTransitionPANDFailsave.setPriority(priority);
					immediateTransitionPANDFailsave.setWeight(0.0);
					immediateTransitionPANDFailsave.setInhibitionArcMultiplicity(placePANDFailsave, 1);
					immediateTransitionPANDFailsave.setOutputArcMultiplicity(placePANDFailsave, 1);
					mGspn.addImmediateTransition(immediateTransitionPANDFailsave);
				}
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawSPARE(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare) {
				// TODO: Implement.
				
				uint_fast64_t priority = getPriority(0, dftSpare);
				
				storm::gspn::Place placeSPAREActivated;
				placeSPAREActivated.setName(dftSpare->name() + STR_ACTIVATED);
				placeSPAREActivated.setNumberOfInitialTokens(isBEActive(dftSpare));
				mGspn.addPlace(placeSPAREActivated);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPCActivating;
				immediateTransitionPCActivating.setName(dftSpare->children()[0]->name() + STR_ACTIVATING);
				immediateTransitionPCActivating.setPriority(priority);
				immediateTransitionPCActivating.setWeight(0.0);
				immediateTransitionPCActivating.setInputArcMultiplicity(placeSPAREActivated, 1);
				immediateTransitionPCActivating.setOutputArcMultiplicity(placeSPAREActivated, 1);
				mGspn.addImmediateTransition(immediateTransitionPCActivating);
				
				auto children = dftSpare->children();
				
				// Repeat for every spare child (every child that is not the first!).
				for (std::size_t i = 1; i < children.size(); i++) {
					auto placeChildClaimedPreexist = mGspn.getPlace(children[i]->name() + "_claimed");
					
					if (!placeChildClaimedPreexist.first) { // Only draw this place if it doesn't exist jet.
						storm::gspn::Place placeChildClaimed;
						placeChildClaimed.setName(children[i]->name() + "_claimed");
						placeChildClaimed.setNumberOfInitialTokens(0);
						mGspn.addPlace(placeChildClaimed);
						
						storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionSpareChildActivating;
						immediateTransitionSpareChildActivating.setName(dftSpare->children()[i]->name() + STR_ACTIVATING);
						immediateTransitionSpareChildActivating.setPriority(priority);
						immediateTransitionSpareChildActivating.setWeight(0.0);
						immediateTransitionSpareChildActivating.setInputArcMultiplicity(placeChildClaimed, 1);
						immediateTransitionSpareChildActivating.setOutputArcMultiplicity(placeChildClaimed, 1);
						mGspn.addImmediateTransition(immediateTransitionSpareChildActivating);
					}
					
					auto placeChildClaimedExist = mGspn.getPlace(children[i]->name() + "_claimed");
					
					storm::gspn::Place placeSPAREClaimedChild;
					placeSPAREClaimedChild.setName(dftSpare->name() + "_claimed_" + children[i]->name());
					placeSPAREClaimedChild.setNumberOfInitialTokens(0);
					mGspn.addPlace(placeSPAREClaimedChild);
					
					storm::gspn::Place placeSPAREChildConsumed;
					placeSPAREChildConsumed.setName(children[i]->name() + "_consumed"); // TODO: If its the last child, this label must be "SPARE_failed".
					placeSPAREChildConsumed.setNumberOfInitialTokens(0);
					mGspn.addPlace(placeSPAREChildConsumed);
					
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionChildConsuming1;
					immediateTransitionChildConsuming1.setName(dftSpare->children()[i]->name() + "_consuming1");
					immediateTransitionChildConsuming1.setPriority(priority);
					immediateTransitionChildConsuming1.setWeight(0.0);
					immediateTransitionChildConsuming1.setOutputArcMultiplicity(placeSPAREChildConsumed, 1);
					immediateTransitionChildConsuming1.setInhibitionArcMultiplicity(placeSPAREChildConsumed, 1);
					immediateTransitionChildConsuming1.setInhibitionArcMultiplicity(placeSPAREClaimedChild, 1);
					mGspn.addImmediateTransition(immediateTransitionChildConsuming1);
					
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionChildConsuming2;
					immediateTransitionChildConsuming2.setName(dftSpare->children()[i]->name() + "_consuming2");
					immediateTransitionChildConsuming2.setPriority(priority);
					immediateTransitionChildConsuming2.setWeight(0.0);
					immediateTransitionChildConsuming2.setOutputArcMultiplicity(placeSPAREChildConsumed, 1);
					immediateTransitionChildConsuming2.setInhibitionArcMultiplicity(placeSPAREChildConsumed, 1);
					immediateTransitionChildConsuming2.setOutputArcMultiplicity(placeSPAREClaimedChild, 1);
					immediateTransitionChildConsuming2.setInputArcMultiplicity(placeSPAREClaimedChild, 1);
					mGspn.addImmediateTransition(immediateTransitionChildConsuming2);
				}
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor) {
				uint_fast64_t priority = getPriority(0, dftPor);
				
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
				immediateTransitionPORFailing.setPriority(priority);
				immediateTransitionPORFailing.setWeight(0.0);
				immediateTransitionPORFailing.setInhibitionArcMultiplicity(placePORFailed, 1);
				immediateTransitionPORFailing.setInhibitionArcMultiplicity(placePORFailsave, 1);
				immediateTransitionPORFailing.setOutputArcMultiplicity(placePORFailed, 1);
				mGspn.addImmediateTransition(immediateTransitionPORFailing);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPORFailsave;
				immediateTransitionPORFailsave.setName(dftPor->name() + STR_FAILSAVING);
				immediateTransitionPORFailsave.setPriority(priority);
				immediateTransitionPORFailsave.setWeight(0.0);
				immediateTransitionPORFailsave.setInhibitionArcMultiplicity(placePORFailsave, 1);
				immediateTransitionPORFailsave.setOutputArcMultiplicity(placePORFailsave, 1);
				mGspn.addImmediateTransition(immediateTransitionPORFailsave);
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTF(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstF) {
				storm::gspn::Place placeCONSTFFailed;
				placeCONSTFFailed.setName(dftConstF->name() + STR_FAILED);
				placeCONSTFFailed.setNumberOfInitialTokens(1);
				mGspn.addPlace(placeCONSTFFailed);
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTS(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstS) {
				storm::gspn::Place placeCONSTSFailed;
				placeCONSTSFailed.setName(dftConstS->name() + STR_FAILED);
				placeCONSTSFailed.setNumberOfInitialTokens(0);
				placeCONSTSFailed.setCapacity(0); // It cannot contain a token, because it cannot fail.
				mGspn.addPlace(placeCONSTSFailed);
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPDEP(std::shared_ptr<storm::storage::DFTDependency<ValueType> const>(dftDependency)) {
				// Only draw dependency, if it wasn't drawn before.
				std::string gateName = dftDependency->name().substr(0, dftDependency->name().find("_"));
				auto exists = mGspn.getPlace(gateName + STR_FAILED);
				if (!exists.first) {
					storm::gspn::Place placeDEPFailed;
					placeDEPFailed.setName(gateName + STR_FAILED);
					placeDEPFailed.setNumberOfInitialTokens(0);
					mGspn.addPlace(placeDEPFailed);
					
					storm::gspn::TimedTransition<double> timedTransitionDEPFailure;
					timedTransitionDEPFailure.setName(gateName + STR_FAILING);
					timedTransitionDEPFailure.setPriority(getPriority(0, dftDependency));
					timedTransitionDEPFailure.setRate(dftDependency->probability());
					timedTransitionDEPFailure.setOutputArcMultiplicity(placeDEPFailed, 1);
					timedTransitionDEPFailure.setInhibitionArcMultiplicity(placeDEPFailed, 1);
					mGspn.addTimedTransition(timedTransitionDEPFailure);
				}
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
								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
								auto pandEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILING);
								
								if (childExit.first && pandEntry.first) { // Only add arcs if the objects have been found.
									pandEntry.second->setInputArcMultiplicity(childExit.second, 1);
									pandEntry.second->setOutputArcMultiplicity(childExit.second, 1);
									
									if (children[0] == child) { // Current element is primary child.
										auto pandEntry2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_0" + STR_FAILSAVING);
										
										if (pandEntry2.first) {
											pandEntry2.second->setInhibitionArcMultiplicity(childExit.second, 1);
										}
									}
									else { // Current element is not the primary child.
										for (std::size_t k = 1; k < children.size(); k++) {
											if (children[k] == child) {
												auto pandEntry2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + std::to_string((k - 1)) + STR_FAILSAVING);
												auto pandEntry3 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + std::to_string((k)) + STR_FAILSAVING);
												
												if (pandEntry2.first) {
													pandEntry2.second->setInputArcMultiplicity(childExit.second, 1);
													pandEntry2.second->setOutputArcMultiplicity(childExit.second, 1);
												}
												
												if (pandEntry3.first) {
													pandEntry3.second->setInhibitionArcMultiplicity(childExit.second, 1);
												}
												
												continue;
											}
										}
									}
								}
								
								break;
							}
							case storm::storage::DFTElementType::SPARE:
							{
								// TODO: Implement.
								
								// Check if child is a primary or spare child.
								auto children =  std::static_pointer_cast<storm::storage::DFTSpare<ValueType> const>(mDft.getElement(parents[j]))->children();
								
								if (child == children[0]) { // Primary child.
									// TODO: Draw line from "FC_activating" to every BE, that is connected to the primary child.
								}
								else { // Spare child.
									
								}
								
								break;
							}
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
									else { // Current element is not the primary child.
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
							{
								// MUTEX are realized with restrictions. Nothing to do here.
								break;
							}
							case storm::storage::DFTElementType::BE:
							{
								// The parent is never a Basic Event.
								break;
							}
							case storm::storage::DFTElementType::CONSTF:
							{
								// The parent is never a Basic Event.
								break;
							}
							case storm::storage::DFTElementType::CONSTS:
							{
								// The parent is never a Basic Event.
								break;
							}
							case storm::storage::DFTElementType::PDEP:
							{
								// The parent is never a DEP. Hence the connections must be drawn somewhere else.
								break;
							}
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
            bool DftToGspnTransformator<ValueType>::isBEActive(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement)
			{
				// TODO: This method must be tested again after SPAREs are implemented.
				
				// If element is the top element, return true.
				if (dftElement->id() == mDft.getTopLevelIndex()) {
					return true;
				}
				else { // Else look at all parents.
					auto parents = dftElement->parents();
					std::vector<bool> pathValidities;
					
					for (std::size_t i = 0; i < parents.size(); i++) {
						// Add all parents to the vector, except if the parent is a SPARE and the current element is an inactive child of the SPARE.
						if (parents[i]->type() == storm::storage::DFTElementType::SPARE) {
							auto children = std::static_pointer_cast<storm::storage::DFTSpare<ValueType> const>(parents[i])->children();
							if (children[0]->id() != dftElement->id()) {
								continue;
							}
						}
						
						pathValidities.push_back(isBEActive(parents[i]));
					}
					
					// Check all vector entries. If one is true, a "valid" path has been found.
					for (std::size_t i = 0; i < pathValidities.size(); i++) {
						if (pathValidities[i]) {
							return true;
						}
					}
				}
				
				// No "valid" path found. BE is inactive.
				return false;
			}
			
			template <typename ValueType>
            uint_fast64_t DftToGspnTransformator<ValueType>::getPriority(uint_fast64_t priority, std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement)
			{
				// If element is the top element, return current priority.
				if (dftElement->id() == mDft.getTopLevelIndex()) {
					return priority;
				}
				else { // Else look at all parents.
					auto parents = dftElement->parents();
					std::vector<uint_fast64_t> pathLengths;
					
					// If the element has no parents, return.
					if (parents.size() == 0) {
						return UINT_FAST64_MAX / 2; // High enough value so that this priority is never used as the shortest path to the top event.
					}
					
					// Check priorities of all parents.
					for (std::size_t i = 0; i < parents.size(); i++) {
						pathLengths.push_back(getPriority(priority + 2, parents[i]));
					}
					
					// And only use the path to the parent with the lowest priority.
					return *std::min_element(pathLengths.begin(), pathLengths.end());
				}
				
				return priority;
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNDependencies() {
				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
					auto dftElement = mDft.getElement(i);
					
					if (dftElement->isDependency()) {
						std::string gateName = dftElement->name().substr(0, dftElement->name().find("_"));
						auto depEntry = mGspn.getTimedTransition(gateName + STR_FAILING);
						auto trigger = mGspn.getPlace(std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(dftElement)->nameTrigger() + STR_FAILED);
						
						if (depEntry.first && trigger.first) { // Only add arcs if the objects have been found.
							if (!depEntry.second->existsInputArc(trigger.second)) {
								depEntry.second->setInputArcMultiplicity(trigger.second, 1);
							}
							if (!depEntry.second->existsOutputArc(trigger.second)){
								depEntry.second->setOutputArcMultiplicity(trigger.second, 1);
							}
						}
						
						auto dependent = mGspn.getPlace(std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(dftElement)->nameDependent() + STR_FAILED);
						
						if (dependent.first) { // Only add arcs if the objects have been found.
							depEntry.second->setOutputArcMultiplicity(dependent.second, 1);
						}
					}
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
							case storm::storage::DFTElementType::MUTEX:
							{
								// MUTEX is not implemented by the DFTGalileoParser yet. Nothing to do here.
								STORM_LOG_ASSERT(false, "MUTEX is not supported by DftToGspnTransformator.");
								break;
							}
							default:
							{
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


