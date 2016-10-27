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

            using HeuristicPointer = std::shared_ptr<storm::builder::DFTExplorationHeuristicProbability<ValueType>>;

        public:
            explicit BucketPriorityQueue(size_t nrBuckets, double lowerValue, double stepPerBucket);

            void fix();

            bool empty() const;

            std::size_t size() const;

            HeuristicPointer const& top() const;

            void push(HeuristicPointer const& item);

            void update(HeuristicPointer const& item, double oldPriority);

            void pop();

            HeuristicPointer popTop();

            void print(std::ostream& out) const;

        private:

            size_t getBucket(double priority) const;

            // List of buckets
            std::vector<std::vector<HeuristicPointer>> buckets;

            // Bucket containing all items which should be considered immediately
            std::vector<HeuristicPointer> immediateBucket;

            // Index of first bucket which contains items
            size_t currentBucket;

            std::function<bool(HeuristicPointer, HeuristicPointer)> compare;

            double lowerValue;

            double stepPerBucket;

            size_t nrUnsortedItems;

            const bool HIGHER = true;

            const bool AUTOSORT = false;
        };

    }
}

#endif // STORM_STORAGE_BUCKETPRIORITYQUEUE_H_
