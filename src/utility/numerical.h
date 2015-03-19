#include <vector>
#include <tuple>
#include <cmath>

#include "src/utility/macros.h"
#include "src/utility/constants.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace utility {
        namespace numerical {
            template<typename ValueType>
            std::tuple<uint_fast64_t, uint_fast64_t, ValueType, std::vector<ValueType>> getFoxGlynnCutoff(ValueType lambda, ValueType underflow, ValueType overflow, ValueType accuracy) {
                storm::utility::ConstantsComparator<ValueType> comparator;
                STORM_LOG_THROW(!comparator.isZero(lambda), storm::exceptions::InvalidArgumentException, "Error in Fox-Glynn algorithm: lambda must not be zero.");
                
                std::cout << "calling Fox-Glynn with " << lambda << ", " << overflow << ", " << underflow << ", " << accuracy << std::endl;
                
                // This code is a modified version of the one in PRISM. According to their implementation, for lambda
                // smaller than 400, we compute the result using the naive method.
                if (lambda < 400) {
                    ValueType eToPowerMinusLambda = std::exp(-lambda);
                    ValueType targetValue = (1 - accuracy) / eToPowerMinusLambda;
                    std::vector<ValueType> weights;
                    
                    ValueType exactlyKEvents = 1;
                    ValueType atMostKEvents = exactlyKEvents;
                    weights.push_back(exactlyKEvents * eToPowerMinusLambda);
                    
                    uint_fast64_t k = 1;
                    do {
                        exactlyKEvents *= lambda / k;
                        atMostKEvents += exactlyKEvents;
                        weights.push_back(exactlyKEvents * eToPowerMinusLambda);
                        ++k;
                    } while (atMostKEvents < targetValue);
                    
                    return std::make_tuple(0, k - 1, 1.0, weights);
                } else {
                    STORM_LOG_THROW(accuracy >= 1e-10, storm::exceptions::InvalidArgumentException, "Error in Fox-Glynn algorithm: the accuracy must not be below 1e-10.");
                    
                    // Factor from Fox&Glynn's paper. The paper does not explain where it comes from.
                    ValueType factor = 1e+10;
                    
                    // Now start the Finder algorithm to find the truncation points.
                    ValueType m = std::floor(lambda);
                    uint_fast64_t leftTruncationPoint = 0, rightTruncationPoint = 0;
                    {
                        // Factors used by the corollaries explained in Fox & Glynns paper.
                        // Square root of pi.
                        ValueType sqrtpi = 1.77245385090551602729;
                        
                        // Square root of 2.
                        ValueType sqrt2 = 1.41421356237309504880;
                        
                        // Set up a_\lambda, b_\lambda, and the square root of lambda.
                        ValueType aLambda = 0, bLambda = 0, sqrtLambda = 0;
                        if (m < 400) {
                            sqrtLambda = std::sqrt(400.0);
                            aLambda = (1.0 + 1.0 / 400.0) * std::exp(0.0625) * sqrt2;
                            bLambda = (1.0 + 1.0 / 400.0) * std::exp(0.125 / 400.0);
                        } else {
                            sqrtLambda = std::sqrt(lambda);
                            aLambda = (1.0 + 1.0 / lambda) * std::exp(0.0625) * sqrt2;
                            bLambda = (1.0 + 1.0 / lambda) * std::exp(0.125 / lambda);
                        }
                        
                        // Use Corollary 1 from the paper to find the right truncation point.
                        uint_fast64_t k = 4;
                        
                        ValueType dkl = 1.0 / (1 - std::exp(-(2.0 / 9.0) * (k * sqrt2 * sqrtLambda + 1.5)));
                        
                        // According to David Jansen the upper bound can be ignored to achieve more accurate results.
                        // Right hand side of the equation in Corollary 1.
                        while ((accuracy / 2.0) < (aLambda * dkl * std::exp(-(k*k / 2.0)) / (k * sqrt2 * sqrtpi))) {
                            ++k;
                            
                            // d(k,Lambda) from the paper.
                            dkl = 1.0 / (1 - std::exp(-(2.0 / 9.0)*(k * sqrt2 * sqrtLambda + 1.5)));
                        }
                        
                        std::cout << "k for upper: " << k << std::endl;
                        
                        // Left hand side of the equation in Corollary 1.
                        rightTruncationPoint = static_cast<uint_fast64_t>(std::ceil((m + k * sqrt2 * sqrtLambda + 1.5)));
                        
                        // Use Corollary 2 to find left truncation point.
                        k = 4;
                        
                        // Right hand side of the equation in Corollary 2.
                        while ((accuracy / 2.0) < ((bLambda * std::exp(-(k*k / 2.0))) / (k * sqrt2 * sqrtpi))) {
                            std::cout << "k=" << k << " produces: " << ((bLambda * std::exp(-(k*k / 2.0))) / (k * sqrt2 * sqrtpi)) << std::endl;
                            ++k;
                        }
                        std::cout << "k=" << k << " produces: " << ((bLambda * std::exp(-(k*k / 2.0))) / (k * sqrt2 * sqrtpi)) << std::endl;
                        std::cout << "k for lower: " << k << std::endl;
                        
                        // Left hand side of the equation in Corollary 2.
                        leftTruncationPoint = static_cast<uint_fast64_t>(std::max(std::trunc(m - k * sqrtLambda - 1.5), storm::utility::zero<ValueType>()));
                        
                        // Check for underflow.
                        STORM_LOG_THROW(std::trunc((m - k * sqrtLambda - 1.5)) >= 0, storm::exceptions::OutOfRangeException, "Error in Fox-Glynn algorithm: Underflow of left truncation point.");
                    }
                    
                    std::vector<ValueType> weights(rightTruncationPoint - leftTruncationPoint + 1);
                    weights[m - leftTruncationPoint] = overflow / (factor * (rightTruncationPoint - leftTruncationPoint));
                    for (uint_fast64_t j = m; j > leftTruncationPoint; --j) {
                        weights[j - 1 - leftTruncationPoint] = (j / lambda) * weights[j - leftTruncationPoint];
                    }
                    
                    for (uint_fast64_t j = m; j < rightTruncationPoint; ++j) {
                        weights[j + 1 - leftTruncationPoint] = (lambda / (j + 1)) * weights[j - leftTruncationPoint];
                    }
                    
                    
                    // Compute the total weight and start with smallest to avoid roundoff errors.
                    ValueType totalWeight = storm::utility::zero<ValueType>();
                    
                    uint_fast64_t s = leftTruncationPoint;
                    uint_fast64_t t = rightTruncationPoint;
                    while (s < t) {
                        if (weights[s - leftTruncationPoint] <= weights[t - leftTruncationPoint]) {
                            totalWeight += weights[s - leftTruncationPoint];
                            ++s;
                        } else {
                            totalWeight += weights[t - leftTruncationPoint];
                            --t;
                        }
                    }
                    
                    totalWeight += weights[s - leftTruncationPoint];
                    
                    return std::make_tuple(leftTruncationPoint, rightTruncationPoint, totalWeight, weights);
                }
            }
        }
    }
}