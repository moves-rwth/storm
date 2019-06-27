#pragma once

#include <boost/container/flat_set.hpp>

namespace storm {
    namespace storage {

        /*!
         * Redefinition of flat_set was needed, because from Boost 1.70 on the default allocator is set to void.
         */
        template<typename Key>
        using FlatSet = boost::container::flat_set<Key, std::less<Key>, boost::container::new_allocator<Key>>;

    }
}
