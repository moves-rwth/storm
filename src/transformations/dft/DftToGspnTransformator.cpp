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
		
				// TODO: Do I need to check if every DFT element has an unique name?
				// TODO: GSPN elements are picked by their name in method drawGSPNConnections(), so this might cause problems...
				
				// Loop through every DFT element and draw them as a GSPN.
				drawGSPNElements();
				
				// When all DFT elements are drawn, draw the connections between them.
				drawGSPNConnections();
		
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
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a SEQ is not yet implemented.");
							break;
						case storm::storage::DFTElementType::MUTEX:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a MUTEX is not yet implemented.");
							break;
						case storm::storage::DFTElementType::BE:
							drawBE(std::static_pointer_cast<storm::storage::DFTBE<ValueType> const>(dftElement));
							break;
						case storm::storage::DFTElementType::CONSTF:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a CONSTF is not yet implemented.");
							break;
						case storm::storage::DFTElementType::CONSTS:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a CONSTS is not yet implemented.");
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
				placeBEActivated.setNumberOfInitialTokens(false ? 1 : 0); // TODO: How can I check if BE is activated?
				mGspn.addPlace(placeBEActivated);
				
				storm::gspn::Place placeBEFailed;
				placeBEFailed.setName(dftBE->name() + "_failed");
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
				placeANDFailed.setName(dftAnd->name() + "_failed");
				placeANDFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeANDFailed);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionANDFailing;
				immediateTransitionANDFailing.setName(dftAnd->name() + "_failing");
				immediateTransitionANDFailing.setPriority(1);
				immediateTransitionANDFailing.setWeight(0.0);
				immediateTransitionANDFailing.setInhibitionArcMultiplicity(placeANDFailed, 1);
				immediateTransitionANDFailing.setOutputArcMultiplicity(placeANDFailed, 1);
				mGspn.addImmediateTransition(immediateTransitionANDFailing);
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawOR(std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr) {
				storm::gspn::Place placeORFailed;
				placeORFailed.setName(dftOr->name() + "_failed");
				placeORFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeORFailed);
				
				auto children = dftOr->children();
				for (std::size_t i = 0; i < dftOr->nrChildren(); i++) {
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionORFailing;
					immediateTransitionORFailing.setName(dftOr->name() + "_" + children[i]->name() + "_failing");
					immediateTransitionORFailing.setPriority(1);
					immediateTransitionORFailing.setWeight(0.0);
					immediateTransitionORFailing.setInhibitionArcMultiplicity(placeORFailed, 1);
					immediateTransitionORFailing.setOutputArcMultiplicity(placeORFailed, 1);
					mGspn.addImmediateTransition(immediateTransitionORFailing);
				}
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawVOT(std::shared_ptr<storm::storage::DFTVot<ValueType> const> dftVot) {
				STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a VOT is not yet implemented.");
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPAND(std::shared_ptr<storm::storage::DFTPand<ValueType> const> dftPand) {
				STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a PAND is not yet implemented.");
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawSPARE(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare) {
				STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a SPARE is not yet implemented.");
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawPOR(std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor) {
				STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a POR is not yet implemented.");
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
								auto andEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_failing");
								auto childExit = mGspn.getPlace(child->name() + "_failed");
								if (andEntry.first && childExit.first) { // Only add arcs if the objects have been found.
									andEntry.second->setInputArcMultiplicity(childExit.second, 1);
									andEntry.second->setOutputArcMultiplicity(childExit.second, 1);
								}
								break;
							}
							case storm::storage::DFTElementType::OR:
							{
								auto orEntry = mGspn.getImmediateTransition(mDft.getElement(parents[j])->name() + "_" + child->name() + "_failing");
								auto childExit = mGspn.getPlace(child->name() + "_failed");
								if (orEntry.first && childExit.first) { // Only add arcs if the objects have been found.
									orEntry.second->setInputArcMultiplicity(childExit.second, 1);
									orEntry.second->setOutputArcMultiplicity(childExit.second, 1);
								}
								break;
							}
							case storm::storage::DFTElementType::VOT:
								break;
							case storm::storage::DFTElementType::PAND:
								break;
							case storm::storage::DFTElementType::SPARE:
								break;
							case storm::storage::DFTElementType::POR:
								break;
							case storm::storage::DFTElementType::SEQ:
								break;
							case storm::storage::DFTElementType::MUTEX:
								break;
							case storm::storage::DFTElementType::BE:
							{
								// The parent is never a Basic Event.
								break;
							}
							case storm::storage::DFTElementType::CONSTF:
								break;
							case storm::storage::DFTElementType::CONSTS:
								break;
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


