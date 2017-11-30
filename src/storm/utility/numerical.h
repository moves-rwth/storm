#include <vector>
#include <tuple>
#include <cmath>

#include <boost/math/constants/constants.hpp>

#include "storm/utility/macros.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/OutOfRangeException.h"
#include "storm/exceptions/PrecisionExceededException.h"

namespace storm {
    namespace utility {
        namespace numerical {
            template<typename ValueType>
            std::tuple<uint_fast64_t, uint_fast64_t, ValueType, std::vector<ValueType>> getFoxGlynnCutoff(ValueType lambda, ValueType overflow, ValueType accuracy) {
                STORM_LOG_THROW(lambda != storm::utility::zero<ValueType>(), storm::exceptions::InvalidArgumentException, "Error in Fox-Glynn algorithm: lambda must not be zero.");
                
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
                        
                        // Left hand side of the equation in Corollary 1.
                        rightTruncationPoint = static_cast<uint_fast64_t>(std::ceil((m + k * sqrt2 * sqrtLambda + 1.5)));
                        
                        // Use Corollary 2 to find left truncation point.
                        k = 4;
                        
                        // Right hand side of the equation in Corollary 2.
                        while ((accuracy / 2.0) < ((bLambda * std::exp(-(k*k / 2.0))) / (k * sqrt2 * sqrtpi))) {
                            ++k;
                        }
                        
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

            template<typename ValueType>
            std::tuple<uint64_t, uint64_t, ValueType> foxGlynnFinder(ValueType lambda, ValueType underflow, ValueType overflow, ValueType accuracy) {
                uint64_t m = static_cast<uint64_t>(lambda);
                int64_t L = 0;
                uint64_t R = 0;
                uint64_t k = 4;
                
                ValueType pi = boost::math::constants::pi<ValueType>();

                if (m < 25) {
                    STORM_LOG_THROW(std::exp(-static_cast<double>(m)) >= underflow, storm::exceptions::PrecisionExceededException, "Underflow in Fox-Glynn.");
                    L = 0;
                } else {
                    ValueType rhsb = (accuracy / 2) * std::sqrt(2 * pi) / ((1 + 1/lambda) * std::exp(1 / (8 * lambda)));
                    k = 4;
                    
                    do {
                        L = m - std::ceil(k * std::sqrt(lambda) + 0.5);
                        if (L <= 0) {
                            break;
                        }
                        if (std::exp(-(k*k) / 2) / k <= rhsb) {
                            break;
                        }
                        ++k;
                    } while (true);
                }
                
                uint64_t mmax;
                uint64_t maxr;
                ValueType r2l;
                ValueType rhsa;

                if (m < 400) {
                    mmax = 400;
                    r2l = std::sqrt(2 * 400);
                    rhsa = (accuracy / 2) * std::sqrt(2 * pi) / ((1 + 1.0/400) * std::sqrt(2.0) * std::exp(1.0 / 16));
                    maxr = 400 + static_cast<uint64_t>(std::ceil((400 + 1) / 2.0));
                } else {
                    mmax = m;
                    r2l = std::sqrt(2 * lambda);
                    rhsa = (accuracy / 2) * std::sqrt(2 * pi) / ((1 + 1.0/lambda) * std::sqrt(2.0) * std::exp(1.0 / 16));
                    maxr = m + static_cast<uint64_t>(std::ceil((lambda + 1) / 2.0));
                }
                
                k = 4;
                do {
                    R = mmax + static_cast<uint64_t>(std::ceil(k * r2l + 0.5));
                    STORM_LOG_THROW(R <= maxr, storm::exceptions::PrecisionExceededException, "Fox-Glynn: cannot bound right tail.");
                    ValueType dkl = storm::utility::one<ValueType>();
                    if (dkl * std::exp(-(k * k / 2.0)) / k <= rhsa) {
                        break;
                    }
                    ++k;
                } while (true);
                
                ValueType wm = std::pow<ValueType>(10, -10) * overflow * (R - L + 1);

                if (m >= 25) {
                    ValueType lcm = -1 - (1.0/12*25) - std::log(2 * pi) - 0.5 * std::log(static_cast<ValueType>(m));
                    uint64_t i = m - L;
                    ValueType llb;
                    if (i <= static_cast<uint64_t>(L)) {
                        llb = -static_cast<ValueType>(i)*(i + 1)*(((2 * i + 1) / (6 * lambda)) + 0.5) * (1 / lambda) + lcm;
                    } else {
                        llb = std::max(i * std::log(1 - (i / (m + 1.0))), -lambda);
                    }
                    STORM_LOG_THROW(llb >= std::log(underflow) - std::log(wm), storm::exceptions::PrecisionExceededException, "Fox-Glynn: Underflow at lower bound.");
                    if (m >= 400) {
                        llb = lcm - std::pow(R - m + 1, 2) / (2 * lambda);
                        STORM_LOG_THROW(llb >= std::log(underflow) - std::log(wm), storm::exceptions::PrecisionExceededException, "Fox-Glynn: Underflow at upper bound.");
                    }
                }
                
                return std::make_tuple(static_cast<uint64_t>(L), R, wm);
            }
            
            template<typename ValueType>
            std::tuple<uint64_t, uint64_t, ValueType, std::vector<ValueType>> foxGlynnWeighter(ValueType lambda, ValueType accuracy) {
                
                STORM_LOG_THROW(lambda > storm::utility::zero<ValueType>(), storm::exceptions::InvalidArgumentException, "Fox-Glynn: Lambda must be positive.");
                
                ValueType underflow = std::numeric_limits<ValueType>::min();
                ValueType overflow = std::numeric_limits<ValueType>::max();

                // Initialize.
                uint64_t m = static_cast<uint64_t>(lambda);
                std::tuple<uint64_t, uint64_t, ValueType> finderResult = foxGlynnFinder(lambda, underflow, overflow, accuracy);
                uint64_t L = std::get<0>(finderResult);
                uint64_t R = std::get<1>(finderResult);
                ValueType wm = std::get<2>(finderResult);
                std::vector<ValueType> w(R - L + 1);
                w.back() = wm;

                // Down.
                for (uint64_t j = m; j > L; --j) {
                    w[j - L - 1] = (j / lambda) * w[j - L];
                }

                // Up.
                if (lambda < 400) {
                    // Special.
                    STORM_LOG_THROW(R <= 600, storm::exceptions::PrecisionExceededException, "Potential underflow in Fox-Glynn.");
                    for (uint64_t j = m; j < R; ++j) {
                        auto q = lambda / (j + 1);
                        if (w[j - L] > underflow / q) {
                            w[j - L + 1] = q * w[j - L];
                        } else {
                            R = j;
                            break;
                        }
                    }
                } else {
                    for (uint64_t j = m; j < R; ++j) {
                        w[j - L + 1] = (lambda / (j + 1)) * w[j - L];
                    }
                }

                // Compute W.
                ValueType W = storm::utility::zero<ValueType>();
                uint64_t s = L;
                uint64_t t = R;
                
                while (s < t) {
                    if (w[s - L] <= w[t - L]) {
                        W += w[s - L];
                        ++s;
                    } else {
                        W += w[t - L];
                        --t;
                    }
                }
                W += w[s - L];
                
                // Return.
                return std::make_tuple(L, R, W, w);
            }
            
            template<typename ValueType>
            std::tuple<uint_fast64_t, uint_fast64_t, ValueType, std::vector<ValueType>> foxGlynn(ValueType lambda, ValueType accuracy) {
                return foxGlynnWeighter(lambda, accuracy);
            }
                
        }
    }
}
