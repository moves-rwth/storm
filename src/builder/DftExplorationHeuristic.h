#ifndef STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_
#define STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_

#include <memory>

namespace storm {

    namespace builder {

        /*!
         * Enum representing the heuristic used for deciding which states to expand.
         */
        enum class ApproximationHeuristic { NONE, DEPTH, RATERATIO };


        /*!
         * General super class for appoximation heuristics.
         */
        template<typename ValueType>
        class DFTExplorationHeuristic {

        public:
            DFTExplorationHeuristic(size_t id);

            void updateHeuristicValues(size_t depth, ValueType rate, ValueType exitRate);

            virtual bool isSkip(double approximationThreshold) const = 0;

            void markExpand() {
                expand = true;
            }

            size_t getId() const {
                return id;
            }

            size_t getDepth() const {
                return depth;
            }

        protected:
            size_t id;
            bool expand;
            size_t depth;
            ValueType rate;
            ValueType exitRate;

        };

        template<typename ValueType>
        class DFTExplorationHeuristicNone : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicNone(size_t id);

            bool isSkip(double approximationThreshold) const override;

            bool operator<(DFTExplorationHeuristicNone<ValueType> const& other) const;
        };

        template<typename ValueType>
        class DFTExplorationHeuristicDepth : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicDepth(size_t id);

            bool isSkip(double approximationThreshold) const override;

            bool operator<(DFTExplorationHeuristicDepth<ValueType> const& other) const;
        };

        template<typename ValueType>
        class DFTExplorationHeuristicRateRatio : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicRateRatio(size_t id);

            bool isSkip(double approximationThreshold) const override;

            bool operator<(DFTExplorationHeuristicRateRatio<ValueType> const& other) const;

        private:
            double getRateRatio() const;
        };

    }
}

#endif /* STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_ */
