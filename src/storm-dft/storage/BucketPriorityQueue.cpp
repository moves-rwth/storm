#include "BucketPriorityQueue.h"
#include "storm/utility/macros.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include <cmath>

namespace storm {
    namespace storage {

        template<typename ValueType>
        BucketPriorityQueue<ValueType>::BucketPriorityQueue(size_t nrBuckets, double lowerValue, double ratio, bool higher) : lowerValue(lowerValue), higher(higher), logBase(std::log(ratio)), nrBuckets(nrBuckets), nrUnsortedItems(0), buckets(nrBuckets), currentBucket(nrBuckets) {
            compare = ([](HeuristicPointer a, HeuristicPointer b) {
                return *a < *b;
            });
        }

        template<typename ValueType>
        void BucketPriorityQueue<ValueType>::fix() {
            if (currentBucket < nrBuckets && nrUnsortedItems > buckets[currentBucket].size() / 10) {
                // Fix current bucket
                std::make_heap(buckets[currentBucket].begin(), buckets[currentBucket].end(), compare);
                nrUnsortedItems = 0;
            }
        }

        template<typename ValueType>
        bool BucketPriorityQueue<ValueType>::empty() const {
            return currentBucket == nrBuckets && immediateBucket.empty();
        }

        template<typename ValueType>
        std::size_t BucketPriorityQueue<ValueType>::size() const {
            size_t size = immediateBucket.size();
            for (size_t i = currentBucket; currentBucket < nrBuckets; ++i) {
                size += buckets[i].size();
            }
            return size;
        }

        template<typename ValueType>
        typename BucketPriorityQueue<ValueType>::HeuristicPointer const& BucketPriorityQueue<ValueType>::top() const {
            if (!immediateBucket.empty()) {
                return immediateBucket.back();
            }
            STORM_LOG_ASSERT(!empty(), "BucketPriorityQueue is empty");
            return buckets[currentBucket].front();
        }

        template<typename ValueType>
        void BucketPriorityQueue<ValueType>::push(HeuristicPointer const& item) {
            if (item->isExpand()) {
                immediateBucket.push_back(item);
                return;
            }
            size_t bucket = getBucket(item->getPriority());
            if (bucket < currentBucket) {
                currentBucket = bucket;
                nrUnsortedItems = 0;
            }
            buckets[bucket].push_back(item);
            if (bucket == currentBucket) {
                // Insert in first bucket
                if (AUTOSORT) {
                    std::push_heap(buckets[currentBucket].begin(), buckets[currentBucket].end(), compare);
                } else {
                    ++nrUnsortedItems;
                }
            }
        }

