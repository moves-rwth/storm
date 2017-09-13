#include "storm/modelchecker/multiobjective/rewardbounded/EpochManager.h"

#include "storm/utility/macros.h"

#include "storm/exceptions/IllegalArgumentException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            EpochManager::EpochManager() : dimensionCount(0) {
                // Intentionally left empty
            }
            
            EpochManager::EpochManager(uint64_t dimensionCount) : dimensionCount(dimensionCount) {
                STORM_LOG_THROW(dimensionCount > 0, storm::exceptions::IllegalArgumentException, "Invoked EpochManager with zero dimension count.");
                bitsPerDimension = 64 / dimensionCount;
                if (dimensionCount == 1) {
                    dimensionBitMask = -1ull;
                } else {
                    dimensionBitMask = (1ull << bitsPerDimension) - 1;
                }
                
                if (dimensionCount * bitsPerDimension == 64ull) {
                    relevantBitsMask = -1ull;
                } else {
                    relevantBitsMask  = (1ull << (dimensionCount * bitsPerDimension)) - 1;
                }
            }
            
            uint64_t const& EpochManager::getDimensionCount() const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                return dimensionCount;
            }

            bool EpochManager::compareEpochClass(Epoch const& epoch1, Epoch const& epoch2) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                uint64_t mask = dimensionBitMask;
                for (uint64_t d = 0; d < dimensionCount; ++d) {
                    if (((epoch1 & mask) == mask) != ((epoch2 & mask) == mask)) {
                        assert(getEpochClass(epoch1) != getEpochClass(epoch2));
                        return false;
                    }
                    mask = mask << bitsPerDimension;
                }
                assert(getEpochClass(epoch1) == getEpochClass(epoch2));
                return true;
            }
            
            typename EpochManager::EpochClass EpochManager::getEpochClass(Epoch const& epoch) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                EpochClass result = 0;
                uint64_t mask = dimensionBitMask;
                for (uint64_t d  = 0; d < dimensionCount; ++d) {
                    if ((epoch & mask) == mask) {
                        ++result;
                    }
                    result = result << 1;
                    mask = mask << bitsPerDimension;
                }
                return result;
            }

            typename EpochManager::Epoch EpochManager::getSuccessorEpoch(Epoch const& epoch, Epoch const& step) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                STORM_LOG_ASSERT(!hasBottomDimension(step), "The given step has at least one bottom dimension.");

                // Start with dimension zero
                uint64_t mask = dimensionBitMask;
                uint64_t e_d = epoch & mask;
                uint64_t s_d = step & mask;
                uint64_t result = (e_d < s_d || e_d == mask) ? mask : e_d - s_d;
                
                // Consider the remaining dimensions
                for (uint64_t d = 1; d < dimensionCount; ++d) {
                    mask = mask << bitsPerDimension;
                    e_d = epoch & mask;
                    s_d = step & mask;
                    result |= ((e_d < s_d || e_d == mask) ? mask : e_d - s_d);
                }
                return result;
            }
            
            std::vector<typename EpochManager::Epoch> EpochManager::getPredecessorEpochs(Epoch const& epoch, Epoch const& step) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                STORM_LOG_ASSERT(!hasBottomDimension(step), "The given step has at least one bottom dimension.");
                std::set<Epoch> resultAsSet;
                gatherPredecessorEpochs(resultAsSet, epoch, step);
                return std::vector<Epoch>(resultAsSet.begin(), resultAsSet.end());
            }
            
            void EpochManager::gatherPredecessorEpochs(std::set<Epoch>& gatheredPredecessorEpochs, Epoch const& epoch, Epoch const& step) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                STORM_LOG_ASSERT(!hasBottomDimension(step), "The given step has at least one bottom dimension.");
                Epoch currStep = step;
                uint64_t d = 0;
                while (d < dimensionCount) {
                    Epoch predecessor = epoch;
                    for (uint64_t dPrime = 0; dPrime < dimensionCount; ++dPrime) {
                        uint64_t step_dPrime = getDimensionOfEpoch(currStep, dPrime);
                        if (isBottomDimension(predecessor, dPrime)) {
                            if (step_dPrime != 0) {
                                setDimensionOfEpoch(predecessor, dPrime, step_dPrime - 1);
                            }
                        } else {
                            setDimensionOfEpoch(predecessor, dPrime, getDimensionOfEpoch(predecessor, dPrime) + step_dPrime);
                        }
                    }
                    assert(epoch == getSuccessorEpoch(predecessor, step));
                    gatheredPredecessorEpochs.insert(predecessor);
                    
                    do {
                        if (isBottomDimension(epoch, d)) {
                            uint64_t step_d = getDimensionOfEpoch(currStep, d);
                            if (step_d == 0) {
                                setDimensionOfEpoch(currStep, d, getDimensionOfEpoch(step, d));
                            } else {
                                setDimensionOfEpoch(currStep, d, step_d - 1);
                                d = 0;
                                break;
                            }
                        }
                        ++d;
                    } while(d < dimensionCount);
                }
            }
            
            bool EpochManager::isValidDimensionValue(uint64_t const& value) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                return ((value & dimensionBitMask) == value) && value != dimensionBitMask;
            }
            
            bool EpochManager::isZeroEpoch(Epoch const& epoch) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                return (epoch & relevantBitsMask) == 0;
            }
            
            bool EpochManager::isBottomEpoch(Epoch const& epoch) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                return (epoch & relevantBitsMask) == relevantBitsMask;
            }
            
            bool EpochManager::hasBottomDimension(Epoch const& epoch) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                uint64_t mask = dimensionBitMask;
                for (uint64_t d  = 0; d < dimensionCount; ++d) {
                    if ((epoch | mask) == epoch) {
                        return true;
                    }
                    mask = mask << bitsPerDimension;
                }
                return false;
            }

            void EpochManager::setBottomDimension(Epoch& epoch, uint64_t const& dimension) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                epoch |= (dimensionBitMask << (dimension * bitsPerDimension));
            }
            
            void EpochManager::setDimensionOfEpoch(Epoch& epoch, uint64_t const& dimension, uint64_t const& value) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                STORM_LOG_ASSERT(isValidDimensionValue(value), "The dimension value " << value << " is too high.");
                epoch &= ~(dimensionBitMask << (dimension * bitsPerDimension));
                epoch |= (value << (dimension * bitsPerDimension));
            }
            
            bool EpochManager::isBottomDimension(Epoch const& epoch, uint64_t const& dimension) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                return (epoch | (dimensionBitMask << (dimension * bitsPerDimension))) == epoch;
            }
            
            uint64_t EpochManager::getDimensionOfEpoch(Epoch const& epoch, uint64_t const& dimension) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                return (epoch >> (dimension * bitsPerDimension)) & dimensionBitMask;
            }
            
            std::string EpochManager::toString(Epoch const& epoch) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                std::string res = "<" + (isBottomDimension(epoch, 0) ? "_" : std::to_string(getDimensionOfEpoch(epoch, 0)));
                for (uint64_t d = 1; d < dimensionCount; ++d) {
                    res += ", ";
                    res += (isBottomDimension(epoch, d) ? "_" : std::to_string(getDimensionOfEpoch(epoch, d)));
                }
                res += ">";
                return res;
            }
            
            bool EpochManager::epochClassZigZagOrder(Epoch const& epoch1, Epoch const& epoch2) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                
                // Return true iff epoch 1 has to be computed before epoch 2
                
                // Check whether the number of bottom dimensions is not equal
                uint64_t e1Count = 0;
                uint64_t e2Count = 0;
                for (uint64_t dim = 0; dim < dimensionCount; ++dim) {
                    if (isBottomDimension(epoch1, dim)) {
                        ++e1Count;
                    }
                    if (isBottomDimension(epoch2, dim)) {
                        ++e2Count;
                    }
                }
                if (e1Count > e2Count) {
                    return true;
                } else if (e1Count < e2Count) {
                    return false;
                }
                
                // Check the epoch classes
                EpochClass e1Class = getEpochClass(epoch1);
                uint64_t e2Class = getEpochClass(epoch2);
                if (e1Class < e2Class) {
                    return true;
                } else if (e1Class > e2Class) {
                    return false;
                }
                assert(compareEpochClass(epoch1, epoch2));
                
                // check whether the sum of dimensions is the same
                uint64_t e1Sum = 0;
                uint64_t e2Sum = 0;
                for (uint64_t dim = 0; dim < dimensionCount; ++dim) {
                    if (!isBottomDimension(epoch1, dim)) {
                        assert(!isBottomDimension(epoch2, dim));
                        e1Sum += getDimensionOfEpoch(epoch1, dim);
                        e2Sum += getDimensionOfEpoch(epoch2, dim);
                    }
                }
                if (e1Sum < e2Sum) {
                    return true;
                } else if (e1Sum > e2Sum) {
                    return false;
                }
                
                // find the first dimension where the epochs do not match.
                // if the sum is even, we search from left to right, otherwise from right to left
                bool sumEven = (e1Sum % 2) == 0;
                if (sumEven) {
                    for (uint64_t dim = 0; dim < dimensionCount; ++dim) {
                        uint64_t e1Value = getDimensionOfEpoch(epoch1, dim);
                        uint64_t e2Value = getDimensionOfEpoch(epoch2, dim);
                        if (e1Value < e2Value) {
                            return true;
                        } else if (e1Value > e2Value) {
                            return false;
                        }
                    }
                } else {
                    uint64_t dim = dimensionCount;
                    while (dim > 0) {
                        --dim;
                        uint64_t e1Value = getDimensionOfEpoch(epoch1, dim);
                        uint64_t e2Value = getDimensionOfEpoch(epoch2, dim);
                        if (e1Value < e2Value) {
                            return true;
                        } else if (e1Value > e2Value) {
                            return false;
                        }
                    }
                }
                
                // reaching this point means that the epochs are equal
                assert(epoch1 == epoch2);
                return false;
            }
            
        }
    }
}