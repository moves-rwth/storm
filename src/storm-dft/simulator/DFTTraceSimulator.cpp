#include "DFTTraceSimulator.h"

namespace storm {
    namespace dft {
        namespace simulator {

            template<typename ValueType>
            DFTTraceSimulator<ValueType>::DFTTraceSimulator(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTStateGenerationInfo const& stateGenerationInfo, boost::mt19937& randomGenerator) : dft(dft), stateGenerationInfo(stateGenerationInfo), generator(dft, stateGenerationInfo), randomGenerator(randomGenerator) {
                // Set initial state
                state = generator.createInitialState();
            }

            template<typename ValueType>
            void DFTTraceSimulator<ValueType>::setRandomNumberGenerator(boost::mt19937& randomNumberGenerator) {
                this->randomGenerator = randomNumberGenerator;
            }

            template<typename ValueType>
            void DFTTraceSimulator<ValueType>::resetToInitial() {
                state = generator.createInitialState();;
            }

            template<typename ValueType>
            typename DFTTraceSimulator<ValueType>::DFTStatePointer DFTTraceSimulator<ValueType>::getCurrentState() const {
                return state;
            }

            template<typename ValueType>
            double DFTTraceSimulator<ValueType>::randomStep() {
                auto iterFailable = state->getFailableElements().begin();

                // Check for absorbing state:
                // - either no relevant event remains (i.e., all relevant events have failed already), or
                // - no BE can fail
                if (!state->hasOperationalRelevantEvent() || iterFailable == state->getFailableElements().end()) {
                    STORM_LOG_TRACE("No sucessor states available for " << state->getId());
                    return -1;
                }

                // Get all failable elements
                if (iterFailable.isFailureDueToDependency()) {
                    if (iterFailable.isConflictingDependency()) {
                        // We take the first dependeny to resolve the non-determinism
                        STORM_LOG_WARN("Non-determinism present! We take the dependency with the lowest id");
                    }
                    STORM_LOG_TRACE("Let dependency " << *iterFailable.getFailBE(dft).second << " fail");
                    bool res = step(iterFailable);
                    return res ? 0 : -1;
                } else {
                    // Consider all "normal" BE failures
                    // Initialize with first BE
                    storm::dft::storage::FailableElements::const_iterator nextFail = iterFailable;
                    double rate = state->getBERate(iterFailable.getFailBE(dft).first->id());
                    storm::utility::ExponentialDistributionGenerator rateGenerator(rate);
                    double smallestTimebound = rateGenerator.random(randomGenerator);
                    ++iterFailable;

                    // Consider all other BEs and find the one which fails first
                    for (; iterFailable != state->getFailableElements().end(); ++iterFailable) {
                        auto nextBE = iterFailable.getFailBE(dft).first;
                        rate = state->getBERate(nextBE->id());
                        rateGenerator = storm::utility::ExponentialDistributionGenerator(rate);
                        double timebound = rateGenerator.random(randomGenerator);
                        if (timebound < smallestTimebound) {
                            // BE fails earlier -> use as nextFail
                            nextFail = iterFailable;
                            smallestTimebound = timebound;
                        }
                    }
                    STORM_LOG_TRACE("Let BE " << *nextFail.getFailBE(dft).first << "fail after time " << smallestTimebound);
                    bool res = step(nextFail);
                    return res ? smallestTimebound : -1;
                }
            }

            template<typename ValueType>
            bool DFTTraceSimulator<ValueType>::step(storm::dft::storage::FailableElements::const_iterator nextFailElement) {
                if (nextFailElement == state->getFailableElements().end()) {
                    return false;
                }

                auto nextBEPair = nextFailElement.getFailBE(dft);
                auto newState = generator.createSuccessorState(state, nextBEPair.first, nextBEPair.second);

                // TODO handle PDEP
                
                if(newState->isInvalid() || newState->isTransient()) {
                    STORM_LOG_TRACE("Step is invalid because new state " << (newState->isInvalid() ? "it is invalid" : "the transient fault is ignored"));
                    return false;
                }

                state = newState;
                return true;
            }

            template<typename ValueType>
            bool DFTTraceSimulator<ValueType>::simulateCompleteTrace(double timebound) {
                resetToInitial();
                double time = 0;
                while (time <= timebound) {
                    // Check whether DFT failed within timebound
                    if (state->hasFailed(dft.getTopLevelIndex())) {
                        STORM_LOG_TRACE("DFT has failed after " << time);
                        return true;
                    }

                    // Generate next state
                    double res = randomStep();
                    STORM_LOG_TRACE("Current state: " << dft.getStateString(state));
                    if (res < 0) {
                        // No next state can be reached
                        STORM_LOG_TRACE("No next state possible in state " << dft.getStateString(state));
                        return false;
                    }
                    time += res;
                }
                // Time is up
                return false;
            } 

            template class DFTTraceSimulator<double>;
        }
    }
}
