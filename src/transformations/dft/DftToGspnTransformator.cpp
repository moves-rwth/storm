#include "src/transformations/dft/DftToGspnTransformator.h"
#include "src/exceptions/NotImplementedException.h"
#include <memory>

namespace storm {
    namespace transformations {
        namespace dft {
            
            // Prevent some magic constants
            static constexpr const uint64_t defaultPriority = 0;
            static constexpr const uint64_t defaultCapacity = 0;

            template <typename ValueType>
            DftToGspnTransformator<ValueType>::DftToGspnTransformator(storm::storage::DFT<ValueType> const& dft) : mDft(dft) {
                // Intentionally left empty.
            }

            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::transform() {
				
                builder.setGspnName("DftToGspnTransformation");
				
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
				uint64_t beActive = builder.addPlace(defaultCapacity, isBEActive(dftBE) ? 1 : 0, dftBE->name() + STR_ACTIVATED);
                uint64_t beFailed = builder.addPlace(defaultCapacity, 0, dftBE->name() + STR_FAILED);
                assert(failedNodes.size() == dftBE->id());
                failedNodes.push_back(beFailed);
                uint64_t tActive = builder.addTimedTransition(defaultPriority, dftBE->activeFailureRate(), dftBE->name() + "_activeFailing");
                builder.addInputArc(beActive, tActive);
                builder.addInhibitionArc(beFailed, tActive);
                builder.addOutputArc(tActive, beActive);
                builder.addOutputArc(tActive, beFailed);
                uint64_t tPassive = builder.addTimedTransition(defaultPriority, dftBE->passiveFailureRate(), dftBE->name() + "_passiveFailing");
                builder.addInhibitionArc(beActive, tPassive);
                builder.addInhibitionArc(beFailed, tPassive);
                builder.addOutputArc(tActive, beFailed);
            }
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawAND(std::shared_ptr<storm::storage::DFTAnd<ValueType> const> dftAnd) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftAnd->name() + STR_FAILED);
                assert(failedNodes.size() == dftAnd->id());
                failedNodes.push_back(nodeFailed);
                uint64_t tAndFailed = builder.addImmediateTransition( getFailPriority(dftAnd)  , 0.0, dftAnd->name() + STR_FAILING );
                builder.addInhibitionArc(nodeFailed, tAndFailed);
                builder.addOutputArc(tAndFailed, nodeFailed);
                for(auto const& child : dftAnd->children()) {
                    assert(failedNodes.size() > child->id());
                    builder.addInputArc(failedNodes[child->id()], tAndFailed);
                }
                
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawOR(std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftOr->name() + STR_FAILED);
                assert(failedNodes.size() == dftOr->id());
                failedNodes.push_back(nodeFailed);
                uint64_t i = 0;
                for (auto const& child : dftOr->children()) {
                    uint64_t tNodeFailed = builder.addImmediateTransition( getFailPriority(dftOr)  , 0.0, dftOr->name() + STR_FAILING + std::to_string(i) );
                    builder.addInhibitionArc(nodeFailed, tNodeFailed);
                    builder.addOutputArc(tNodeFailed, nodeFailed);
                    assert(failedNodes.size() > child->id());
                    builder.addInputArc(failedNodes[child->id()], tNodeFailed);
                    ++i;
                }
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawVOT(std::shared_ptr<storm::storage::DFTVot<ValueType> const> dftVot) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftVot->name() + STR_FAILED);
                assert(failedNodes.size() == dftVot->id());
                failedNodes.push_back(nodeFailed);
                uint64_t nodeCollector = builder.addPlace(dftVot->nrChildren(), 0, dftVot->name() + "_collector");
                uint64_t tNodeFailed = builder.addImmediateTransition(getFailPriority(dftVot), 0.0, dftVot->name() + STR_FAILING);
                builder.addOutputArc(tNodeFailed, nodeFailed);
                builder.addInhibitionArc(nodeFailed, tNodeFailed);
                builder.addInputArc(nodeCollector, tNodeFailed, dftVot->threshold());
                builder.addOutputArc(tNodeFailed, nodeCollector, dftVot->threshold());
                uint64_t i = 0;
                for (auto const& child : dftVot->children()) {
                    uint64_t childInhibPlace = builder.addPlace(1, 0, dftVot->name() + "_child_fail_inhib" + std::to_string(i));
                    uint64_t tCollect = builder.addImmediateTransition(getFailPriority(dftVot), 0.0, dftVot->name() + "_child_collect" + std::to_string(i));
                    builder.addOutputArc(tCollect, nodeCollector);
                    builder.addOutputArc(tCollect, childInhibPlace);
                    builder.addInhibitionArc(childInhibPlace, tCollect);
                    builder.addInputArc(failedNodes[child->id()], tCollect);
                    ++i;
                }
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPAND(std::shared_ptr<storm::storage::DFTPand<ValueType> const> dftPand) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftPand->name()  + STR_FAILED);
                assert(failedNodes.size() == dftPand->id());
                failedNodes.push_back(nodeFailed);
                uint64_t nodeFS = builder.addPlace(defaultCapacity, 0, dftPand->name() + STR_FAILSAVE);
                uint64_t tNodeFailed = builder.addImmediateTransition(getFailPriority(dftPand), 0.0,  dftPand->name() + STR_FAILING);
                builder.addInhibitionArc(nodeFailed, tNodeFailed);
                builder.addInhibitionArc(nodeFS, tNodeFailed);
                builder.addOutputArc(tNodeFailed, nodeFailed);
                for(auto const& child : dftPand->children()) {
                    builder.addInputArc(failedNodes[child->id()], tNodeFailed);
                }
                for (uint64_t j = 1; j < dftPand->nrChildren(); ++j) {
                    uint64_t tfs = builder.addImmediateTransition(getFailPriority(dftPand), 0.0, dftPand->name() + STR_FAILSAVING + std::to_string(j));
                    builder.addInputArc(failedNodes[dftPand->children().at(j)->id()], tfs);
                    builder.addInhibitionArc(failedNodes[dftPand->children().at(j-1)->id()], tfs);
                    builder.addOutputArc(tfs, nodeFS);
                    
                }
                
