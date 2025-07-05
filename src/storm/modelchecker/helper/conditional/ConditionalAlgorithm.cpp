#include "ConditionalAlgorithm.h"

namespace storm {
std::ostream& operator<<(std::ostream& stream, ConditionalAlgorithm const& algorithm) {
    switch (algorithm) {
        case ConditionalAlgorithm::Default:
            return stream << "default";
        case ConditionalAlgorithm::Restart:
            return stream << "restart";
        case ConditionalAlgorithm::Bisection:
            return stream << "bisection";
        case ConditionalAlgorithm::BisectionAdvanced:
            return stream << "bisection-advanced";
        case ConditionalAlgorithm::PolicyIteration:
            return stream << "pi";
    }
    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unknown conditional algorithm");
    return stream;
}

ConditionalAlgorithm conditionalAlgorithmFromString(std::string const& algorithm) {
    if (algorithm == "default") {
        return ConditionalAlgorithm::Default;
    } else if (algorithm == "restart") {
        return ConditionalAlgorithm::Restart;
    } else if (algorithm == "bisection") {
        return ConditionalAlgorithm::Bisection;
    } else if (algorithm == "bisection-advanced") {
        return ConditionalAlgorithm::BisectionAdvanced;
    } else if (algorithm == "pi") {
        return ConditionalAlgorithm::PolicyIteration;
    }
    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unknown conditional algorithm: " << algorithm);
}

}  // namespace storm