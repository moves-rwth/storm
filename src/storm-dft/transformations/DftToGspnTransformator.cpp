#include "DftToGspnTransformator.h"
#include "storm/exceptions/NotImplementedException.h"
#include <memory>

namespace storm {
    namespace transformations {
        namespace dft {
            
            // Prevent some magic constants
            static constexpr const uint64_t defaultPriority = 1;
            static constexpr const uint64_t defaultCapacity = 1;

            template <typename ValueType>
            DftToGspnTransformator<ValueType>::DftToGspnTransformator(storm::storage::DFT<ValueType> const& dft) : mDft(dft) {
                // Intentionally left empty.
            }

            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::transform() {
				
                builder.setGspnName("DftToGspnTransformation");
				
				// Loop through every DFT element and draw them as a GSPN.
				drawGSPNElements();

				// Draw restrictions into the GSPN (i.e. SEQ or MUTEX).
				//drawGSPNRestrictions();
            }
            
            template<typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::toplevelFailedPlaceId() {
                assert(failedNodes.size() > mDft.getTopLevelIndex());
                return failedNodes[mDft.getTopLevelIndex()];
            }
            
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNElements() {
                
                
				// Loop through every DFT element and draw them as a GSPN.
				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
					auto dftElement = mDft.getElement(i);
                    bool isRepresentative = mDft.isRepresentative(i);
                    
					// Check which type the element is and call the corresponding drawing-function.
					switch (dftElement->type()) {
						case storm::storage::DFTElementType::AND:
							drawAND(std::static_pointer_cast<storm::storage::DFTAnd<ValueType> const>(dftElement), isRepresentative);
							break;
						case storm::storage::DFTElementType::OR:
							drawOR(std::static_pointer_cast<storm::storage::DFTOr<ValueType> const>(dftElement), isRepresentative);
							break;
						case storm::storage::DFTElementType::VOT:
							drawVOT(std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(dftElement), isRepresentative);
							break;
						case storm::storage::DFTElementType::PAND:
							drawPAND(std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(dftElement), isRepresentative);
							break;
						case storm::storage::DFTElementType::SPARE:
							drawSPARE(std::static_pointer_cast<storm::storage::DFTSpare<ValueType> const>(dftElement), isRepresentative);
							break;
						case storm::storage::DFTElementType::POR:
							drawPOR(std::static_pointer_cast<storm::storage::DFTPor<ValueType> const>(dftElement), isRepresentative);
							break;
						case storm::storage::DFTElementType::SEQ:
                            drawSeq(std::static_pointer_cast<storm::storage::DFTSeq<ValueType> const>(dftElement));
                            break;
						case storm::storage::DFTElementType::MUTEX:
							// No method call needed here. MUTEX only consists of restrictions, which are handled later.
							break;
						case storm::storage::DFTElementType::BE:
							drawBE(std::static_pointer_cast<storm::storage::DFTBE<ValueType> const>(dftElement), isRepresentative);
							break;
						case storm::storage::DFTElementType::CONSTF:
							drawCONSTF(dftElement, isRepresentative);
							break;
						case storm::storage::DFTElementType::CONSTS:
							drawCONSTS(dftElement, isRepresentative);
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
            void DftToGspnTransformator<ValueType>::drawBE(std::shared_ptr<storm::storage::DFTBE<ValueType> const> dftBE, bool isRepresentative) {
                double xcenter = 10.0;
                double ycenter = 10.0;
				uint64_t beActive = builder.addPlace(defaultCapacity, isBEActive(dftBE) ? 1 : 0, dftBE->name() + STR_ACTIVATED);
                activeNodes.emplace(dftBE->id(), beActive);
                uint64_t beFailed = builder.addPlace(defaultCapacity, 0, dftBE->name() + STR_FAILED);
                
                uint64_t disabledNode = 0;
                if (!smart || dftBE->nrRestrictions() > 0) {
                    disabledNode = addDisabledPlace(dftBE);
                }
                
                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftBE);
                }
                
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
                builder.addOutputArc(tPassive, beFailed);
                
                if (!smart || dftBE->nrRestrictions() > 0) {
                    builder.addInhibitionArc(disabledNode, tActive);
                    builder.addInhibitionArc(disabledNode, tPassive);
                }
                
                if (!smart || isRepresentative) {
                    builder.addOutputArc(tActive, unavailableNode);
                    builder.addOutputArc(tPassive, unavailableNode);
                }
                
                builder.setPlaceLayoutInfo(beActive, storm::gspn::LayoutInfo(xcenter - 3.0, ycenter));
                builder.setPlaceLayoutInfo(beFailed, storm::gspn::LayoutInfo(xcenter + 3.0, ycenter));
                builder.setTransitionLayoutInfo(tActive, storm::gspn::LayoutInfo(xcenter, ycenter + 3.0));
                builder.setTransitionLayoutInfo(tPassive, storm::gspn::LayoutInfo(xcenter, ycenter - 3.0));
                
            }
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawAND(std::shared_ptr<storm::storage::DFTAnd<ValueType> const> dftAnd, bool isRepresentative) {
                double xcenter = 10.0;
                double ycenter = 20.0;
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftAnd->name() + STR_FAILED);
                assert(failedNodes.size() == dftAnd->id());
                failedNodes.push_back(nodeFailed);
                