        template<typename ValueType>
        void BucketPriorityQueue<ValueType>::update(HeuristicPointer const& item, double oldPriority) {
            STORM_LOG_ASSERT(!item->isExpand(), "Item is marked for expansion");
            size_t newBucket = getBucket(item->getPriority());
            size_t oldBucket = getBucket(oldPriority);

            if (oldBucket == newBucket) {
                if (currentBucket < newBucket) {
                    // No change as the bucket is not sorted yet
                } else {
                    if (AUTOSORT) {
                        // Sort first bucket
                        fix();
                    } else {
                        ++nrUnsortedItems;
                    }
                }
            } else {
                // Move to new bucket
                STORM_LOG_ASSERT(newBucket < oldBucket, "Will update to higher bucket");
                if (newBucket < currentBucket) {
                    currentBucket = newBucket;
                    nrUnsortedItems = 0;
                }
                // Remove old entry by swap-and-pop
                if (buckets[oldBucket].size() >= 2) {
                    // Find old index by linear search
                    // Notice: using a map to rememeber index was not efficient
                    size_t oldIndex = 0;
                    for ( ; oldIndex < buckets[oldBucket].size(); ++oldIndex) {
                        if (buckets[oldBucket][oldIndex]->getId() == item->getId()) {
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(oldIndex < buckets[oldBucket].size(), "Id " << item->getId() << " not found");
                    std::iter_swap(buckets[oldBucket].begin() + oldIndex, buckets[oldBucket].end() - 1);
                }
                buckets[oldBucket].pop_back();
                // Insert new element
                buckets[newBucket].push_back(item);
                if (newBucket == currentBucket) {
                    if (AUTOSORT) {
                        // Sort in first bucket
                        std::push_heap(buckets[currentBucket].begin(), buckets[currentBucket].end(), compare);
                    } else {
                        ++nrUnsortedItems;
                    }
                }
            }
        }


        template<typename ValueType>
        void BucketPriorityQueue<ValueType>::pop() {
            if (!immediateBucket.empty()) {
                immediateBucket.pop_back();
                return;
            }
            STORM_LOG_ASSERT(!empty(), "BucketPriorityQueue is empty");
            std::pop_heap(buckets[currentBucket].begin(), buckets[currentBucket].end(), compare);
            buckets[currentBucket].pop_back();
            if (buckets[currentBucket].empty()) {
                // Find next bucket with elements
                for ( ; currentBucket < nrBuckets; ++currentBucket) {
                    if (!buckets[currentBucket].empty()) {
                        nrUnsortedItems = buckets[currentBucket].size();
                        if (AUTOSORT) {
                            fix();
                        }
                        break;
                    }
                }
            }
        }

        template<typename ValueType>
        typename BucketPriorityQueue<ValueType>::HeuristicPointer BucketPriorityQueue<ValueType>::popTop() {
            HeuristicPointer item = top();
            pop();
            return item;
        }

        template<typename ValueType>
        size_t BucketPriorityQueue<ValueType>::getBucket(double priority) const {
            STORM_LOG_ASSERT(priority >= lowerValue, "Priority " << priority << " is too low");

            // For possible values greater 1
            double tmpBucket = std::log(priority - lowerValue) / logBase;
            tmpBucket += buckets.size() / 2;
            if (tmpBucket < 0) {
                tmpBucket = 0;
            }
            size_t newBucket = tmpBucket;
            // For values ensured to be lower 1
            //size_t newBucket = std::log(priority - lowerValue) / logBase;
            if (newBucket >= nrBuckets) {
                newBucket = nrBuckets - 1;
            }
            if (!higher) {
                newBucket = nrBuckets-1 - newBucket;
            }
            //std::cout << "get Bucket: " << priority << ", " << newBucket << std::endl;
            STORM_LOG_ASSERT(newBucket < nrBuckets, "Priority " << priority << " is too high");
            return newBucket;
        }

        template<typename ValueType>
        void BucketPriorityQueue<ValueType>::print(std::ostream& out) const {
            out << "Bucket priority queue with size " << buckets.size() << ", lower value: " << lowerValue << " and logBase: " << logBase << std::endl;
            out << "Immediate bucket: ";
            for (HeuristicPointer heuristic : immediateBucket) {
                out << heuristic->getId() << ", ";
            }
            out << std::endl;
            out << "Current bucket (" << currentBucket << ") has " << nrUnsortedItems  << " unsorted items" << std::endl;
            for (size_t bucket = 0; bucket < buckets.size(); ++bucket) {
                if (!buckets[bucket].empty()) {
                    out << "Bucket " << bucket << ":" << std::endl;
                    for (HeuristicPointer heuristic : buckets[bucket]) {
                        out << "\t" << heuristic->getId() << ": " << heuristic->getPriority() << std::endl;
                    }
                }
            }
        }

        template<typename ValueType>
        void BucketPriorityQueue<ValueType>::printSizes(std::ostream& out) const {
            out << "Bucket sizes: " << immediateBucket.size() << " | ";
            for (size_t bucket = 0; bucket < buckets.size(); ++bucket) {
                out << buckets[bucket].size() << " ";
            }
            std::cout << std::endl;
        }

        // Template instantiations
        template class BucketPriorityQueue<double>;

#ifdef STORM_HAVE_CARL
        template class BucketPriorityQueue<storm::RationalFunction>;
#endif
    }
}
