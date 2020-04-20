#include <random>

namespace storm {
    namespace utility {
        template<typename ValueType>
        class RandomProbabilityGenerator {
        public:
            RandomProbabilityGenerator();
            RandomProbabilityGenerator(uint64_t seed);
            ValueType random() const;

        };

        template<>
        class RandomProbabilityGenerator<double> {
        public:
            RandomProbabilityGenerator();
            RandomProbabilityGenerator(uint64_t seed);
            double random();
        private:
            std::uniform_real_distribution<double> distribution;
            std::mt19937 engine;

        };



    }
}