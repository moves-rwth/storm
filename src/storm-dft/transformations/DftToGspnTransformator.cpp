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
            void DftToGspnTransformator<ValueType>::transform(bool smart) {
                this->smart = smart;
                builder.setGspnName("DftToGspnTransformation");
				
				// Loop through every DFT element and draw them as a GSPN.
				drawGSPNElements();

				// Draw restrictions into the GSPN (i.e. SEQ or MUTEX).
                // TODO
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
							STORM_LOG_ASSERT(false, "DFT type " << dftElement->type() << " unknown.");
							break;
					}
				}
                
			}
            
            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawBE(std::shared_ptr<storm::storage::DFTBE<ValueType> const> dftBE, bool isRepresentative) {
                uint64_t beActive = builder.addPlace(defaultCapacity, isActiveInitially(dftBE) ? 1 : 0, dftBE->name() + STR_ACTIVATED);
                activeNodes.emplace(dftBE->id(), beActive);
                uint64_t beFailed = builder.addPlace(defaultCapacity, 0, dftBE->name() + STR_FAILED);

                double xcenter = mDft.getElementLayoutInfo(dftBE->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftBE->id()).y;
                builder.setPlaceLayoutInfo(beActive, storm::gspn::LayoutInfo(xcenter - 3.0, ycenter));
                builder.setPlaceLayoutInfo(beFailed, storm::gspn::LayoutInfo(xcenter + 3.0, ycenter));
                
                uint64_t disabledNode = 0;
                if (!smart || dftBE->nrRestrictions() > 0) {
                    disabledNode = addDisabledPlace(dftBE, storm::gspn::LayoutInfo(xcenter-9.0, ycenter));
                }
                
                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftBE, storm::gspn::LayoutInfo(xcenter+9.0, ycenter));
                }
                
                assert(failedNodes.size() == dftBE->id());
                failedNodes.push_back(beFailed);
                uint64_t tActive = builder.addTimedTransition(defaultPriority, dftBE->activeFailureRate(), dftBE->name() + "_activeFailing");
                builder.setTransitionLayoutInfo(tActive, storm::gspn::LayoutInfo(xcenter, ycenter + 3.0));
                builder.addInputArc(beActive, tActive);
                builder.addInhibitionArc(beFailed, tActive);
                builder.addOutputArc(tActive, beActive);
                builder.addOutputArc(tActive, beFailed);
                uint64_t tPassive = builder.addTimedTransition(defaultPriority, dftBE->passiveFailureRate(), dftBE->name() + "_passiveFailing");
                builder.setTransitionLayoutInfo(tPassive, storm::gspn::LayoutInfo(xcenter, ycenter - 3.0));
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
            }
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawAND(std::shared_ptr<storm::storage::DFTAnd<ValueType> const> dftAnd, bool isRepresentative) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftAnd->name() + STR_FAILED);
                assert(failedNodes.size() == dftAnd->id());
                failedNodes.push_back(nodeFailed);

                double xcenter = mDft.getElementLayoutInfo(dftAnd->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftAnd->id()).y;
                builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter, ycenter-3.0));

                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftAnd, storm::gspn::LayoutInfo(xcenter+6.0, ycenter-3.0));
                }

                
                uint64_t tAndFailed = builder.addImmediateTransition( getFailPriority(dftAnd), 0.0, dftAnd->name() + STR_FAILING );
                builder.setTransitionLayoutInfo(tAndFailed, storm::gspn::LayoutInfo(xcenter, ycenter+3.0));
                builder.addInhibitionArc(nodeFailed, tAndFailed);
                builder.addOutputArc(tAndFailed, nodeFailed);
                if (!smart || isRepresentative) {
                    builder.addOutputArc(tAndFailed, unavailableNode);
                }
                for (auto const& child : dftAnd->children()) {
                    assert(failedNodes.size() > child->id());
                    builder.addInputArc(failedNodes[child->id()], tAndFailed);
                    builder.addOutputArc(tAndFailed, failedNodes[child->id()]);
                }
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawOR(std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr, bool isRepresentative) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftOr->name() + STR_FAILED);
                assert(failedNodes.size() == dftOr->id());
                failedNodes.push_back(nodeFailed);

                double xcenter = mDft.getElementLayoutInfo(dftOr->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftOr->id()).y;
                builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter, ycenter-3.0));

                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftOr, storm::gspn::LayoutInfo(xcenter+6.0, ycenter-3.0));
                }
                
                uint64_t i = 0;
                for (auto const& child : dftOr->children()) {
                    uint64_t tNodeFailed = builder.addImmediateTransition( getFailPriority(dftOr), 0.0, dftOr->name() + STR_FAILING + std::to_string(i) );
                    builder.setTransitionLayoutInfo(tNodeFailed, storm::gspn::LayoutInfo(xcenter-5.0+i*3.0, ycenter+3.0));
                    builder.addInhibitionArc(nodeFailed, tNodeFailed);
                    builder.addOutputArc(tNodeFailed, nodeFailed);
                    if (!smart || isRepresentative) {
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
                // TODO: finish layouting
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftVot->name() + STR_FAILED);
                assert(failedNodes.size() == dftVot->id());
                failedNodes.push_back(nodeFailed);

                double xcenter = mDft.getElementLayoutInfo(dftVot->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftVot->id()).y;
                builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter, ycenter-3.0));

                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftVot, storm::gspn::LayoutInfo(xcenter+6.0, ycenter-3.0));
                }
                
                uint64_t nodeCollector = builder.addPlace(dftVot->nrChildren(), 0, dftVot->name() + "_collector");
                builder.setPlaceLayoutInfo(nodeCollector, storm::gspn::LayoutInfo(xcenter, ycenter));

                uint64_t tNodeFailed = builder.addImmediateTransition(getFailPriority(dftVot), 0.0, dftVot->name() + STR_FAILING);
                builder.addOutputArc(tNodeFailed, nodeFailed);
                if (!smart || isRepresentative) {
                    builder.addOutputArc(tNodeFailed, unavailableNode);
                }
                builder.addInhibitionArc(nodeFailed, tNodeFailed);
                builder.addInputArc(nodeCollector, tNodeFailed, dftVot->threshold());
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

                double xcenter = mDft.getElementLayoutInfo(dftPand->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftPand->id()).y;
                builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter+3.0, ycenter-3.0));

                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftPand, storm::gspn::LayoutInfo(xcenter+9.0, ycenter-3.0));
                }
                
                uint64_t tNodeFailed = builder.addImmediateTransition(getFailPriority(dftPand), 0.0,  dftPand->name() + STR_FAILING);
                builder.setTransitionLayoutInfo(tNodeFailed, storm::gspn::LayoutInfo(xcenter+3.0, ycenter+3.0));
                builder.addInhibitionArc(nodeFailed, tNodeFailed);
                builder.addOutputArc(tNodeFailed, nodeFailed);
                if (!smart || isRepresentative) {
                    builder.addOutputArc(tNodeFailed, unavailableNode);
                }

                if(dftPand->isInclusive()) {
                    // Inclusive PAND
                    uint64_t nodeFS = builder.addPlace(defaultCapacity, 0, dftPand->name() + STR_FAILSAVE);
                    builder.setPlaceLayoutInfo(nodeFS, storm::gspn::LayoutInfo(xcenter-3.0, ycenter-3.0));

                    builder.addInhibitionArc(nodeFS, tNodeFailed);
                    // Transition for failed
                    for (auto const& child : dftPand->children()) {
                        builder.addInputArc(failedNodes[child->id()], tNodeFailed);
                        builder.addOutputArc(tNodeFailed, failedNodes[child->id()]);
                    }
                    // Transitions for fail-safe
                    for (uint64_t j = 1; j < dftPand->nrChildren(); ++j) {
                        uint64_t tfs = builder.addImmediateTransition(getFailPriority(dftPand), 0.0, dftPand->name() + STR_FAILSAVING + std::to_string(j));
                        builder.setTransitionLayoutInfo(tfs, storm::gspn::LayoutInfo(xcenter-6.0+j*3.0, ycenter+3.0));

                        builder.addInputArc(failedNodes[dftPand->children().at(j)->id()], tfs);
                        builder.addOutputArc(tfs, failedNodes[dftPand->children().at(j)->id()]);
                        builder.addInhibitionArc(failedNodes[dftPand->children().at(j-1)->id()], tfs);
                        builder.addOutputArc(tfs, nodeFS);
                        builder.addInhibitionArc(nodeFS, tfs);
                        
                    }
                } else {
                    // Exclusive PAND
                    uint64_t fi = 0;
                    uint64_t tn = 0;
                    for(uint64_t j = 0; j < dftPand->nrChildren(); ++j) {
                        auto const& child = dftPand->children()[j];
                        if (j > 0) {
                            builder.addInhibitionArc(failedNodes.at(child->id()), tn);
                        }
                        if (j != dftPand->nrChildren() - 1) {
                            // Not last child
                            tn = builder.addImmediateTransition(getFailPriority(dftPand), 0.0, dftPand->name() + STR_FAILING + "_" +std::to_string(j));
                            builder.setTransitionLayoutInfo(tn, storm::gspn::LayoutInfo(xcenter-3.0, ycenter+3.0));
                        } else {
                            // Last child
                            tn = tNodeFailed;
                        }
                        builder.addInputArc(failedNodes.at(child->id()), tn);
                        builder.addOutputArc(tn, failedNodes.at(child->id()));
                        if (j > 0) {
                            builder.addInputArc(fi, tn);
                        }
                        if (j != dftPand->nrChildren() - 1) {
                            fi = builder.addPlace(defaultCapacity, 0, dftPand->name() + "_F_" + std::to_string(j));
                            builder.setPlaceLayoutInfo(fi, storm::gspn::LayoutInfo(xcenter-3.0+j*3.0, ycenter));
                            builder.addOutputArc(tn, fi);
                        }
                        
                    }
                }
            }

            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor, bool isRepresentative) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftPor->name() + STR_FAILED);
                failedNodes.push_back(nodeFailed);

                double xcenter = mDft.getElementLayoutInfo(dftPor->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftPor->id()).y;
                builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter+3.0, ycenter-3.0));

                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftPor, storm::gspn::LayoutInfo(xcenter+9.0, ycenter-3.0));
                }

                uint64_t tfail = builder.addImmediateTransition(getFailPriority(dftPor), 0.0, dftPor->name() + STR_FAILING);
                builder.setTransitionLayoutInfo(tfail, storm::gspn::LayoutInfo(xcenter+3.0, ycenter+3.0));
                builder.addOutputArc(tfail, nodeFailed);
                builder.addInhibitionArc(nodeFailed, tfail);

                builder.addInputArc(failedNodes.at(dftPor->children().front()->id()), tfail);
                builder.addOutputArc(tfail, failedNodes.at(dftPor->children().front()->id()));

                if(!smart || isRepresentative) {
                    builder.addOutputArc(tfail, unavailableNode);
                }

                if(dftPor->isInclusive()) {
                    // Inclusive POR
                    uint64_t nodeFS = builder.addPlace(defaultCapacity, 0, dftPor->name() + STR_FAILSAVE);
                    builder.setPlaceLayoutInfo(nodeFS, storm::gspn::LayoutInfo(xcenter-3.0, ycenter-3.0));

                    builder.addInhibitionArc(nodeFS, tfail);
                    uint64_t j = 0;
                    for (auto const& child : dftPor->children()) {
                        if(j > 0) {
                            uint64_t tfailsf = builder.addImmediateTransition(getFailPriority(dftPor), 0.0, dftPor->name() + STR_FAILSAVING + std::to_string(j));
                            builder.setTransitionLayoutInfo(tfailsf, storm::gspn::LayoutInfo(xcenter-3.0+j*3.0, ycenter+3.0));
                            builder.addInputArc(failedNodes.at(child->id()), tfailsf);
                            builder.addOutputArc(tfailsf, failedNodes.at(child->id()));
                            builder.addOutputArc(tfailsf, nodeFS);
                            builder.addInhibitionArc(nodeFS, tfailsf);
                            builder.addInhibitionArc(failedNodes.at(dftPor->children().front()->id()), tfailsf);
                        }
                        ++j;
                    }
                } else {
                    // Exclusive POR
                    uint64_t j = 0;
                    for (auto const& child : dftPor->children()) {
                        if(j > 0) {
                            builder.addInhibitionArc(failedNodes.at(child->id()), tfail);
                        }
                        ++j;
                    }

                }
                
            }

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawSPARE(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare, bool isRepresentative) {
                uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftSpare->name() + STR_FAILED);
                failedNodes.push_back(nodeFailed);

                double xcenter = mDft.getElementLayoutInfo(dftSpare->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftSpare->id()).y;
                builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter+10.0, ycenter-8.0));

                uint64_t unavailableNode = 0;
                if (!smart || isRepresentative) {
                    unavailableNode = addUnavailableNode(dftSpare, storm::gspn::LayoutInfo(xcenter+16.0, ycenter-8.0));
                }
                uint64_t spareActive = builder.addPlace(defaultCapacity, isActiveInitially(dftSpare) ? 1 : 0, dftSpare->name() + STR_ACTIVATED);
                builder.setPlaceLayoutInfo(spareActive, storm::gspn::LayoutInfo(xcenter-20.0, ycenter-12.0));
                activeNodes.emplace(dftSpare->id(), spareActive);
                
                std::vector<uint64_t> nextclTransitions;
                std::vector<uint64_t> nextconsiderTransitions;
                uint64_t j = 0;
                for(auto const& child : dftSpare->children()) {
                    // Consider next child
                    size_t nodeConsider = builder.addPlace(defaultCapacity, j == 0 ? 1 : 0, dftSpare->name()+ "_consider_" + child->name());
                    builder.setPlaceLayoutInfo(nodeConsider, storm::gspn::LayoutInfo(xcenter-15.0+j*14.0, ycenter-8.0));

                    if (j > 0) {
                        // Set output transition from previous next_claim
                        builder.addOutputArc(nextclTransitions.back(), nodeConsider);
                        // Set output transition from previous cannot_claim
                        builder.addOutputArc(nextconsiderTransitions.back(), nodeConsider);
                    }

                    // Cannot claim child
                    uint64_t tnextconsider = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0, dftSpare->name() + "_cannot_claim_" + child->name());
                    builder.setTransitionLayoutInfo(tnextconsider, storm::gspn::LayoutInfo(xcenter-7.0+j*14.0, ycenter-8.0));
                    builder.addInputArc(nodeConsider, tnextconsider);
                    builder.addInputArc(unavailableNodes.at(child->id()), tnextconsider);
                    builder.addOutputArc(tnextconsider, unavailableNodes.at(child->id()));
                    nextconsiderTransitions.push_back(tnextconsider);

                    // Claimed child
                    size_t nodeCUC = builder.addPlace(defaultCapacity, 0, dftSpare->name() + "_claimed_" + child->name());
                    builder.setPlaceLayoutInfo(nodeCUC, storm::gspn::LayoutInfo(xcenter-15.0+j*14.0, ycenter+5.0));
                    uint64_t tclaim = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0, dftSpare->name() + "_claim_" + child->name());
                    builder.setTransitionLayoutInfo(tclaim, storm::gspn::LayoutInfo(xcenter-15.0+j*14.0, ycenter));
                    builder.addInhibitionArc(unavailableNodes.at(child->id()), tclaim);
                    builder.addInputArc(nodeConsider, tclaim);
                    builder.addOutputArc(tclaim, nodeCUC);
                    builder.addOutputArc(tclaim, unavailableNodes.at(child->id()));

                    // Claim next
                    uint64_t tnextcl = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0, dftSpare->name() + "_next_claim_" + std::to_string(j));
                    builder.setTransitionLayoutInfo(tnextcl, storm::gspn::LayoutInfo(xcenter-7.0+j*14.0, ycenter+5.0));
                    builder.addInputArc(nodeCUC, tnextcl);
                    builder.addInputArc(failedNodes.at(child->id()), tnextcl);
                    builder.addOutputArc(tnextcl, failedNodes.at(child->id()));
                    nextclTransitions.push_back(tnextcl);

                    ++j;
                    // Activate all nodes in spare module
                    uint64_t l = 0;
                    for (uint64_t k : mDft.module(child->id())) {
                        uint64_t tactive = builder.addImmediateTransition(defaultPriority, 0.0, dftSpare->name() + "_activate_" + std::to_string(j) + "_" +  std::to_string(k));
                        builder.setTransitionLayoutInfo(tactive, storm::gspn::LayoutInfo(xcenter-18.0+(j+l)*3, ycenter-12.0));
                        builder.addInhibitionArc(activeNodes.at(k), tactive);
                        builder.addInputArc(nodeCUC, tactive);
                        builder.addInputArc(spareActive, tactive);
                        builder.addOutputArc(tactive, nodeCUC);
                        builder.addOutputArc(tactive, spareActive);
                        builder.addOutputArc(tactive, activeNodes.at(k));
                        ++l;
                    }
                }

                // Set arcs to failed
                builder.addOutputArc(nextconsiderTransitions.back(), nodeFailed);
                builder.addOutputArc(nextclTransitions.back(), nodeFailed);

                if (!smart || isRepresentative) {
                    builder.addOutputArc(nextconsiderTransitions.back(), unavailableNode);
                    builder.addOutputArc(nextclTransitions.back(), unavailableNode);
                }
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawCONSTF(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstF, bool isRepresentative) {
			    failedNodes.push_back(builder.addPlace(defaultCapacity, 1, dftConstF->name() + STR_FAILED));
                uint64_t unavailableNode = 0;
                if (isRepresentative) {
                    // TODO set position
                    unavailableNode = addUnavailableNode(dftConstF, storm::gspn::LayoutInfo(0, 0), false);
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
                double xcenter = mDft.getElementLayoutInfo(dftDependency->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftDependency->id()).y;

                if (!smart) {
                    uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftDependency->name() + STR_FAILED);
                    failedNodes.push_back(nodeFailed);
                    builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter+10.0, ycenter-8.0));
                    addUnavailableNode(dftDependency, storm::gspn::LayoutInfo(xcenter+16.0, ycenter-8.0));
                }

                uint64_t coinPlace = builder.addPlace(defaultCapacity, 1, dftDependency->name() + "_coin");
                builder.setPlaceLayoutInfo(coinPlace, storm::gspn::LayoutInfo(xcenter-5.0, ycenter+2.0));
                uint64_t t1 = builder.addImmediateTransition(defaultPriority, 0.0, dftDependency->name() + "_start_flip");
                
                builder.addInputArc(coinPlace, t1);
                builder.addInputArc(failedNodes.at(dftDependency->triggerEvent()->id()), t1);
                builder.addOutputArc(t1, failedNodes.at(dftDependency->triggerEvent()->id()));
                uint64_t forwardPlace = builder.addPlace(defaultCapacity, 0, dftDependency->name() + "_forward");
                builder.setPlaceLayoutInfo(forwardPlace, storm::gspn::LayoutInfo(xcenter+1.0, ycenter+2.0));
                
                if (!smart || dftDependency->probability() < 1.0) {
                    uint64_t flipPlace = builder.addPlace(defaultCapacity, 0, dftDependency->name() + "_flip");
                    builder.addOutputArc(t1, flipPlace);
                    
                    builder.setPlaceLayoutInfo(flipPlace, storm::gspn::LayoutInfo(xcenter-2.0, ycenter+2.0));
                    uint64_t t2 = builder.addImmediateTransition(defaultPriority, dftDependency->probability(), "_win_flip");
                    builder.addInputArc(flipPlace, t2);
                    builder.addOutputArc(t2, forwardPlace);
                    if (dftDependency->probability() < 1.0) {
                        uint64_t t3 = builder.addImmediateTransition(defaultPriority, 1 - dftDependency->probability(), "_loose_flip");
                        builder.addInputArc(flipPlace, t3);
                    }
                } else {
                    builder.addOutputArc(t1, forwardPlace);
                }
                for(auto const& depEv : dftDependency->dependentEvents()) {
                    uint64_t tx = builder.addImmediateTransition(defaultPriority, 0.0, dftDependency->name() + "_propagate_" + depEv->name());
                    builder.addInputArc(forwardPlace, tx);
                    builder.addOutputArc(tx, forwardPlace);
                    builder.addOutputArc(tx, failedNodes.at(depEv->id()));
                    builder.addInhibitionArc(failedNodes.at(depEv->id()), tx);
                    if (!smart || depEv->nrRestrictions() > 0) {
                        builder.addInhibitionArc(disabledNodes.at(depEv->id()), tx);
                    }
                    if (!smart || mDft.isRepresentative(depEv->id())) {
                        builder.addOutputArc(tx, unavailableNodes.at(depEv->id()));
                    }
                }
			}
            
            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawSeq(std::shared_ptr<storm::storage::DFTSeq<ValueType> const> dftSeq) {
                STORM_LOG_THROW(dftSeq->allChildrenBEs(), storm::exceptions::NotImplementedException, "Sequence enforcers with gates as children are currently not supported");
                double xcenter = mDft.getElementLayoutInfo(dftSeq->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftSeq->id()).y;

                if (!smart) {
                    uint64_t nodeFailed = builder.addPlace(defaultCapacity, 0, dftSeq->name() + STR_FAILED);
                    failedNodes.push_back(nodeFailed);
                    builder.setPlaceLayoutInfo(nodeFailed, storm::gspn::LayoutInfo(xcenter+10.0, ycenter-8.0));
                    addUnavailableNode(dftSeq, storm::gspn::LayoutInfo(xcenter+16.0, ycenter-8.0));
                }

                uint64_t j = 0;
                uint64_t tEnable = 0;
                uint64_t nextPlace = 0;
                for(auto const& child : dftSeq->children()) {
                    nextPlace = builder.addPlace(defaultCapacity, j==0 ? 1 : 0, dftSeq->name() + "_next_" + child->name());
                    builder.setPlaceLayoutInfo(nextPlace, storm::gspn::LayoutInfo(xcenter-5.0+j*3.0, ycenter-3.0));
                    if (j>0) {
                        builder.addOutputArc(tEnable, nextPlace);
                    }
                    tEnable = builder.addImmediateTransition(defaultPriority, 0.0, dftSeq->name() + "_unblock_" +child->name());
                    builder.setTransitionLayoutInfo(tEnable, storm::gspn::LayoutInfo(xcenter-5.0+j*3.0, ycenter+3.0));

                    builder.addInputArc(nextPlace, tEnable);
                    builder.addInputArc(disabledNodes.at(child->id()), tEnable);
                    if (j>0) {
                        builder.addInputArc(failedNodes.at(dftSeq->children().at(j-1)->id()), tEnable);
                    }
                    ++j;
                }
                
            }
            
            template<typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::addUnavailableNode(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement, storm::gspn::LayoutInfo const& layoutInfo, bool initialAvailable) {
                uint64_t unavailableNode = builder.addPlace(defaultCapacity, initialAvailable ? 0 : 1, dftElement->name() + "_unavail");
                assert(unavailableNode != 0);
                unavailableNodes.emplace(dftElement->id(), unavailableNode);
                builder.setPlaceLayoutInfo(unavailableNode, layoutInfo);
                return unavailableNode;
            }
            
            template<typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::addDisabledPlace(std::shared_ptr<const storm::storage::DFTBE<ValueType> > dftBe, storm::gspn::LayoutInfo const& layoutInfo) {
                uint64_t disabledNode = builder.addPlace(dftBe->nrRestrictions(), dftBe->nrRestrictions(), dftBe->name() + "_dabled");
                assert(disabledNode != 0);
                disabledNodes.emplace(dftBe->id(), disabledNode);
                builder.setPlaceLayoutInfo(disabledNode, layoutInfo);
                return disabledNode;
            }
			
			template <typename ValueType>
            bool DftToGspnTransformator<ValueType>::isActiveInitially(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement) {
				// If element is in the top module, return true.
                return !mDft.hasRepresentant(dftElement->id());
			}
			
			template <typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::getFailPriority(std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement)
			{
                // Temporariliy use one priority for all
                return defaultPriority;
                //return mDft.maxRank() - dftElement->rank() + 2;
			}

			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawGSPNRestrictions() {
                // TODO
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


