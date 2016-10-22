#include "src/builder/DftExplorationHeuristic.h"
#include "src/adapters/CarlAdapter.h"
#include "src/utility/macros.h"
#include "src/utility/constants.h"
#include "src/exceptions/NotImplementedException.h"

#include <limits>

namespace storm {
    namespace builder {

        template<typename ValueType>
        DFTExplorationHeuristic<ValueType>::DFTExplorationHeuristic(size_t id) : id(id), expand(false), depth(std::numeric_limits<std::size_t>::max()), rate(storm::utility::zero<ValueType>()), exitRate(storm::utility::zero<ValueType>()) {
            // Intentionally left empty
        }

        template<typename ValueType>
        void DFTExplorationHeuristic<ValueType>::updateHeuristicValues(size_t depth, ValueType rate, ValueType exitRate) {
            this->depth = std::min(this->depth, depth);
            this->rate = std::max(this->rate, rate);
            this->exitRate = std::max(this->exitRate, exitRate);
        }

        template<typename ValueType>
        DFTExplorationHeuristicNone<ValueType>::DFTExplorationHeuristicNone(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
            // Intentionally left empty
        }

        template<typename ValueType>
        bool DFTExplorationHeuristicNone<ValueType>::isSkip(double approximationThreshold) const {
            return false;
        }

        template<typename ValueType>
        bool DFTExplorationHeuristicNone<ValueType>::operator<(DFTExplorationHeuristicNone<ValueType> const& other) const {
            // Just use memory address for comparing
            // TODO Matthias: better idea?
            return this > &other;
        }

        template<typename ValueType>
        DFTExplorationHeuristicDepth<ValueType>::DFTExplorationHeuristicDepth(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
            // Intentionally left empty
        }

        template<typename ValueType>
        bool DFTExplorationHeuristicDepth<ValueType>::isSkip(double approximationThreshold) const {
            return !this->expand && this->depth > approximationThreshold;
        }

        template<typename ValueType>
        bool DFTExplorationHeuristicDepth<ValueType>::operator<(DFTExplorationHeuristicDepth<ValueType> const& other) const {
            return this->depth > other.depth;
        }

        template<typename ValueType>
        DFTExplorationHeuristicRateRatio<ValueType>::DFTExplorationHeuristicRateRatio(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
            // Intentionally left empty
        }

        template<typename ValueType>
        bool DFTExplorationHeuristicRateRatio<ValueType>::isSkip(double approximationThreshold) const {
            return !this->expand && this->getRateRatio() < approximationThreshold;
        }

        template<typename ValueType>
        bool DFTExplorationHeuristicRateRatio<ValueType>::operator<(DFTExplorationHeuristicRateRatio<ValueType> const& other) const {
            return this->getRateRatio() < other.getRateRatio();
        }


        template<>
        double DFTExplorationHeuristicRateRatio<double>::getRateRatio() const {
            return rate/exitRate;
        }

        template<>
        double DFTExplorationHeuristicRateRatio<storm::RationalFunction>::getRateRatio() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work.");
        }


        // Instantiate templates.
        template class DFTExplorationHeuristic<double>;
        template class DFTExplorationHeuristicNone<double>;
        template class DFTExplorationHeuristicDepth<double>;
        template class DFTExplorationHeuristicRateRatio<double>;

#ifdef STORM_HAVE_CARL
        template class DFTExplorationHeuristic<storm::RationalFunction>;
        template class DFTExplorationHeuristicNone<storm::RationalFunction>;
        template class DFTExplorationHeuristicDepth<storm::RationalFunction>;
        template class DFTExplorationHeuristicRateRatio<storm::RationalFunction>;
#endif
    }
}
