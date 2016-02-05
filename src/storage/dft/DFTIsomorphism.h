#pragma once

#include <vector>

namespace storm {
namespace storage {
    /**
     * Saves isomorphism between subtrees
     */
    class DFTIsomorphism {
        std::vector<std::vector<size_t>> classes;
    };

    /**
     * Saves sets of isomorphic subtrees
     */
    class DFTIsomorphisms {
        std::vector<DFTIsomorphism> isomorphisms;

        void addIsomorphismClass(DFTIsomorphism const &) {

        }
    };
} // namespace storm::dft
} // namespace storm
