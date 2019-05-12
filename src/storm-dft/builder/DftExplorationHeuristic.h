#pragma once
#include <memory>

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"


namespace storm {

    namespace builder {

        /*!
         * Enum representing the heuristic used for deciding which states to expand.
         */
        enum class ApproximationHeuristic { DEPTH, PROBABILITY, BOUNDDIFFERENCE };


        /*!
         * General super class for approximation heuristics.
         */
        template<typename ValueType>
        class DFTExplorationHeuristic {

        public:
            explicit DFTExplorationHeuristic(size_t id) : id(id), expand(false), lowerBound(storm::utility::zero<ValueType>()), upperBound(storm::utility::infinity<ValueType>()), depth(0), probability(storm::utility::one<ValueType>()) {
                // Intentionally left empty
            }

            DFTExplorationHeuristic(size_t id, DFTExplorationHeuristic const& predecessor, ValueType rate, ValueType exitRate) : id(id), expand(false), lowerBound(storm::utility::zero<ValueType>()), upperBound(storm::utility::infinity<ValueType>()), depth(predecessor.depth + 1), probability(storm::utility::zero<ValueType>()) {
                this->updateHeuristicValues(predecessor, rate, exitRate);
            }

            virtual ~DFTExplorationHeuristic() = default;

            void setBounds(ValueType lowerBound, ValueType upperBound) {
                this->lowerBound = lowerBound;
                this->upperBound = upperBound;
            }

            virtual bool updateHeuristicValues(DFTExplorationHeuristic const& predecessor, ValueType rate, ValueType exitRate) {
                STORM_LOG_ASSERT(!storm::utility::isZero<ValueType>(exitRate), "Exit rate is 0");
                probability += predecessor.getProbability() * rate/exitRate;
                return true;
            }

            void markExpand() {
                expand = true;
            }

            size_t getId() const {
                return id;
            }

            bool isExpand() const {
                return expand;
            }

            size_t getDepth() const {
                return depth;
            }

            ValueType getProbability() const {
                return probability;
            }

            ValueType getLowerBound() const {
                return lowerBound;
            }

            ValueType getUpperBound() const {
                return upperBound;
            }

            virtual double getPriority() const = 0;

            virtual bool isSkip(double approximationThreshold) const {
                return !this->isExpand() && this->getPriority() < approximationThreshold;
            }

            virtual bool operator<(DFTExplorationHeuristic<ValueType> const& other) const {
                return this->getPriority() < other.getPriority();
            }

        protected:
            size_t id;
            bool expand;
            ValueType lowerBound;
            ValueType upperBound;
            size_t depth;
            ValueType probability;
        };

        template<typename ValueType>
        class DFTExplorationHeuristicDepth : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicDepth(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
                // Intentionally left empty
            }

            DFTExplorationHeuristicDepth(size_t id, DFTExplorationHeuristic<ValueType> const& predecessor, ValueType rate, ValueType exitRate) : DFTExplorationHeuristic<ValueType>(id, predecessor, rate, exitRate) {
                // Intentionally left empty
            }

            bool updateHeuristicValues(DFTExplorationHeuristic<ValueType> const& predecessor, ValueType, ValueType) override {
                if (predecessor.getDepth() + 1 < this->depth) {
                    this->depth = predecessor.getDepth() + 1;
                    return true;
                }
                return false;
            }

            double getPriority() const override {
                return this->depth;
            }

            bool isSkip(double approximationThreshold) const override {
                return !this->expand && this->getPriority() > approximationThreshold;
            }

            bool operator<(DFTExplorationHeuristic<ValueType> const& other) const override {
                return this->getPriority() > other.getPriority();
            }
        };

        template<typename ValueType>
        class DFTExplorationHeuristicProbability : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicProbability(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
                // Intentionally left empty
            }

            DFTExplorationHeuristicProbability(size_t id, DFTExplorationHeuristic<ValueType> const& predecessor, ValueType rate, ValueType exitRate) : DFTExplorationHeuristic<ValueType>(id, predecessor, rate, exitRate) {
                // Intentionally left empty
            }

            double getPriority() const override;
        };

        template<typename ValueType>
        class DFTExplorationHeuristicBoundDifference : public DFTExplorationHeuristic<ValueType> {
        public:
            DFTExplorationHeuristicBoundDifference(size_t id) : DFTExplorationHeuristic<ValueType>(id) {
                // Intentionally left empty
            }

            DFTExplorationHeuristicBoundDifference(size_t id, DFTExplorationHeuristic<ValueType> const& predecessor, ValueType rate, ValueType exitRate) : DFTExplorationHeuristic<ValueType>(id, predecessor, rate, exitRate) {
                // Intentionally left empty
            }

            double getPriority() const override;
        };

    }
}
