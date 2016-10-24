#include "src/builder/DftExplorationHeuristic.h"
#include "src/adapters/CarlAdapter.h"
#include "src/utility/macros.h"
#include "src/utility/constants.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace builder {

        template<typename ValueType>
        DFTExplorationHeuristic<ValueType>::DFTExplorationHeuristic(size_t id, size_t depth, ValueType rate, ValueType exitRate) : id(id), expand(false), depth(depth) {
            STORM_LOG_ASSERT(storm::utility::zero<ValueType>() < exitRate, "Exit rate is 0");
            rateRatio = rate/exitRate;
        }

        template<>
        bool DFTExplorationHeuristicRateRatio<double>::updateHeuristicValues(size_t depth, double rate, double exitRate) {
            STORM_LOG_ASSERT(exitRate > 0, "Exit rate is 0");
            rateRatio += rate/exitRate;
            return true;
        }

        template<>
        bool DFTExplorationHeuristicRateRatio<storm::RationalFunction>::updateHeuristicValues(size_t depth, storm::RationalFunction rate, storm::RationalFunction exitRate) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work for rational functions.");
            return false;
        }

        template<>
        double DFTExplorationHeuristicRateRatio<double>::getPriority() const {
            return rateRatio;
        }

        template<>
        double DFTExplorationHeuristicRateRatio<storm::RationalFunction>::getPriority() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work for rational functions.");
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
