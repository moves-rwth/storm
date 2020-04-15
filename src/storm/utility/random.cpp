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


    }
}
