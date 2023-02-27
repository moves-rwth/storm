#pragma once

#include <vector>

#include "storm/generator/DistributionEntry.h"

namespace storm::generator {

template<typename IndexType, typename ValueType>
class Distribution {
   public:
    typedef std::vector<DistributionEntry<IndexType, ValueType>> ContainerType;

    Distribution();

    Distribution(Distribution const&);
    Distribution(Distribution&&);
    Distribution& operator=(Distribution const&);
    Distribution& operator=(Distribution&&);

    /*!
     * Adds the given entry to the distribution.
     */
    void add(DistributionEntry<IndexType, ValueType> const& entry);

    /*!
     * Adds the given entry to the distribution.
     */
    void add(IndexType const& index, ValueType const& value);

    /*!
     * Adds the given other distribution to the distribution.
     */
    void add(Distribution&& distribution);

    /*!
     * Compresses the internal storage by summing the values of entries which agree on the index. As a side
     * effect, this sorts the entries in the distribution by their index.
     */
    void compress();

    /*!
     * Divides all values in the distribution by the provided value.
     */
    void divide(ValueType const& value);

    /*!
     * Clears this distribution.
     */
    void clear();

    /*!
     * Access to iterators over the entries of the distribution. Note that there may be multiple entries for
     * the same index. Also, no order is guaranteed. After a call to compress, the order is guaranteed to be
     * ascending wrt. index and there are no elements with the same index.
     */
    typename ContainerType::iterator begin();
    typename ContainerType::const_iterator begin() const;
    typename ContainerType::iterator end();
    typename ContainerType::const_iterator end() const;

   private:
    // The underlying storage of the distribution.
    ContainerType storage;

    bool compressed;
};

}  // namespace storm::generator