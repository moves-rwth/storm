#include "src/transformations/dft/DftToGspnTransformator.h"
#include "src/exceptions/NotImplementedException.h"

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
							drawAND(dftElement->name()); 
							break;
						case storm::storage::DFTElementType::OR:
							drawOR(dftElement->name(), 2); // TODO: set parameters correctly.
							break;
						case storm::storage::DFTElementType::VOT:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a VOT is not yet implemented.");
							break;
						case storm::storage::DFTElementType::PAND:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a PAND is not yet implemented.");
							break;
						case storm::storage::DFTElementType::SPARE:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a SPARE is not yet implemented.");
							break;
						case storm::storage::DFTElementType::POR:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a POR is not yet implemented.");
							break;
						case storm::storage::DFTElementType::SEQ:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a SEQ is not yet implemented.");
							break;
						case storm::storage::DFTElementType::MUTEX:
							STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The transformation of a MUTEX is not yet implemented.");
							break;
						case storm::storage::DFTElementType::BE:
							drawBE(dftElement->name(), true, 0.5, 0.25); // TODO: set parameters correctly.
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
            
            template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawBE(std::string name, bool activated, double activeFailureRate, double passiveFailureRate) {
				storm::gspn::Place placeBEActivated;
				placeBEActivated.setName(name + "_activated");
				placeBEActivated.setNumberOfInitialTokens(activated ? 1 : 0);
				mGspn.addPlace(placeBEActivated);
				
				storm::gspn::Place placeBEFailed;
				placeBEFailed.setName(name + "_failed");
				placeBEFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeBEFailed);
				
				storm::gspn::TimedTransition<double> timedTransitionActiveFailure;
				timedTransitionActiveFailure.setName(name + "_activeFailure");
				timedTransitionActiveFailure.setPriority(1);
				timedTransitionActiveFailure.setRate(activeFailureRate);
				timedTransitionActiveFailure.setInputArcMultiplicity(placeBEActivated, 1);
				timedTransitionActiveFailure.setInhibitionArcMultiplicity(placeBEFailed, 1);
				timedTransitionActiveFailure.setOutputArcMultiplicity(placeBEActivated, 1);
				timedTransitionActiveFailure.setOutputArcMultiplicity(placeBEFailed, 1);
				mGspn.addTimedTransition(timedTransitionActiveFailure);
				
				storm::gspn::TimedTransition<double> timedTransitionPassiveFailure;
				timedTransitionPassiveFailure.setName(name + "_passiveFailure");
				timedTransitionPassiveFailure.setPriority(1);
				timedTransitionPassiveFailure.setRate(passiveFailureRate);
				timedTransitionPassiveFailure.setInhibitionArcMultiplicity(placeBEActivated, 1);
				timedTransitionPassiveFailure.setInhibitionArcMultiplicity(placeBEFailed, 1);
				timedTransitionPassiveFailure.setOutputArcMultiplicity(placeBEFailed, 1);
				mGspn.addTimedTransition(timedTransitionPassiveFailure);
			}
			
			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawAND(std::string name) {
				storm::gspn::Place placeANDFailed;
				placeANDFailed.setName(name + "_failed");
				placeANDFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeANDFailed);
				
				storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionANDFailing;
				immediateTransitionANDFailing.setName(name + "_failing");
				immediateTransitionANDFailing.setPriority(1);
				immediateTransitionANDFailing.setWeight(0.0);
				immediateTransitionANDFailing.setInhibitionArcMultiplicity(placeANDFailed, 1);
				immediateTransitionANDFailing.setOutputArcMultiplicity(placeANDFailed, 1);
				mGspn.addImmediateTransition(immediateTransitionANDFailing);
			}

			template <typename ValueType>
            void DftToGspnTransformator<ValueType>::drawOR(std::string name, std::size_t numberOfChildren) {
				storm::gspn::Place placeORFailed;
				placeORFailed.setName(name + "_failed");
				placeORFailed.setNumberOfInitialTokens(0);
				mGspn.addPlace(placeORFailed);
				
				for (std::size_t i = 0; i < numberOfChildren; i++) {
					storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> immediateTransitionORFailing;
					immediateTransitionORFailing.setName(name + std::to_string((int)i) + "_failing");
					immediateTransitionORFailing.setPriority(1);
					immediateTransitionORFailing.setWeight(0.0);
					immediateTransitionORFailing.setInhibitionArcMultiplicity(placeORFailed, 1);
					immediateTransitionORFailing.setOutputArcMultiplicity(placeORFailed, 1);
					mGspn.addImmediateTransition(immediateTransitionORFailing);
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


