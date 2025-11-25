#include "storm/utility/random.h"

#include <limits>

#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace utility {

RandomProbabilityGenerator<double>::RandomProbabilityGenerator() : probabilityDistribution(0.0, 1.0) {
    std::random_device rd;
    engine = std::mt19937(rd());
}

RandomProbabilityGenerator<double>::RandomProbabilityGenerator(uint64_t seed) : engine(seed), probabilityDistribution(0.0, 1.0) {}

double RandomProbabilityGenerator<double>::randomProbability() {
    return probabilityDistribution(engine);
}

uint64_t RandomProbabilityGenerator<double>::randomSelect(uint64_t min, uint64_t max) {
    decltype(uniformDistribution.param()) newInterval(min, max);
    uniformDistribution.param(newInterval);
    return uniformDistribution(engine);
    // return std::uniform_int_distribution<uint64_t>(min, max)(engine);
}

double RandomProbabilityGenerator<double>::randomExponential(double rate) {
    decltype(exponentialDistribution.param()) newRate(rate);
    exponentialDistribution.param(newRate);
    return exponentialDistribution(engine);
}

RandomProbabilityGenerator<storm::RationalNumber>::RandomProbabilityGenerator() : probabilityDistribution(0, std::numeric_limits<uint64_t>::max()) {
    std::random_device rd;
    engine = std::mt19937(rd());
}

RandomProbabilityGenerator<storm::RationalNumber>::RandomProbabilityGenerator(uint64_t seed)
    : engine(seed), probabilityDistribution(0, std::numeric_limits<uint64_t>::max()) {}

storm::RationalNumber RandomProbabilityGenerator<storm::RationalNumber>::randomProbability() {
    return carl::rationalize<storm::RationalNumber>(probabilityDistribution(engine)) /
           carl::rationalize<storm::RationalNumber>(std::numeric_limits<uint64_t>::max());
}

uint64_t RandomProbabilityGenerator<storm::RationalNumber>::randomSelect(uint64_t min, uint64_t max) {
    decltype(uniformDistribution.param()) newInterval(min, max);
    uniformDistribution.param(newInterval);
    return uniformDistribution(engine);
    // return std::uniform_int_distribution<uint64_t>(min, max)(engine);
}

storm::RationalNumber RandomProbabilityGenerator<storm::RationalNumber>::randomExponential(storm::RationalNumber rate) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Exponential distribution is not supported for rational numbers");
}

}  // namespace utility
}  // namespace storm
