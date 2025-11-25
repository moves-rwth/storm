#pragma once

#include <random>

#include "storm/adapters/RationalNumberAdapter.h"

namespace storm {
namespace utility {

template<typename ValueType>
class RandomProbabilityGenerator {
   public:
    RandomProbabilityGenerator();
    RandomProbabilityGenerator(uint64_t seed);
    ValueType randomProbability();
    uint64_t randomSelect(uint64_t min, uint64_t max);
    ValueType randomExponential(ValueType rate);

   protected:
    std::mt19937 engine;
    std::uniform_int_distribution<uint64_t> uniformDistribution;
};

template<>
class RandomProbabilityGenerator<double> {
   public:
    RandomProbabilityGenerator();
    RandomProbabilityGenerator(uint64_t seed);
    double randomProbability();
    uint64_t randomSelect(uint64_t min, uint64_t max);
    double randomExponential(double rate);

   private:
    std::mt19937 engine;
    std::uniform_int_distribution<uint64_t> uniformDistribution;
    std::uniform_real_distribution<double> probabilityDistribution;
    std::exponential_distribution<double> exponentialDistribution;
};

template<>
class RandomProbabilityGenerator<storm::RationalNumber> {
   public:
    RandomProbabilityGenerator();
    RandomProbabilityGenerator(uint64_t seed);
    storm::RationalNumber randomProbability();
    uint64_t randomSelect(uint64_t min, uint64_t max);
    storm::RationalNumber randomExponential(storm::RationalNumber rate);

   private:
    std::mt19937 engine;
    std::uniform_int_distribution<uint64_t> uniformDistribution;
    std::uniform_int_distribution<uint64_t> probabilityDistribution;
};
}  // namespace utility
}  // namespace storm