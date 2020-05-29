#include "storm-dft/utility/MTTFHelper.h"

namespace {

/**
 * Fills Vector buffer with elements which are
 * stepsize apart starting with the given start
 *
 * \note
 * Helperfunction for MTTFHelper
 */
void linspace(std::vector<double> &buffer, double start,
              double const stepsize) {
    for (size_t i{0}; i < buffer.size(); ++i) {
        buffer[i] = i * stepsize + start;
    }
}

}  // namespace

namespace storm {
namespace dft {
namespace utility {

double MTTFHelper(std::shared_ptr<storm::storage::DFT<double>> const dft,
                  double const stepsize) {
    constexpr size_t chunksize{101};
    storm::modelchecker::DFTModularizer checker{dft};

    std::vector<double> timepoints{};
    timepoints.resize(chunksize);
    linspace(timepoints, 0.0, stepsize);
    std::vector<double> probabilities{
        checker.getProbabilitiesAtTimepoints(timepoints)};

    double delta{1.0};
    double rval{0.0};

    double y1{0.0}, y2{0.0}, y3{1.0 - probabilities[0]};
    size_t i{1};
    while (std::abs(delta) > 1e-7) {
        if (i + 1 >= probabilities.size()) {
            double const start{timepoints.back()};
            linspace(timepoints, start, stepsize);
            probabilities = checker.getProbabilitiesAtTimepoints(timepoints);
            i = 1;
        }

        y1 = y3;
        y2 = 1.0 - probabilities[i];
        y3 = 1.0 - probabilities[i + 1];
        i += 2;

        delta = y1 + 4.0 * y2 + y3;
        delta /= 6.0;
        rval += delta;
    }

    return 2 * stepsize * rval;
}

}  // namespace utility
}  // namespace dft
}  // namespace storm
