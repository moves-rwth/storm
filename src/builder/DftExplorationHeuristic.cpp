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
        bool DFTExplorationHeuristic<ValueType>::isSkip() const {
            return skip;
        }

        template<typename ValueType>
        size_t DFTExplorationHeuristic<ValueType>::getDepth() const {
            return depth;
        }

        template<typename ValueType>
        void DFTExplorationHeuristic<ValueType>::setHeuristicValues(size_t depth, ValueType rate, ValueType exitRate) {
            std::cout << "Set priority: " << depth << ", old: " << this->depth << std::endl;
            this->depth = depth;
            // TODO Matthias: update rates and exitRates as well
            this->rate = rate;
            this->exitRate = exitRate;
        }

        template<typename ValueType>
        double DFTExplorationHeuristic<ValueType>::getPriority() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Approximation works only for double.");
        }

        template<>
        double DFTExplorationHeuristic<double>::getPriority() const {
            // TODO Matthias: change according to heuristic
            if (!skip) {
                // TODO Matthias: change to non-magic number
                return 0;
            }
            //return rate/exitRate;
            return depth;
        }

        template<>
        double DFTExplorationHeuristic<storm::RationalFunction>::getPriority() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Approximation works only for double.");
            /*std::cout << (rate / exitRate) << " < " << threshold << ": " << (number < threshold) << std::endl;
             std::map<storm::Variable, storm::RationalNumber> mapping;
             storm::RationalFunction eval(number.evaluate(mapping));
             std::cout << "Evaluated: " << eval << std::endl;
             return eval < threshold;*/
        }

        template<typename ValueType>
        bool compareDepth(std::shared_ptr<storm::storage::DFTState<ValueType>> stateA, std::shared_ptr<storm::storage::DFTState<ValueType>> stateB) {
            return stateA->getPriority() > stateB->getPriority();
        }

        template class DFTExplorationHeuristic<double>;
        template bool compareDepth(std::shared_ptr<storm::storage::DFTState<double>>, std::shared_ptr<storm::storage::DFTState<double>>);

#ifdef STORM_HAVE_CARL
        template class DFTExplorationHeuristic<storm::RationalFunction>;
        template bool compareDepth(std::shared_ptr<storm::storage::DFTState<storm::RationalFunction>>, std::shared_ptr<storm::storage::DFTState<storm::RationalFunction>>);
#endif
    }
}
