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
            std::tuple<storm::dft::storage::FailableElements::const_iterator, double, bool> DFTTraceSimulator<ValueType>::randomNextFailure() {
                auto iterFailable = state->getFailableElements().begin();

                // Check for absorbing state:
                // - either no relevant event remains (i.e., all relevant events have failed already), or
                // - no BE can fail
                if (!state->hasOperationalRelevantEvent() || iterFailable == state->getFailableElements().end()) {
                    STORM_LOG_TRACE("No sucessor states available for " << state->getId());
                    return std::make_tuple(iterFailable, -1, true);
                }

                // Get all failable elements
                if (iterFailable.isFailureDueToDependency()) {
                    if (iterFailable.isConflictingDependency()) {
                        // We take the first dependency to resolve the non-determinism
                        STORM_LOG_WARN("Non-determinism present! We take the dependency with the lowest id");
                    }

                    auto dependency = iterFailable.getFailBE(dft).second;
                    if (!dependency->isFDEP()) {
                        // Flip a coin whether the PDEP is successful
                        storm::utility::BernoulliDistributionGenerator probGenerator(dependency->probability());
                        bool successful = probGenerator.random(randomGenerator);
                        return std::make_tuple(iterFailable, 0, successful);
                    }
                    STORM_LOG_TRACE("Let dependency " << *dependency << " fail");
                    return std::make_tuple(iterFailable, 0, true);
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
                    STORM_LOG_TRACE("Let BE " << *nextFail.getFailBE(dft).first << " fail after time " << smallestTimebound);
                    return std::make_tuple(nextFail, smallestTimebound, true);
                }
            }

            template<>
            std::tuple<storm::dft::storage::FailableElements::const_iterator, double, bool> DFTTraceSimulator<storm::RationalFunction>::randomNextFailure() {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Simulation not support for parametric DFTs.");
            }

            template<typename ValueType>
            double DFTTraceSimulator<ValueType>::randomStep() {
                auto retTuple = this->randomNextFailure();
                storm::dft::storage::FailableElements::const_iterator nextFailable = std::get<0>(retTuple);
                double time = std::get<1>(retTuple);
                bool successful = std::get<2>(retTuple);
                //double time = pairNextFailure.second;
                if (time < 0) {
                    return -1;
                } else {
                    // Apply next failure
                    bool res = step(nextFailable, successful);
                    return res ? time : -1;
                }
            }

            template<typename ValueType>
            bool DFTTraceSimulator<ValueType>::step(storm::dft::storage::FailableElements::const_iterator nextFailElement, bool dependencySuccessful) {
                if (nextFailElement == state->getFailableElements().end()) {
                    return false;
                }

                auto nextBEPair = nextFailElement.getFailBE(dft);
                auto newState = generator.createSuccessorState(state, nextBEPair.first, nextBEPair.second, dependencySuccessful);
                
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

            template<>
            bool DFTTraceSimulator<storm::RationalFunction>::simulateCompleteTrace(double timebound) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Simulation not support for parametric DFTs.");
            } 

            template class DFTTraceSimulator<double>;
            template class DFTTraceSimulator<storm::RationalFunction>;
        }
    }
}
