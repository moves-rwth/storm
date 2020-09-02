#include "storm/utility/random.h"

namespace storm {
    namespace utility {
        RandomProbabilityGenerator<double>::RandomProbabilityGenerator()
        : distribution(0.0, 1.0)
        {
            std::random_device rd;
            engine = std::mt19937(rd());
        }

        RandomProbabilityGenerator<double>::RandomProbabilityGenerator(uint64_t seed)
        : distribution(0.0, 1.0), engine(seed)
        {

        }

        double RandomProbabilityGenerator<double>::random() {
            return distribution(engine);
        }

        uint64_t RandomProbabilityGenerator<double>::random_uint(uint64_t min, uint64_t max) {
            return std::uniform_int_distribution<uint64_t>(min, max)(engine);
        }


    }
}
