#include <vector>
#include <tuple>
#include <cmath>

#include "src/utility/macros.h"
#include "src/utility/ConstantsComparator.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace utility {
        namespace numerical {
            template<typename ValueType>
            std::tuple<uint_fast64_t, uint_fast64_t, ValueType, std::vector<ValueType>> getFoxGlynnCutoff(ValueType lambda, ) {
                storm::utility::ConstantsComparator<ValueType> comparator;
                
                ValueType underflow, overflow, accuracy = 0;
                
                STORM_LOG_THROW(!comparator.isZero(lambda), storm::exceptions::InvalidArgumentException, "Error in Fox-Glynn algorithm: lambda must not be zero.");
                
                // This code is more or less taken from PRISM. According to their implementation, for lambda smaller than
                // 400, we compute the result using the naive method.
                if (lambda < 400) {
                    ValueType eToPowerMinusLambda = std::exp(-lambda);
                    ValueType targetValue = (1 - (this->accuracy / 2.0)) / eToPowerMinusLambda;
                    std::vector<ValueType> weights;
                    
                    ValueType exactlyKEvents = 1;
                    ValueType atMostKEvents = exactlyKEvents;
                    weights.push_back(exactlyKEvents * eToPowerMinusLambda);
                    
                    uint_fast64_t currentK = 1;
                    do {
                        exactlyKEvents *= lambda / k;
                        atMostKEvents += exactlyKEvents;
                        weights.push_back(exactlyKEvents * eToPowerMinusLambda);
                        ++k;
                    } while (atMostKEvents < targetValue);
                    
                    return std::make_tuple(0, k - 1, 1.0, weights);
                } else {
                    STORM_LOG_THROW(accuracy >= 1e-10, storm::exceptions::InvalidArgumentException, "Error in Fox-Glynn algorithm: the accuracy must not be below 1e-10.");
                    
                    ValueType factor = 1e+10;
                    ValueType m = std::floor(lambda);
                    
                    // Now start the Finder algorithm to find the truncation points.
                    {
                        // Factors used by the corollaries explained in Fox & Glynns paper.
                        // Square root of pi.
                        Type sqrtpi = 1.77245385090551602729;
                        
                        // Square root of 2.
                        Type sqrt2 = 1.41421356237309504880;
                        
                        // Square root of lambda.
                        Type sqrtLambda = std::sqrt(lambda);
                        
                        Type a_Lambda = (1.0 + 1.0 / lambda) * exp(0.0625) * sqrt2; // a_\lambda.
                        Type b_Lambda = (1.0 + 1.0 / lambda) * exp(0.125 / lambda); // b_\lambda.
                    }
                }
                
                    // Use Fox Glynn algorithm for lambda>400.
                    if (accuracy < 1e-10) {
                        LOG4CPLUS_ERROR(logger, "Given Value accuracy must at least be 1e-10.");
                        throw storm::exceptions::InvalidArgumentException("Error while computing FoxGlynn values. accuracy < 1e-10.");
                    }
                    // Factor from Fox&Glynns paper. The paper does not explain where it comes from.
                    Type factor = 1e+10;
                    
                    // Run the FINDER algorithm.
                    Type m = floor(lambda);
                    {
                        // Factores used by the corollaries explained in Fox&Glynns paper.
                        Type sqrtpi = 1.77245385090551602729; // Squareroot of PI.
                        Type sqrt2 = 1.41421356237309504880; // Squareroot of 2.
                        Type sqrtLambda = sqrt(lambda); // Sqareroot of Lambda.
                        Type a_Lambda = (1.0 + 1.0 / lambda) * exp(0.0625) * sqrt2; // a_\lambda.
                        Type b_Lambda = (1.0 + 1.0 / lambda) * exp(0.125 / lambda); // b_\lambda.
                        
                        // Use Corollary 1 from the paper to find the right truncation point.
                        Type k = 4;
                        res = a_Lambda*dkl*exp(-k*k / 2.0) / (k*sqrt2*sqrtpi);
                        /* Normally: Iterate between the bounds stated above to find the right truncationpoint.
                         * This is a modification to the Fox-Glynn paper. The search for the right truncationpoint is only
                         * terminated by the error condition and not by the upper bound. Thus the calculation can be more accurate.
                         */
                        while (res > accuracy / 2.0)
                        {
                            k++;
                            Type dkl = 1.0 / (1 - exp(-(2.0 / 9.0)*(k*sqrt2*sqrtLambda + 1.5))); // d(k,Lambda) from the paper.
                            Type res = a_Lambda*dkl*exp(-k*k / 2.0) / (k*sqrt2*sqrtpi); // Right hand side of the equation in Corollary 1.
                            
                        }
                        
                        // Left hand side of the equation in Corollary 1.
                        this->rightTrunk = (int)ceil(m + k*sqrt2*sqrtLambda + 1.5);
                        
                        // Use Corollary 2 to find left truncation point.
                        Type res;
                        k = 4;
                        
                        do
                        {
                            res = b_Lambda*exp(-k*-k / 2.0) / (k*sqrt2*sqrtpi); // Right hand side of the equation in Corollary 2
                            k++;
                        } while (res > accuracy / 2.0);
                        
                        this->leftTrunk = (int)(m - k*sqrtLambda - 1.5); // Left hand side of the equation in Corollary 2.
                        
                        // Check for underflow.
                        if ((int)(m - k*sqrtLambda - 1.5) < 0.0) {
                            LOG4CPLUS_ERROR(logger, "Underflow while computing left truncation point.");
                            throw storm::exceptions::OutOfRangeException("Error while computing FoxGlynn values. Underflow of left Truncation point.");
                        }
                        
                    }
                    //End of FINDER algorithm.
                    
                    // Use WEIGHTER algorithm to determine weights.
                    // Down from m
                    for (Type j = m; j > this->leftTrunk; j--)
                        this->weights.at(j - 1 - this->leftTrunk) = (j / lambda)*this->weights.at(j - this->leftTrunk);
                    // Up from m
                    for (Type j = m; j < this->rightTrunk; j++)
                        this->weights.at(j + 1 - this->leftTrunk) = (lambda / (j + 1))*this->weights.at(j - this->leftTrunk);
                    
                    // Compute total weight.
                    // Add up weights from smallest to largest to avoid roundoff errors.
                    this->totalWeight = 0.0;
                    Type s = this->leftTrunk;
                    Type t = this->rightTrunk;
                    while (s < t)
                    {
                        if (this->weights.at(s - this->leftTrunk) <= this->weights.at(t - this->leftTrunk))
                        {
                            this->totalWeight += this->weights.at(s - this->leftTrunk);
                            s++;
                        }
                        else
                        {
                            this->totalWeight += this->weights.at(t - this->leftTrunk);
                            t--;
                        }
                    }
                    
                    this->totalWeight += this->weights.at(s - this->leftTrunk);
                    
                    LOG4CPLUS_INFO(logger, "Left truncationpoint: " << this->leftTrunk << ".");
                    LOG4CPLUS_INFO(logger, "Right truncationpoint: " << this->rightTrunk << ".");
                    LOG4CPLUS_INFO(logger, "Total Weight:" << this->totalWeight << ".");
                    LOG4CPLUS_INFO(logger, "10. Weight: " << this->weights.at(9) << ".");
                }
                
            }
        }
    }
}