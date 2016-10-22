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

        template<>
        bool DFTExplorationHeuristicRateRatio<double>::updateHeuristicValues(size_t depth, double rate, double exitRate) {
            bool update = false;
            if (this->rate < rate) {
                this->rate = rate;
                update = true;
            }
            if (this->exitRate < exitRate) {
                this->exitRate = exitRate;
                update = true;
            }
            return update;
        }

        template<>
        bool DFTExplorationHeuristicRateRatio<storm::RationalFunction>::updateHeuristicValues(size_t depth, storm::RationalFunction rate, storm::RationalFunction exitRate) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work for rational functions.");
            return false;
        }

        template<>
        double DFTExplorationHeuristicRateRatio<double>::getPriority() const {
            return rate/exitRate;
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