//
//				storm::gspn::Place placePANDFailed;
//				placePANDFailed.setName(dftPand->name() + STR_FAILED);
//				placePANDFailed.setNumberOfInitialTokens(0);
//				mGspn.addPlace(placePANDFailed);
//				
//				storm::gspn::Place placePANDFailsave;
//				placePANDFailsave.setName(dftPand->name() + STR_FAILSAVE);
//				placePANDFailsave.setNumberOfInitialTokens(0);
//				mGspn.addPlace(placePANDFailsave);
//				
//				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPANDFailing;
//				immediateTransitionPANDFailing.setName(dftPand->name() + STR_FAILING);
//				immediateTransitionPANDFailing.setPriority(priority);
//				immediateTransitionPANDFailing.setWeight(0.0);
//				immediateTransitionPANDFailing.setInhibitionArcMultiplicity(placePANDFailed, 1);
//				immediateTransitionPANDFailing.setInhibitionArcMultiplicity(placePANDFailsave, 1);
//				immediateTransitionPANDFailing.setOutputArcMultiplicity(placePANDFailed, 1);
//				mGspn.addImmediateTransition(immediateTransitionPANDFailing);
//				
//				for (std::size_t i = 0; i < dftPand->nrChildren() -1; i++) {
//					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPANDFailsave;
//					immediateTransitionPANDFailsave.setName(dftPand->name() + "_" + std::to_string(i) + STR_FAILSAVING);
//					immediateTransitionPANDFailsave.setPriority(priority);
//					immediateTransitionPANDFailsave.setWeight(0.0);
//					immediateTransitionPANDFailsave.setInhibitionArcMultiplicity(placePANDFailsave, 1);
//					immediateTransitionPANDFailsave.setOutputArcMultiplicity(placePANDFailsave, 1);
//					mGspn.addImmediateTransition(immediateTransitionPANDFailsave);
//				}
			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawSPARE(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare) {
//				uint_fast64_t priority = getPriority(0, dftSpare);
//				
//				// This codeblock can be removed later, when I am 100% sure it is not needed anymore.
//				/*
//				storm::gspn::Place placeSPAREActivated;
//				placeSPAREActivated.setName(dftSpare->name() + STR_ACTIVATED);
//				placeSPAREActivated.setNumberOfInitialTokens(isBEActive(dftSpare));
//				mGspn.addPlace(placeSPAREActivated);
//				
//				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPCActivating;
//				immediateTransitionPCActivating.setName(dftSpare->children()[0]->name() + STR_ACTIVATING);
//				immediateTransitionPCActivating.setPriority(priority);
//				immediateTransitionPCActivating.setWeight(0.0);
//				immediateTransitionPCActivating.setInputArcMultiplicity(placeSPAREActivated, 1);
//				immediateTransitionPCActivating.setOutputArcMultiplicity(placeSPAREActivated, 1);
//				mGspn.addImmediateTransition(immediateTransitionPCActivating);
//				*/
//				
//				auto children = dftSpare->children();
//				
//				// Draw places and transitions that belong to each spare child.
//				for (std::size_t i = 1; i < children.size(); i++) {
//					auto placeChildClaimedPreexist = mGspn.getPlace(children[i]->name() + "_claimed");
//					
//					if (!placeChildClaimedPreexist.first) { // Only draw this place if it doesn't exist jet.
//						storm::gspn::Place placeChildClaimed;
//						placeChildClaimed.setName(children[i]->name() + "_claimed");
//						placeChildClaimed.setNumberOfInitialTokens(0);
//						mGspn.addPlace(placeChildClaimed);
//						
//						storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionSpareChildActivating;
//						immediateTransitionSpareChildActivating.setName(children[i]->name() + STR_ACTIVATING);
//						immediateTransitionSpareChildActivating.setPriority(priority);
//						immediateTransitionSpareChildActivating.setWeight(0.0);
//						immediateTransitionSpareChildActivating.setInputArcMultiplicity(placeChildClaimed, 1);
//						immediateTransitionSpareChildActivating.setOutputArcMultiplicity(placeChildClaimed, 1);
//						mGspn.addImmediateTransition(immediateTransitionSpareChildActivating);
//					}
//					
//					auto placeChildClaimedExist = mGspn.getPlace(children[i]->name() + "_claimed");
//					
//					storm::gspn::Place placeSPAREClaimedChild;
//					placeSPAREClaimedChild.setName(dftSpare->name() + "_claimed_" + children[i]->name());
//					placeSPAREClaimedChild.setNumberOfInitialTokens(0);
//					mGspn.addPlace(placeSPAREClaimedChild);
//					
//					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionChildClaiming;
//					immediateTransitionChildClaiming.setName(dftSpare->name() + "_claiming_" + children[i]->name());
//					immediateTransitionChildClaiming.setPriority(priority + 1); // Higher priority needed!
//					immediateTransitionChildClaiming.setWeight(0.0);
//					immediateTransitionChildClaiming.setInhibitionArcMultiplicity(placeChildClaimedExist.second, 1);
//					immediateTransitionChildClaiming.setOutputArcMultiplicity(placeChildClaimedExist.second, 1);
//					immediateTransitionChildClaiming.setOutputArcMultiplicity(placeSPAREClaimedChild, 1);
//					mGspn.addImmediateTransition(immediateTransitionChildClaiming);
//					
//					storm::gspn::Place placeSPAREChildConsumed;
//					if (i < children.size() - 1) {
//						placeSPAREChildConsumed.setName(dftSpare->name() + "_" + children[i]->name() + "_consumed");
//					}
//					else {
//						placeSPAREChildConsumed.setName(dftSpare->name() + STR_FAILED);
//					}
//					placeSPAREChildConsumed.setNumberOfInitialTokens(0);
//					mGspn.addPlace(placeSPAREChildConsumed);
//					
//					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionChildConsuming1;
//					immediateTransitionChildConsuming1.setName(dftSpare->name() + "_" + children[i]->name() + "_consuming1");
//					immediateTransitionChildConsuming1.setPriority(priority);
//					immediateTransitionChildConsuming1.setWeight(0.0);
//					immediateTransitionChildConsuming1.setOutputArcMultiplicity(placeSPAREChildConsumed, 1);
//					immediateTransitionChildConsuming1.setInhibitionArcMultiplicity(placeSPAREChildConsumed, 1);
//					immediateTransitionChildConsuming1.setInhibitionArcMultiplicity(placeSPAREClaimedChild, 1);
//					mGspn.addImmediateTransition(immediateTransitionChildConsuming1);
//					
//					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionChildConsuming2;
//					immediateTransitionChildConsuming2.setName(dftSpare->name() + "_" + children[i]->name() + "_consuming2");
//					immediateTransitionChildConsuming2.setPriority(priority);
//					immediateTransitionChildConsuming2.setWeight(0.0);
//					immediateTransitionChildConsuming2.setOutputArcMultiplicity(placeSPAREChildConsumed, 1);
//					immediateTransitionChildConsuming2.setInhibitionArcMultiplicity(placeSPAREChildConsumed, 1);
//					immediateTransitionChildConsuming2.setOutputArcMultiplicity(placeSPAREClaimedChild, 1);
//					immediateTransitionChildConsuming2.setInputArcMultiplicity(placeSPAREClaimedChild, 1);
//					mGspn.addImmediateTransition(immediateTransitionChildConsuming2);
//				}
//				
//				// Draw connections between all spare childs.
//				for (std::size_t i = 1; i < children.size() - 1; i++) {
//					auto placeSPAREChildConsumed = mGspn.getPlace(dftSpare->name() + "_" + children[i]->name() + "_consumed");
//					auto immediateTransitionChildClaiming = mGspn.getImmediateTransition(dftSpare->name() + "_claiming_" + children[i + 1]->name());
//					auto immediateTransitionChildConsuming1 = mGspn.getImmediateTransition(dftSpare->name() + "_" + children[i + 1]->name() + "_consuming1");
//					
//					immediateTransitionChildClaiming.second->setOutputArcMultiplicity(placeSPAREChildConsumed.second, 1);
//					immediateTransitionChildClaiming.second->setInputArcMultiplicity(placeSPAREChildConsumed.second, 1);
//					
//					immediateTransitionChildConsuming1.second->setOutputArcMultiplicity(placeSPAREChildConsumed.second, 1);
//					immediateTransitionChildConsuming1.second->setInputArcMultiplicity(placeSPAREChildConsumed.second, 1);
//				}
			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor) {
//				uint_fast64_t priority = getPriority(0, dftPor);
//				
//				storm::gspn::Place placePORFailed;
//				placePORFailed.setName(dftPor->name() + STR_FAILED);
//				placePORFailed.setNumberOfInitialTokens(0);
//				mGspn.addPlace(placePORFailed);
//				
//				storm::gspn::Place placePORFailsave;
//				placePORFailsave.setName(dftPor->name() + STR_FAILSAVE);
//				placePORFailsave.setNumberOfInitialTokens(0);
//				mGspn.addPlace(placePORFailsave);
//				
//				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPORFailing;
//				immediateTransitionPORFailing.setName(dftPor->name() + STR_FAILING);
//				immediateTransitionPORFailing.setPriority(priority);
//				immediateTransitionPORFailing.setWeight(0.0);
//				immediateTransitionPORFailing.setInhibitionArcMultiplicity(placePORFailed, 1);
//				immediateTransitionPORFailing.setInhibitionArcMultiplicity(placePORFailsave, 1);
//				immediateTransitionPORFailing.setOutputArcMultiplicity(placePORFailed, 1);
//				mGspn.addImmediateTransition(immediateTransitionPORFailing);
//				
//				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionPORFailsave;
//				immediateTransitionPORFailsave.setName(dftPor->name() + STR_FAILSAVING);
//				immediateTransitionPORFailsave.setPriority(priority);
//				immediateTransitionPORFailsave.setWeight(0.0);
//				immediateTransitionPORFailsave.setInhibitionArcMultiplicity(placePORFailsave, 1);
//				immediateTransitionPORFailsave.setOutputArcMultiplicity(placePORFailsave, 1);
//				mGspn.addImmediateTransition(immediateTransitionPORFailsave);
			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTF(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstF) {
//				storm::gspn::Place placeCONSTFFailed;
//				placeCONSTFFailed.setName(dftConstF->name() + STR_FAILED);
//				placeCONSTFFailed.setNumberOfInitialTokens(1);
//				mGspn.addPlace(placeCONSTFFailed);
			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTS(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstS) {
//				storm::gspn::Place placeCONSTSFailed;
//				placeCONSTSFailed.setName(dftConstS->name() + STR_FAILED);
//				placeCONSTSFailed.setNumberOfInitialTokens(0);
//				placeCONSTSFailed.setCapacity(0); // It cannot contain a token, because it cannot fail.
//				mGspn.addPlace(placeCONSTSFailed);
			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPDEP(std::shared_ptr<storm::storage::DFTDependency<ValueType> const>(dftDependency)) {
//				// Only draw dependency, if it wasn't drawn before.
//				std::string gateName = dftDependency->name().substr(0, dftDependency->name().find("_"));
//				auto exists = mGspn.getPlace(gateName + STR_FAILED);
//				if (!exists.first) {
//					storm::gspn::Place placeDEPFailed;
//					placeDEPFailed.setName(gateName + STR_FAILED);
//					placeDEPFailed.setNumberOfInitialTokens(0);
//					mGspn.addPlace(placeDEPFailed);
//					
//					storm::gspn::TimedTransition<double> timedTransitionDEPFailure;
//					timedTransitionDEPFailure.setName(gateName + STR_FAILING);
//					timedTransitionDEPFailure.setPriority(getPriority(0, dftDependency));
//					timedTransitionDEPFailure.setRate(dftDependency->probability());
//					timedTransitionDEPFailure.setOutputArcMultiplicity(placeDEPFailed, 1);
//					timedTransitionDEPFailure.setInhibitionArcMultiplicity(placeDEPFailed, 1);
//					mGspn.addTimedTransition(timedTransitionDEPFailure);
//				}
			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNConnections() {
//				// Check for every element, if they have parents (all will have at least 1, except the top event). 
//				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
//					auto child = mDft.getElement(i);
//					auto parents = child->parentIds();
//					
//					// Draw a connection to every parent.
//					for (std::size_t j = 0; j < parents.size(); j++) {	
//						// Check the type of the parent and act accordingly (every parent gate has different entry points...).
//						switch (mDft.getElement(parents[j])->type()) {
//							case storm::storage::DFTElementType::AND:
//							{
//								auto andEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILING);
//								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
//								if (andEntry.first && childExit.first) { // Only add arcs if the objects have been found.
//									andEntry.second->setInputArcMultiplicity(childExit.second, 1);
//									andEntry.second->setOutputArcMultiplicity(childExit.second, 1);
//								}
//								break;
//							}
//							case storm::storage::DFTElementType::OR:
//							{
//								auto orEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + child->name() + STR_FAILING);
//								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
//								if (orEntry.first && childExit.first) { // Only add arcs if the objects have been found.
//									orEntry.second->setInputArcMultiplicity(childExit.second, 1);
//									orEntry.second->setOutputArcMultiplicity(childExit.second, 1);
//								}
//								break;
//							}
//							case storm::storage::DFTElementType::VOT:
//							{
//								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
//								auto parentEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + child->name() + "_collecting");
//								
//								if (childExit.first && parentEntry.first) { // Only add arcs if the objects have been found.
//									parentEntry.second->setInputArcMultiplicity(childExit.second, 1);
//									parentEntry.second->setOutputArcMultiplicity(childExit.second, 1);
//								}
//								
//								break;
//							}
//							case storm::storage::DFTElementType::PAND:
//							{
//								auto children =  std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(mDft.getElement(parents[j]))->children();
//								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
//								auto pandEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILING);
//								
//								if (childExit.first && pandEntry.first) { // Only add arcs if the objects have been found.
//									pandEntry.second->setInputArcMultiplicity(childExit.second, 1);
//									pandEntry.second->setOutputArcMultiplicity(childExit.second, 1);
//									
//									if (children[0] == child) { // Current element is primary child.
//										auto pandEntry2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_0" + STR_FAILSAVING);
//										
//										if (pandEntry2.first) {
//											pandEntry2.second->setInhibitionArcMultiplicity(childExit.second, 1);
//										}
//									}
//									else { // Current element is not the primary child.
//										for (std::size_t k = 1; k < children.size(); k++) {
//											if (children[k] == child) {
//												auto pandEntry2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + std::to_string((k - 1)) + STR_FAILSAVING);
//												auto pandEntry3 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + std::to_string((k)) + STR_FAILSAVING);
//												
//												if (pandEntry2.first) {
//													pandEntry2.second->setInputArcMultiplicity(childExit.second, 1);
//													pandEntry2.second->setOutputArcMultiplicity(childExit.second, 1);
//												}
//												
//												if (pandEntry3.first) {
//													pandEntry3.second->setInhibitionArcMultiplicity(childExit.second, 1);
//												}
//												
//												continue;
//											}
//										}
//									}
//								}
//								
//								break;
//							}
//							case storm::storage::DFTElementType::SPARE:
//							{
//								// Check if current child is a primary or spare child.
//								auto children =  std::static_pointer_cast<storm::storage::DFTSpare<ValueType> const>(mDft.getElement(parents[j]))->children();
//								
//								if (child == children[0]) { // Primary child.
//									auto spareExit = mGspn.getImmediateTransition(child->name() + STR_ACTIVATING);
//									
//									std::vector<int> ids = getAllBEIDsOfElement(child);
//									for (std::size_t k = 0; k < ids.size(); k++) {
//										auto childEntry = mGspn.getPlace(mDft.getElement(ids[k])->name() + STR_ACTIVATED);
//										
//										if (spareExit.first && childEntry.first) { // Only add arcs if the objects have been found.
//											spareExit.second->setInhibitionArcMultiplicity(childEntry.second, 1);
//											spareExit.second->setOutputArcMultiplicity(childEntry.second, 1);
//										}
//									}
//									
//									// Draw lines from "primary child_failed" to SPARE.
//									auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
//									auto spareEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_claiming_" + children[1]->name());
//									auto spareEntry2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + children[1]->name() + "_consuming1");
//									
//									if (childExit.first && spareEntry.first && spareEntry2.first) { // Only add arcs if the objects have been found.
//										spareEntry.second->setInputArcMultiplicity(childExit.second, 1);
//										spareEntry.second->setOutputArcMultiplicity(childExit.second, 1);
//										
//										spareEntry2.second->setInputArcMultiplicity(childExit.second, 1);
//										spareEntry2.second->setOutputArcMultiplicity(childExit.second, 1);
//									}
//								}
//								else { // A spare child.
//									auto spareExit = mGspn.getImmediateTransition(child->name() + STR_ACTIVATING);
//									auto spareExit2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_claiming_" + child->name());
//									
//									std::vector<int> ids = getAllBEIDsOfElement(child);
//									for (std::size_t k = 0; k < ids.size(); k++) {
//										auto childEntry = mGspn.getPlace(mDft.getElement(ids[k])->name() + STR_ACTIVATED);
//										
//										if (spareExit.first && spareExit2.first && childEntry.first) { // Only add arcs if the objects have been found.
//											if (!spareExit.second->existsInhibitionArc(childEntry.second)) {
//												spareExit.second->setInhibitionArcMultiplicity(childEntry.second, 1);
//											}
//											if (!spareExit.second->existsOutputArc(childEntry.second)) {
//												spareExit.second->setOutputArcMultiplicity(childEntry.second, 1);
//											}
//											if (!spareExit2.second->existsInhibitionArc(childEntry.second)) {
//												spareExit2.second->setInhibitionArcMultiplicity(childEntry.second, 1);
//											}
//										}
//									}
//									
//									auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
//									auto spareEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_claiming_" + child->name());
//									auto spareEntry2 = mGspn.getImmediateTransition(child->name() + STR_ACTIVATING);
//									auto spareEntry3 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + child->name() + "_consuming2");
//									
//									if (childExit.first && spareEntry.first && spareEntry2.first && spareEntry3.first) { // Only add arcs if the objects have been found.
//										spareEntry.second->setInhibitionArcMultiplicity(childExit.second, 1);
//										
//										if (!spareEntry2.second->existsInhibitionArc(childExit.second)) {
//											spareEntry2.second->setInhibitionArcMultiplicity(childExit.second, 1);
//										}
//										
//										spareEntry3.second->setInputArcMultiplicity(childExit.second, 1);
//										spareEntry3.second->setOutputArcMultiplicity(childExit.second, 1);
//									}
//								}
//								
//								break;
//							}
//							case storm::storage::DFTElementType::POR:
//							{
//								auto children =  std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(mDft.getElement(parents[j]))->children();
//								auto porEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILING);
//								auto porEntry2 = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + STR_FAILSAVING);
//								auto childExit = mGspn.getPlace(child->name() + STR_FAILED);
//								
//								if (porEntry.first && porEntry2.first && childExit.first) { // Only add arcs if the objects have been found.
//									if (children[0] == child) { // Current element is primary child.
//										porEntry.second->setInputArcMultiplicity(childExit.second, 1);
//										porEntry.second->setOutputArcMultiplicity(childExit.second, 1);
//										porEntry2.second->setInhibitionArcMultiplicity(childExit.second, 1);
//										
//									}
//									else { // Current element is not the primary child.
//										porEntry2.second->setInputArcMultiplicity(childExit.second, 1);
//										porEntry2.second->setOutputArcMultiplicity(childExit.second, 1);
//									}
//								}
//								
//								break;
//							}
//							case storm::storage::DFTElementType::SEQ:
//							{
//								// Sequences are realized with restrictions. Nothing to do here.
//								break;
//							}
//							case storm::storage::DFTElementType::MUTEX:
//							{
//								// MUTEX are realized with restrictions. Nothing to do here.
//								break;
//							}
//							case storm::storage::DFTElementType::BE:
//							{
//								// The parent is never a Basic Event.
//								break;
//							}
//							case storm::storage::DFTElementType::CONSTF:
//							{
//								// The parent is never a Basic Event.
//								break;
//							}
//							case storm::storage::DFTElementType::CONSTS:
//							{
//								// The parent is never a Basic Event.
//								break;
//							}
//							case storm::storage::DFTElementType::PDEP:
//							{
//								// The parent is never a DEP. Hence the connections must be drawn somewhere else.
//								break;
//							}
//							default:
//							{
//								STORM_LOG_ASSERT(false, "DFT type unknown.");
//								break;
//							}
//						}
//					}
//				}
			}
			
			template <typename ValueType>
            bool DftToGspnTransformator<ValueType>::isBEActive(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement)
			{
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
            uint64_t DftToGspnTransformator<ValueType>::getFailPriority(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement)
			{
                return mDft.maxRank() - dftElement->rank();
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNDependencies() {
//				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
//					auto dftElement = mDft.getElement(i);
//					
//					if (dftElement->isDependency()) {
//						std::string gateName = dftElement->name().substr(0, dftElement->name().find("_"));
//						auto depEntry = mGspn.getTimedTransition(gateName + STR_FAILING);
//						auto trigger = mGspn.getPlace(std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(dftElement)->nameTrigger() + STR_FAILED);
//						
//						if (depEntry.first && trigger.first) { // Only add arcs if the objects have been found.
//							if (!depEntry.second->existsInputArc(trigger.second)) {
//								depEntry.second->setInputArcMultiplicity(trigger.second, 1);
//							}
//							if (!depEntry.second->existsOutputArc(trigger.second)){
//								depEntry.second->setOutputArcMultiplicity(trigger.second, 1);
//							}
//						}
//						
//						auto dependent = mGspn.getPlace(std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(dftElement)->nameDependent() + STR_FAILED);
//						
//						if (dependent.first) { // Only add arcs if the objects have been found.
//							depEntry.second->setOutputArcMultiplicity(dependent.second, 1);
//						}
//					}
//				}
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNRestrictions() {
//				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
//					auto dftElement = mDft.getElement(i);
//					
//					if (dftElement->isRestriction()) {
//						switch (dftElement->type()) {
//							case storm::storage::DFTElementType::SEQ:
//							{
//								auto children = mDft.getRestriction(i)->children();
//								
//								for (std::size_t j = 0; j < children.size() - 1; j++) {
//									auto suppressor = mGspn.getPlace(children[j]->name() + STR_FAILED);
//									
//									switch (children[j + 1]->type()) {
//										case storm::storage::DFTElementType::BE: // If suppressed is a BE, add 2 arcs to timed transitions.
//										{
//											auto suppressedActive = mGspn.getTimedTransition(children[j + 1]->name() + "_activeFailing");
//											auto suppressedPassive = mGspn.getTimedTransition(children[j + 1]->name() + "_passiveFailing");
//											
//											if (suppressor.first && suppressedActive.first && suppressedPassive.first) { // Only add arcs if the objects have been found.
//												suppressedActive.second->setInputArcMultiplicity(suppressor.second, 1);
//												suppressedActive.second->setOutputArcMultiplicity(suppressor.second, 1);
//												suppressedPassive.second->setInputArcMultiplicity(suppressor.second, 1);
//												suppressedPassive.second->setOutputArcMultiplicity(suppressor.second, 1);
//											}
//											break;
//										}
//										default: // If supressed is not a BE, add single arc to immediate transition.
//										{
//											auto suppressed = mGspn.getImmediateTransition(children[j + 1]->name() + STR_FAILING);
//											
//											if (suppressor.first && suppressed.first) { // Only add arcs if the objects have been found.
//												suppressed.second->setInputArcMultiplicity(suppressor.second, 1);
//												suppressed.second->setOutputArcMultiplicity(suppressor.second, 1);
//											}
//											break;
//										}
//									}
//								}
//								break;
//							}
//							case storm::storage::DFTElementType::MUTEX:
//							{
//								// MUTEX is not implemented by the DFTGalileoParser yet. Nothing to do here.
//								STORM_LOG_ASSERT(false, "MUTEX is not supported by DftToGspnTransformator.");
//								break;
//							}
//							default:
//							{
//								break;
//							}
//						}
//					}
//				}
			}
			
			template <typename ValueType>
            std::vector<int> DftToGspnTransformator<ValueType>::getAllBEIDsOfElement(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement) {
//				std::vector<int> ids;
//				
//				switch (dftElement->type()) {
//					case storm::storage::DFTElementType::AND:
//					{
//						auto children = std::static_pointer_cast<storm::storage::DFTAnd<ValueType> const>(dftElement)->children();
//						
//						for (std::size_t i = 0; i < children.size(); i++) {
//							std::vector<int> newIds = getAllBEIDsOfElement(children[i]);
//							ids.insert(ids.end(), newIds.begin(), newIds.end());
//						}
//						break;
//					}
//					case storm::storage::DFTElementType::OR:
//					{
//						auto children = std::static_pointer_cast<storm::storage::DFTOr<ValueType> const>(dftElement)->children();
//						
//						for (std::size_t i = 0; i < children.size(); i++) {
//							std::vector<int> newIds = getAllBEIDsOfElement(children[i]);
//							ids.insert(ids.end(), newIds.begin(), newIds.end());
//						}
//						break;
//					}
//					case storm::storage::DFTElementType::VOT:
//					{
//						auto children = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(dftElement)->children();
//						
//						for (std::size_t i = 0; i < children.size(); i++) {
//							std::vector<int> newIds = getAllBEIDsOfElement(children[i]);
//							ids.insert(ids.end(), newIds.begin(), newIds.end());
//						}
//						break;
//					}
//					case storm::storage::DFTElementType::PAND:
//					{
//						auto children = std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(dftElement)->children();
//						
//						for (std::size_t i = 0; i < children.size(); i++) {
//							std::vector<int> newIds = getAllBEIDsOfElement(children[i]);
//							ids.insert(ids.end(), newIds.begin(), newIds.end());
//						}
//						break;
//					}
//					case storm::storage::DFTElementType::SPARE:
//					{								
//						auto children = std::static_pointer_cast<storm::storage::DFTSpare<ValueType> const>(dftElement)->children();
//						
//						// Only regard the primary child of a SPARE. The spare childs are not allowed to be listed here.
//						for (std::size_t i = 0; i < 1; i++) {
//							std::vector<int> newIds = getAllBEIDsOfElement(children[i]);
//							ids.insert(ids.end(), newIds.begin(), newIds.end());
//						}
//						break;
//					}
//					case storm::storage::DFTElementType::POR:
//					{
//						auto children = std::static_pointer_cast<storm::storage::DFTPor<ValueType> const>(dftElement)->children();
//						
//						for (std::size_t i = 0; i < children.size(); i++) {
//							std::vector<int> newIds = getAllBEIDsOfElement(children[i]);
//							ids.insert(ids.end(), newIds.begin(), newIds.end());
//						}
//						break;
//					}
//					case storm::storage::DFTElementType::BE:
//					case storm::storage::DFTElementType::CONSTF:
//					case storm::storage::DFTElementType::CONSTS:
//					{
//						ids.push_back(dftElement->id());
//						break;
//					}
//					case storm::storage::DFTElementType::SEQ:
//					case storm::storage::DFTElementType::MUTEX:
//					case storm::storage::DFTElementType::PDEP:
//					{
//						break;
//					}
//					default:
//					{
//						STORM_LOG_ASSERT(false, "DFT type unknown.");
//						break;
//					}
//				}
//				
//				return ids;
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::writeGspn(bool toFile) {
                if (toFile) {
                    // Writing to file
                    std::ofstream file;
                    file.open("gspn.dot");
                    storm::gspn::GSPN* gspn = builder.buildGspn();
                    gspn->writeDotToStream(file);
                    delete gspn;
                    file.close();
                } else {
                    // Writing to console
                    storm::gspn::GSPN* gspn = builder.buildGspn();
                    gspn->writeDotToStream(std::cout);
                    delete gspn;
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


