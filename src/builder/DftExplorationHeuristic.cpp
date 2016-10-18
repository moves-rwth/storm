#include "src/builder/DftExplorationHeuristic.h"
#include "src/adapters/CarlAdapter.h"
#include "src/utility/macros.h"
#include "src/utility/constants.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/storage/dft/DFTState.h"

#include <limits>

namespace storm {
    namespace builder {

        template<typename ValueType>
        DFTExplorationHeuristic<ValueType>::DFTExplorationHeuristic() : skip(true), depth(std::numeric_limits<std::size_t>::max()), rate(storm::utility::zero<ValueType>()), exitRate(storm::utility::zero<ValueType>()) {
            // Intentionally left empty
        }

        template<typename ValueType>
        void DFTExplorationHeuristic<ValueType>::setHeuristicValues(size_t depth, ValueType rate, ValueType exitRate) {
            this->depth = std::min(this->depth, depth);
            this->rate = std::max(this->rate, rate);
            this->exitRate = std::max(this->exitRate, exitRate);
        }

        template<typename ValueType>
        bool DFTExplorationHeuristic<ValueType>::isSkip(double approximationThreshold, ApproximationHeuristic heuristic) const {
            if (!skip) {
                return false;
            }
            switch (heuristic) {
                case ApproximationHeuristic::NONE:
                    return false;
                case ApproximationHeuristic::DEPTH:
                    return depth > approximationThreshold;
                case ApproximationHeuristic::RATERATIO:
                    return getRateRatio() < approximationThreshold;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic not known.");
            }
        }

        template<typename ValueType>
        void DFTExplorationHeuristic<ValueType>::setNotSkip() {
            skip = false;
        }

        template<typename ValueType>
        size_t DFTExplorationHeuristic<ValueType>::getDepth() const {
            return depth;
        }

        template<typename ValueType>
        bool DFTExplorationHeuristic<ValueType>::compare(DFTExplorationHeuristic<ValueType> other, ApproximationHeuristic heuristic) {
            switch (heuristic) {
                case ApproximationHeuristic::NONE:
                    // Just use memory address for comparing
                    // TODO Matthias: better idea?
                    return this > &other;
                case ApproximationHeuristic::DEPTH:
                    return this->depth > other.depth;
                case ApproximationHeuristic::RATERATIO:
                    return this->getRateRatio() < other.getRateRatio();
                default:
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic not known.");
            }
        }

        template<>
        double DFTExplorationHeuristic<double>::getRateRatio() const {
            return rate/exitRate;
        }

        template<>
        double DFTExplorationHeuristic<storm::RationalFunction>::getRateRatio() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work.");
        }

        template class DFTExplorationHeuristic<double>;

#ifdef STORM_HAVE_CARL
        template class DFTExplorationHeuristic<storm::RationalFunction>;
#endif
    }
}
