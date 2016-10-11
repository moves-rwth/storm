#ifndef STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_
#define STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_

#include <memory>
#include <algorithm>

namespace storm {

    // Forward declaration
    namespace storage {
        template<typename ValueType>
        class DFTState;
    }

    namespace builder {

        /*!
         * Enum representing the heuristic used for deciding which states to expand.
         */
        enum class ApproximationHeuristic { NONE, DEPTH, RATERATIO };

        template<typename ValueType>
        class DFTExplorationHeuristic {

        public:
            DFTExplorationHeuristic();

            void setHeuristicValues(size_t depth, ValueType rate, ValueType exitRate);

            bool isSkip(double approximationThreshold, ApproximationHeuristic heuristic) const;

            void setNotSkip();

            size_t getDepth() const;

            double getPriority() const;
            
        private:

            bool skip;
            size_t depth;
            ValueType rate;
            ValueType exitRate;

        };

        template<typename ValueType>
        bool compareDepth(std::shared_ptr<storm::storage::DFTState<ValueType>> stateA, std::shared_ptr<storm::storage::DFTState<ValueType>> stateB);
    }
}

#endif /* STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_ */
