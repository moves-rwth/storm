#include "DftExplorationHeuristic.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/utility/macros.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace builder {

        template<typename ValueType>
        DFTExplorationHeuristic<ValueType>::DFTExplorationHeuristic(size_t id) : id(id), expand(true), lowerBound(storm::utility::zero<ValueType>()), upperBound(storm::utility::infinity<ValueType>()), depth(0), probability(storm::utility::one<ValueType>()) {
            // Intentionally left empty
        }

        template<typename ValueType>
        DFTExplorationHeuristic<ValueType>::DFTExplorationHeuristic(size_t id, DFTExplorationHeuristic const& predecessor, ValueType rate, ValueType exitRate) : id(id), expand(false), lowerBound(storm::utility::zero<ValueType>()), upperBound(storm::utility::zero<ValueType>()), depth(predecessor.depth + 1) {
            STORM_LOG_ASSERT(storm::utility::zero<ValueType>() < exitRate, "Exit rate is 0");
            probability = predecessor.probability * rate/exitRate;
        }

        template<typename ValueType>
        void DFTExplorationHeuristic<ValueType>::setBounds(ValueType lowerBound, ValueType upperBound) {
            this->lowerBound = lowerBound;
            this->upperBound = upperBound;
        }

        template<>
        bool DFTExplorationHeuristicProbability<double>::updateHeuristicValues(DFTExplorationHeuristic<double> const& predecessor, double rate, double exitRate) {
            STORM_LOG_ASSERT(exitRate > 0, "Exit rate is 0");
            probability += predecessor.getProbability() * rate/exitRate;
            return true;
        }

        template<>
        bool DFTExplorationHeuristicProbability<storm::RationalFunction>::updateHeuristicValues(DFTExplorationHeuristic<storm::RationalFunction> const& predecessor, storm::RationalFunction rate, storm::RationalFunction exitRate) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work for rational functions.");
            return false;
        }

        template<>
        double DFTExplorationHeuristicProbability<double>::getPriority() const {
            return probability;
        }

        template<>
        double DFTExplorationHeuristicProbability<storm::RationalFunction>::getPriority() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work for rational functions.");
        }

        template<>
        bool DFTExplorationHeuristicBoundDifference<double>::updateHeuristicValues(DFTExplorationHeuristic<double> const& predecessor, double rate, double exitRate) {
            STORM_LOG_ASSERT(exitRate > 0, "Exit rate is 0");
            probability += predecessor.getProbability() * rate/exitRate;
            return true;
        }

        template<>
        bool DFTExplorationHeuristicBoundDifference<storm::RationalFunction>::updateHeuristicValues(DFTExplorationHeuristic<storm::RationalFunction> const& predecessor, storm::RationalFunction rate, storm::RationalFunction exitRate) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work for rational functions.");
            return false;
        }

        template<typename ValueType>
        void DFTExplorationHeuristicBoundDifference<ValueType>::setBounds(ValueType lowerBound, ValueType upperBound) {
            this->lowerBound = lowerBound;
            this->upperBound = upperBound;
            difference = (storm::utility::one<ValueType>() / upperBound) - (storm::utility::one<ValueType>() / lowerBound);
        }

        template<>
        double DFTExplorationHeuristicBoundDifference<double>::getPriority() const {
            return probability * difference;
        }

        template<>
        double DFTExplorationHeuristicBoundDifference<storm::RationalFunction>::getPriority() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic bound difference does not work for rational functions.");
        }

        // Instantiate templates.
        template class DFTExplorationHeuristic<double>;
        template class DFTExplorationHeuristicNone<double>;
        template class DFTExplorationHeuristicDepth<double>;
        template class DFTExplorationHeuristicProbability<double>;
        template class DFTExplorationHeuristicBoundDifference<double>;

#ifdef STORM_HAVE_CARL
        template class DFTExplorationHeuristic<storm::RationalFunction>;
        template class DFTExplorationHeuristicNone<storm::RationalFunction>;
        template class DFTExplorationHeuristicDepth<storm::RationalFunction>;
        template class DFTExplorationHeuristicProbability<storm::RationalFunction>;
        template class DFTExplorationHeuristicBoundDifference<storm::RationalFunction>;
#endif
    }
}
