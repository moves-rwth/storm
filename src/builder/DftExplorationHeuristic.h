#ifndef STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_
#define STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_

#include <memory>

namespace storm {

    namespace builder {

        /*!
         * Enum representing the heuristic used for deciding which states to expand.
         */
        enum class ApproximationHeuristic { NONE, DEPTH, PROBABILITY };


        /*!
         * General super class for appoximation heuristics.
         */
        template<typename ValueType>
        class DFTExplorationHeuristic {

        public:
            DFTExplorationHeuristic(size_t id);

            DFTExplorationHeuristic(size_t id, DFTExplorationHeuristic const& predecessor, ValueType rate, ValueType exitRate);

            virtual bool updateHeuristicValues(DFTExplorationHeuristic const& predecessor, ValueType rate, ValueType exitRate) = 0;

            virtual double getPriority() const = 0;

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

            ValueType getProbability() const {
                return probability;
            }

        protected:
            size_t id;
            bool expand;
            size_t depth;
            ValueType probability;
        };

        template<typename ValueType>
        class DFTExplorationHeuristicNone : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicNone(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
                // Intentionally left empty
            }

            DFTExplorationHeuristicNone(size_t id, DFTExplorationHeuristicNone<ValueType> const& predecessor, ValueType rate, ValueType exitRate) : DFTExplorationHeuristic<ValueType>(id, predecessor, rate, exitRate) {
                // Intentionally left empty
            }

            bool updateHeuristicValues(DFTExplorationHeuristic<ValueType> const& predecessor, ValueType rate, ValueType exitRate) override {
                return false;
            }

            double getPriority() const override {
                return this->id;
            }

            bool isSkip(double approximationThreshold) const override {
                return false;
            }

            bool operator<(DFTExplorationHeuristicNone<ValueType> const& other) const {
                return this->id > other.id;
            }
        };

        template<typename ValueType>
        class DFTExplorationHeuristicDepth : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicDepth(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
                // Intentionally left empty
            }

            DFTExplorationHeuristicDepth(size_t id, DFTExplorationHeuristicDepth<ValueType> const& predecessor, ValueType rate, ValueType exitRate) : DFTExplorationHeuristic<ValueType>(id, predecessor, rate, exitRate) {
                // Intentionally left empty
            }

            bool updateHeuristicValues(DFTExplorationHeuristic<ValueType> const& predecessor, ValueType rate, ValueType exitRate) override {
                if (predecessor.getDepth() < this->depth) {
                    this->depth = predecessor.getDepth();
                    return true;
                }
                return false;
            }


            double getPriority() const override {
                return this->depth;
            }

            bool isSkip(double approximationThreshold) const override {
                return !this->expand && this->depth > approximationThreshold;
            }

            bool operator<(DFTExplorationHeuristicDepth<ValueType> const& other) const {
                return this->depth > other.depth;
            }
        };

        template<typename ValueType>
        class DFTExplorationHeuristicProbability : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicProbability(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
                // Intentionally left empty
            }

            DFTExplorationHeuristicProbability(size_t id, DFTExplorationHeuristicProbability<ValueType> const& predecessor, ValueType rate, ValueType exitRate) : DFTExplorationHeuristic<ValueType>(id, predecessor, rate, exitRate) {
                // Intentionally left empty
            }

            bool updateHeuristicValues(DFTExplorationHeuristic<ValueType> const& predecessor, ValueType rate, ValueType exitRate) override;

            double getPriority() const override;

            bool isSkip(double approximationThreshold) const override {
                return !this->expand && this->getPriority() < approximationThreshold;
            }

            bool operator<(DFTExplorationHeuristicProbability<ValueType> const& other) const {
                return this->getPriority() < other.getPriority();
            }
        };

    }
}

#endif /* STORM_BUILDER_DFTEXPLORATIONHEURISTIC_H_ */
