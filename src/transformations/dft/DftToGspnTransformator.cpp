#include "src/transformations/dft/DftToGspnTransformator.h"
#include "src/exceptions/NotImplementedException.h"
#include <memory>

namespace storm {
    namespace transformations {
        namespace dft {

            template <typename ValueType>
            DftToGspnTransformator<ValueType>::DftToGspnTransformator(storm::storage::DFT<ValueType> const& dft) : mDft(dft) {
                // Intentionally left empty
            }

            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::transform() {
                mGspn = storm::gspn::GSPN();
				mGspn.setName("DftToGspnTransformation");
		
				/*
				// Testing place.
				storm::gspn::Place place;
				place.setName("Left");
				place.setID(0);
				place.setNumberOfInitialTokens(2);
				mGspn.addPlace(place);
				
				storm::gspn::Place place2;
				place2.setName("Right");
				place2.setID(1);
				place2.setNumberOfInitialTokens(0);
				mGspn.addPlace(place2);
				
				// Testing transition.
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransition;
				immediateTransition.setName("ImmediateTransition");
				immediateTransition.setPriority(2);
				immediateTransition.setWeight(0.0);
				immediateTransition.setInputArcMultiplicity(place, 1);
				immediateTransition.setOutputArcMultiplicity(place2, 1);
				mGspn.addImmediateTransition(immediateTransition);
				
				storm::gspn::TimedTransition<double> timedTransition;
				timedTransition.setName("TimedTransition");
				timedTransition.setPriority(1);
				timedTransition.setRate(0.3);
				timedTransition.setInhibitionArcMultiplicity(place2, 1);
				timedTransition.setOutputArcMultiplicity(place, 1);
				mGspn.addTimedTransition(timedTransition);
				
				// Testing DFT.
				std::cout << "-------------------------------------------------------------------------" << std::endl;
				std::cout << "Number of elements in DFT: " << mDft.nrElements() << std::endl;
				std::cout << "Number of basic elements in DFT: " << mDft.nrBasicElements() << std::endl;
				std::cout << "Toplevel index of DFT: " << mDft.getTopLevelIndex() << std::endl;
				
				for (std::size_t i = 0; i < mDft.nrElements(); i++) {
					auto dftElement = mDft.getElement(i);
					std::cout << "Index: " << i 
					<< " Gate: " << dftElement->isGate() 
					<< " Dependency: " << dftElement->isDependency() 
					<< " Restriction: " << dftElement->isRestriction()
					<< " String: " << dftElement->toString() 
					<< " Name: " << dftElement->name() << std::endl;
					
					auto parents = dftElement->parentIds();
					for (std::size_t j = 0; j < parents.size(); j++) {
					std::cout << "Parents of " << j << ": " << parents[j] << std::endl;
					}
				}
				std::cout << "-------------------------------------------------------------------------" << std::endl;
				*/
		
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
		
				// Output.
				writeGspn(true);
				writeGspn(false);
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
				
				for (std::size_t i = 0; i < dftOr->nrChildren(); i++) {
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionORFailing;
					immediateTransitionORFailing.setName(dftOr->name() + std::to_string((int)i) + "_failing");
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


