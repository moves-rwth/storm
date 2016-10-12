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
                    // TODO Matthias: implement
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Heuristic rate ration does not work.");
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
        void DFTExplorationHeuristic<ValueType>::setHeuristicValues(size_t depth, ValueType rate, ValueType exitRate) {
            this->depth = depth;
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
