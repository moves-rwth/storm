#pragma once

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

#include "storm-dft/builder/DftExplorationHeuristic.h"

namespace storm::dft {
namespace storage {

/*!
 * Priority queue based on buckets.
 * Can be used to keep track of states during state space exploration.
 * @tparam PriorityType Underlying priority data type
 */
template<class PriorityType>
class BucketPriorityQueue {
    using PriorityTypePointer = std::shared_ptr<PriorityType>;

   public:
    /*!
     * Create new priority queue.
     * @param nrBuckets
     * @param lowerValue
     * @param ratio
     * @param higher
     */
    explicit BucketPriorityQueue(size_t nrBuckets, double lowerValue, double ratio, bool higher);

    BucketPriorityQueue(BucketPriorityQueue const& queue) = default;

    virtual ~BucketPriorityQueue() = default;

    void fix();

    /*!
     * Check whether queue is empty.
     * @return True iff queue is empty.
     */
    bool empty() const;

    /*!
     * Return number of entries.
     * @return Size of queue.
     */
    std::size_t size() const;

    /*!
     * Get element with highest priority.
     * @return Top element.
     */
    PriorityTypePointer const& top() const;

    /*!
     * Add element.
     * @param item Element.
     */
    void push(PriorityTypePointer const& item);

    /*!
     * Update existing element.
     * @param item Element with changes.
     * @param oldPriority Old priority.
     */
    void update(PriorityTypePointer const& item, double oldPriority);

    /*!
     * Get element with highest priority and remove it from the queue.
     * @return Top element.
     */
    PriorityTypePointer pop();

    /*!
     * Print info about priority queue.
     * @param out Output stream.
     */
    void print(std::ostream& out) const;

    /*!
     * Print sizes of buckets.
     * @param out Output stream.
     */
    void printSizes(std::ostream& out) const;

   private:
    /*!
     * Get bucket for given priority.
     * @param priority Priority.
     * @return Bucket containing the priority.
     */
    size_t getBucket(double priority) const;

    // List of buckets
    std::vector<std::vector<PriorityTypePointer>> buckets;

    // Bucket containing all items which should be considered immediately
    std::vector<PriorityTypePointer> immediateBucket;

    // Index of first bucket which contains items
    size_t currentBucket;

    // Number of unsorted items in current bucket
    size_t nrUnsortedItems;

    // Comparison function for priorities
    std::function<bool(PriorityTypePointer, PriorityTypePointer)> compare;

    // Minimal value
    double lowerValue;

    bool higher;

    bool AUTOSORT = false;

    double logBase;

    // Number of available buckets
    size_t nrBuckets;
};

}  // namespace storage
}  // namespace storm::dft
