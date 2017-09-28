#pragma once

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/PrecisionExceededException.h"

namespace storm {
    namespace utility{
        namespace kwek_mehlhorn {
            
            template<typename IntegerType>
            std::pair<IntegerType, IntegerType> findRational(IntegerType const& alpha, IntegerType const& beta, IntegerType const& gamma, IntegerType const& delta) {
                IntegerType alphaDivBetaFloor = alpha / beta;
                IntegerType gammaDivDeltaFloor = gamma / delta;
                IntegerType alphaModBeta = storm::utility::mod(alpha, beta);
                
                if (alphaDivBetaFloor == gammaDivDeltaFloor && !storm::utility::isZero(alphaModBeta)) {
                    std::pair<IntegerType, IntegerType> subresult = findRational(delta, storm::utility::mod(gamma, delta), beta, alphaModBeta);
                    auto result = std::make_pair(alphaDivBetaFloor * subresult.first + subresult.second, subresult.first);
                    
                    return result;
                } else {
                    auto result = std::make_pair(storm::utility::isZero(alphaModBeta) ? alphaDivBetaFloor : alphaDivBetaFloor + storm::utility::one<IntegerType>(), storm::utility::one<IntegerType>());
                    return result;
                }
            }
            
            template<typename RationalType, typename ImpreciseType>
            std::pair<typename NumberTraits<RationalType>::IntegerType, typename NumberTraits<RationalType>::IntegerType> truncateToRational(ImpreciseType const& value, uint64_t precision) {
                typedef typename NumberTraits<RationalType>::IntegerType IntegerType;

                IntegerType powerOfTen = storm::utility::pow(storm::utility::convertNumber<IntegerType>(10ull), precision);
                IntegerType truncated = storm::utility::trunc<RationalType>(value * powerOfTen);
                return std::make_pair(truncated, powerOfTen);
            }
            
            template<typename RationalType>
            std::pair<typename NumberTraits<RationalType>::IntegerType, typename NumberTraits<RationalType>::IntegerType> truncateToRational(double const& value, uint64_t precision) {
                STORM_LOG_THROW(precision < 17, storm::exceptions::PrecisionExceededException, "Exceeded precision of double, consider switching to rational numbers.");
                
                double powerOfTen = std::pow(10, precision);
                double truncated = storm::utility::trunc<double>(value * powerOfTen);
                return std::make_pair(truncated, powerOfTen);
            }
            
            template<typename RationalType, typename ImpreciseType>
            RationalType findRational(uint64_t precision, ImpreciseType const& value) {
                typedef typename NumberTraits<RationalType>::IntegerType IntegerType;
                
                std::pair<IntegerType, IntegerType> truncatedFraction = truncateToRational<RationalType>(value, precision);
                std::pair<IntegerType, IntegerType> result = findRational<IntegerType>(truncatedFraction.first, truncatedFraction.second, truncatedFraction.first + storm::utility::one<IntegerType>(), truncatedFraction.second);
                
                // Convert one of the arguments to a rational type to not get integer division.
                return storm::utility::convertNumber<RationalType>(result.first) / result.second;
            }
            
            template<typename RationalType, typename ImpreciseType>
            void sharpen(uint64_t precision, std::vector<ImpreciseType> const& input, std::vector<RationalType>& output) {
                for (uint64_t index = 0; index < input.size(); ++index) {
                    ImpreciseType integer = storm::utility::floor(input[index]);
                    ImpreciseType fraction = input[index] - integer;
                    auto rational = findRational<RationalType>(precision, fraction);
                    output[index] = storm::utility::convertNumber<RationalType>(integer) + rational;
                    STORM_LOG_ASSERT(storm::utility::isZero(fraction) || !storm::utility::isZero(rational), "Found zero rational for non-zero fraction " << fraction << ".");
                    STORM_LOG_ASSERT(rational >= storm::utility::zero<RationalType>(), "Expected non-negative rational.");
                    if (std::is_same<RationalType, ImpreciseType>::value) {
                        STORM_LOG_ASSERT(output[index] >= input[index], "Sharpen decreased value from " << input[index] << " to " << output[index] << ".");
                    }
                }
            }
            
        }
    }
}