                uint64_t unavailableNode = 0;
                if (isRepresentative) {
                    unavailableNode = addUnavailableNode(dftAnd);
                }

                
                uint64_t tAndFailed = builder.addImmediateTransition( getFailPriority(dftAnd)  , 0.0, dftAnd->name() + STR_FAILING );
                builder.addInhibitionArc(nodeFailed, tAndFailed);
                builder.addOutputArc(tAndFailed, nodeFailed);
                if (isRepresentative) {
                    builder.addOutputArc(tAndFailed, unavailableNode);
                }
                for(auto const& child : dftAnd->children()) {
                    assert(failedNodes.size() > child->id());
                    builder.addInputArc(failedNodes[child->id()], tAndFailed);
                    builder.addOutputArc(tAndFailed, failedNodes[child->id()]);
                }
                
                builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter, ycenter-3.0));
                builder.setTransitionLayoutInfo(tAndFailed, storm::gspn::LayoutInfo(xcenter, ycenter+3.0));
                
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawOR(std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr, bool isRepresentative) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftOr->name() + STR_FAILED);
                assert(failedNodes.size() == dftOr->id());
                failedNodes.push_back(nodeFailed);
                uint64_t unavailableNode = 0;
                if (isRepresentative) {
                    unavailableNode = addUnavailableNode(dftOr);
                }
                
                uint64_t i = 0;
                for (auto const& child : dftOr->children()) {
                    uint64_t tNodeFailed = builder.addImmediateTransition( getFailPriority(dftOr)  , 0.0, dftOr->name() + STR_FAILING + std::to_string(i) );
                    builder.addInhibitionArc(nodeFailed, tNodeFailed);
                    builder.addOutputArc(tNodeFailed, nodeFailed);
                    if (isRepresentative) {
                        builder.addOutputArc(tNodeFailed, unavailableNode);
                    }
                    assert(failedNodes.size() > child->id());
                    builder.addInputArc(failedNodes[child->id()], tNodeFailed);
                    builder.addOutputArc(tNodeFailed, failedNodes[child->id()]);
                    ++i;
                }
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawVOT(std::shared_ptr<storm::storage::DFTVot<ValueType> const> dftVot, bool isRepresentative) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftVot->name() + STR_FAILED);
                assert(failedNodes.size() == dftVot->id());
                failedNodes.push_back(nodeFailed);
                uint64_t unavailableNode = 0;
                if (isRepresentative) {
                    unavailableNode = addUnavailableNode(dftVot);
                }
                
                uint64_t nodeCollector = builder.addPlace(dftVot->nrChildren(), 0, dftVot->name() + "_collector");
                uint64_t tNodeFailed = builder.addImmediateTransition(getFailPriority(dftVot), 0.0, dftVot->name() + STR_FAILING);
                builder.addOutputArc(tNodeFailed, nodeFailed);
                if (isRepresentative) {
                    builder.addOutputArc(tNodeFailed, unavailableNode);
                }
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
                    builder.addOutputArc(tCollect, failedNodes[child->id()]);
                    ++i;
                }
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPAND(std::shared_ptr<storm::storage::DFTPand<ValueType> const> dftPand, bool isRepresentative) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftPand->name()  + STR_FAILED);
                assert(failedNodes.size() == dftPand->id());
                failedNodes.push_back(nodeFailed);
                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftPand);
                }
                
                uint64_t tNodeFailed = builder.addImmediateTransition(getFailPriority(dftPand), 0.0,  dftPand->name() + STR_FAILING);
                builder.addInhibitionArc(nodeFailed, tNodeFailed);
                builder.addOutputArc(tNodeFailed, nodeFailed);
                if (!smart || isRepresentative) {
                    builder.addOutputArc(tNodeFailed, nodeFailed);
                }
                
                
                if(dftPand->isInclusive()) {
                    
                    uint64_t nodeFS = builder.addPlace(defaultCapacity, 0, dftPand->name() + STR_FAILSAVE);
                    builder.addInhibitionArc(nodeFS, tNodeFailed);
                    for(auto const& child : dftPand->children()) {
                        builder.addInputArc(failedNodes[child->id()], tNodeFailed);
                        builder.addOutputArc(tNodeFailed, failedNodes[child->id()]);
                    }
                    for (uint64_t j = 1; j < dftPand->nrChildren(); ++j) {
                        uint64_t tfs = builder.addImmediateTransition(getFailPriority(dftPand), 0.0, dftPand->name() + STR_FAILSAVING + std::to_string(j));
                        builder.addInputArc(failedNodes[dftPand->children().at(j)->id()], tfs);
                        builder.addOutputArc(tfs, failedNodes[dftPand->children().at(j)->id()]);
                        builder.addInhibitionArc(failedNodes[dftPand->children().at(j-1)->id()], tfs);
                        builder.addOutputArc(tfs, nodeFS);
                        builder.addInhibitionArc(nodeFS, tfs);
                        
                    }
                } else {
                    uint64_t fi = 0;
                    uint64_t tn = 0;
                    for(uint64_t j = 0; j < dftPand->nrChildren(); ++j) {
                        auto const& child = dftPand->children()[j];
                        if (j > 0) {
                            builder.addInhibitionArc(failedNodes.at(child->id()), tn);
                        }
                        if (j != dftPand->nrChildren() - 1) {
                            tn = builder.addImmediateTransition(getFailPriority(dftPand), 0.0, dftPand->name() + STR_FAILING + "_" +std::to_string(j));
                        } else {
                            tn = tNodeFailed;
                        }
                        builder.addInputArc(failedNodes.at(child->id()), tn);
                        builder.addOutputArc(tn, failedNodes.at(child->id()));
                        if (j > 0) {
                            builder.addInputArc(fi, tn);
                        }
                        if (j != dftPand->nrChildren() - 1) {
                            fi = builder.addPlace(defaultCapacity, 0, dftPand->name() + "_F_" + std::to_string(j));
                            builder.addOutputArc(tn, fi);
                        }
                        
                    }
                }
            }
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawSPARE(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare, bool isRepresentative) {
                
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftSpare->name() + STR_FAILED);
                failedNodes.push_back(nodeFailed);
                uint64_t unavailableNode = 0;
                if (isRepresentative) {
                    unavailableNode = addUnavailableNode(dftSpare);
                }
                uint64_t spareActive = builder.addPlace(defaultCapacity, 0, dftSpare->name() + STR_ACTIVATED);
                activeNodes.emplace(dftSpare->id(), spareActive);

                
                std::vector<uint64_t> cucNodes;
                std::vector<uint64_t> nextClaimNodes;
                std::vector<uint64_t> nextclTransitions;
                std::vector<uint64_t> nextconsiderTransitions;
                uint64_t j = 0;
                for(auto const& child : dftSpare->children()) {
                    if (j > 0) {
                        nextClaimNodes.push_back(builder.addPlace(defaultCapacity, 0, dftSpare->name()+ "_consider_" + child->name()));
                        
                        builder.addOutputArc(nextclTransitions.back(), nextClaimNodes.back(), 1);
                        if (j > 1) {
                            builder.addOutputArc(nextconsiderTransitions.back(), nextClaimNodes.back());
                        }
                        
                        uint64_t tnextconsider = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0, dftSpare->name() + "_cannot_claim_" + child->name());
                        builder.addInputArc(nextClaimNodes.back(), tnextconsider);
                        builder.addInputArc(unavailableNodes.at(child->id()), tnextconsider);
                        nextconsiderTransitions.push_back(tnextconsider);
                        
                    }
                    cucNodes.push_back(builder.addPlace(defaultCapacity, j == 0 ? 1 : 0, dftSpare->name() + "_claimed_" + child->name()));
                    if (j > 0) {
                        uint64 tclaim = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0, dftSpare->name() + "_claim_" + child->name());
                        builder.addInhibitionArc(unavailableNodes.at(child->id()), tclaim);
                        builder.addInputArc(nextClaimNodes.back(), tclaim);
                        builder.addOutputArc(tclaim, cucNodes.back());
                    }
                    uint64_t tnextcl = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0, dftSpare->name() + "_next_claim_" + std::to_string(j));
                    builder.addInputArc(cucNodes.back(), tnextcl);
                    builder.addOutputArc(tnextcl, cucNodes.back());
                    builder.addInputArc(failedNodes.at(child->id()), tnextcl);
                    builder.addOutputArc(tnextcl, failedNodes.at(child->id()));
                    nextclTransitions.push_back(tnextcl);
                    ++j;
                }
                builder.addOutputArc(nextconsiderTransitions.back(), nodeFailed);
                builder.addOutputArc(nextclTransitions.back(), nodeFailed);

                if (isRepresentative) {
                    builder.addOutputArc(nextconsiderTransitions.back(), unavailableNode);
                    builder.addOutputArc(nextclTransitions.back(), unavailableNode);
                }
                

			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor, bool isRepresentative) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftPor->name() + STR_FAILED);
                failedNodes.push_back(nodeFailed);
                
                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftPor);
                }
                
                uint64_t tfail = builder.addImmediateTransition(getFailPriority(dftPor), 0.0, dftPor->name() + STR_FAILING);
                builder.addOutputArc(tfail, nodeFailed);
                builder.addInhibitionArc(nodeFailed, tfail);
                
                builder.addInputArc(failedNodes.at(dftPor->children().front()->id()), tfail);
                builder.addOutputArc(tfail, failedNodes.at(dftPor->children().front()->id()));
                
                if(!smart || isRepresentative) {
                    builder.addOutputArc(tfail, unavailableNode);
                }
                
                if(dftPor->isInclusive()) {
                    uint64_t nodeFS = builder.addPlace(defaultCapacity, 0, dftPor->name() + STR_FAILSAVE);
                    builder.addInhibitionArc(nodeFS, tfail);
                    uint64_t j = 0;
                    for (auto const& child : dftPor->children()) {
                        if(j > 0) {
                            uint64_t tfailsf = builder.addImmediateTransition(getFailPriority(dftPor), 0.0, dftPor->name() + STR_FAILSAVING + std::to_string(j));
                            builder.addInputArc(failedNodes.at(child->id()), tfailsf);
                            builder.addOutputArc(tfailsf, failedNodes.at(child->id()));
                            builder.addOutputArc(tfailsf, nodeFS);
                            builder.addInhibitionArc(nodeFS, tfailsf);
                            builder.addInhibitionArc(failedNodes.at(dftPor->children().front()->id()), tfailsf);
                        }
                        
                        ++j;
                    }
                } else {
                    uint64_t j = 0;
                    for (auto const& child : dftPor->children()) {
                        if(j > 0) {
                            builder.addInhibitionArc(failedNodes.at(child->id()), tfail);
                        }
                        ++j;
                    }
                    
                }
                
			}
            
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTF(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstF, bool isRepresentative) {
			    failedNodes.push_back(builder.addPlace(defaultCapacity, 1, dftConstF->name() + STR_FAILED));
                uint64_t unavailableNode = 0;
                if (isRepresentative) {
                    unavailableNode = addUnavailableNode(dftConstF, false);
                }

			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTS(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstS, bool isRepresentative) {
//				storm::gspn::Place placeCONSTSFailed;
//				placeCONSTSFailed.setName(dftConstS->name() + STR_FAILED);
//				placeCONSTSFailed.setNumberOfInitialTokens(0);
//				placeCONSTSFailed.setCapacity(0); // It cannot contain a token, because it cannot fail.
//				mGspn.addPlace(placeCONSTSFailed);
			}
//			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPDEP(std::shared_ptr<storm::storage::DFTDependency<ValueType> const> dftDependency) {

			}
            
            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawSeq(std::shared_ptr<storm::storage::DFTSeq<ValueType> const> dftSeq) {
                STORM_LOG_THROW(dftSeq->allChildrenBEs(), storm::exceptions::NotImplementedException, "Sequence enforcers with gates as children are currently not supported");
                uint64_t j = 0;
                uint64_t tEnable = 0;
                uint64_t nextPlace = 0;
                for(auto const& child : dftSeq->children()) {
                    nextPlace = builder.addPlace(defaultCapacity, j==0 ? 1 : 0, dftSeq->name() + "_next_" + child->name());
                    if (j>0) {
                        builder.addOutputArc(tEnable, nextPlace);
                    }
                    tEnable = builder.addImmediateTransition(defaultPriority, 0.0, dftSeq->name() + "_unblock_" +child->name() );
                    builder.addInputArc(nextPlace, tEnable);
                    builder.addInputArc(disabledNodes.at(child->id()), tEnable);
                    if (j>0) {
                        builder.addInputArc(failedNodes.at(dftSeq->children().at(j-1)->id()), tEnable);
                    }
                    ++j;
                }
                
            }
            
            template<typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::addUnavailableNode(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement, bool initialAvailable) {
                uint64_t unavailableNode = builder.addPlace(defaultCapacity, initialAvailable ? 0 : 1, dftElement->name() + "_unavailable");
                assert(unavailableNode != 0);
                unavailableNodes.emplace(dftElement->id(), unavailableNode);
                return unavailableNode;
            }
            
            template<typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::addDisabledPlace(std::shared_ptr<const storm::storage::DFTBE<ValueType> > dftBe) {
                uint64_t disabledNode = builder.addPlace(dftBe->nrRestrictions(), dftBe->nrRestrictions(), dftBe->name() + "_dabled");
                disabledNodes.emplace(dftBe->id(), disabledNode);
                return disabledNode;
            }
//			
			
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
                return mDft.maxRank() - dftElement->rank() + 2;
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
            gspn::GSPN* DftToGspnTransformator<ValueType>::obtainGSPN() {
                return builder.buildGspn();
            }
			
            // Explicitly instantiate the class.
            template class DftToGspnTransformator<double>;
            

    #ifdef STORM_HAVE_CARL
            // template class DftToGspnTransformator<storm::RationalFunction>;
    #endif

        } // namespace dft
    } // namespace transformations
} // namespace storm


