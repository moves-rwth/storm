#ifndef STORM_STORAGE_BUCKETPRIORITYQUEUE_H_
#define STORM_STORAGE_BUCKETPRIORITYQUEUE_H_

#include "src/builder/DftExplorationHeuristic.h"
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

namespace storm {
    namespace storage {

        template<typename ValueType>
        class BucketPriorityQueue {

            using HeuristicPointer = std::shared_ptr<storm::builder::DFTExplorationHeuristicBoundDifference<ValueType>>;

        public:
            explicit BucketPriorityQueue(size_t nrBuckets, double lowerValue, double ratio);

            void fix();

            bool empty() const;

            std::size_t size() const;

            HeuristicPointer const& top() const;

            void push(HeuristicPointer const& item);

            void update(HeuristicPointer const& item, double oldPriority);

            void pop();

            HeuristicPointer popTop();

            void print(std::ostream& out) const;

            void printSizes(std::ostream& out) const;

        private:

            size_t getBucket(double priority) const;

            const double lowerValue;

            const bool HIGHER = true;

            const bool AUTOSORT = false;

            const double logBase;

            const size_t nrBuckets;

            size_t nrUnsortedItems;

            // List of buckets
            std::vector<std::vector<HeuristicPointer>> buckets;

            // Bucket containing all items which should be considered immediately
            std::vector<HeuristicPointer> immediateBucket;

            // Index of first bucket which contains items
            size_t currentBucket;

            std::function<bool(HeuristicPointer, HeuristicPointer)> compare;

        };

    }
}

#endif // STORM_STORAGE_BUCKETPRIORITYQUEUE_H_
