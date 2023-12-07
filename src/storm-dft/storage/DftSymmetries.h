#pragma once

#include <cstddef>
#include <iostream>
#include <map>
#include <vector>

namespace storm::dft {
namespace storage {

class DftSymmetries {
   public:
    DftSymmetries() = default;

    DftSymmetries(std::map<size_t, std::vector<std::vector<size_t>>> groups);

    size_t nrSymmetries() const;

    /*!
     * Retrieves an constant iterator that points to the beginning of the ordered symmetry groups.
     *
     * @return An iterator pointing to first symmetry.
     */
    std::vector<size_t>::const_iterator begin() const;

    /*!
     * Retrieves an constant iterator that points past the last ordered symmetry group.
     *
     * @return An iterator that points past the last symmetry.
     */
    std::vector<size_t>::const_iterator end() const;

    /*!
     * Get symmetry group corresponding to give top level index.
     *
     * @param index: Index of the top level element forming the symmetry group.
     * @return Symmetry group.
     */
    std::vector<std::vector<size_t>> const& getSymmetryGroup(size_t index) const;

    friend std::ostream& operator<<(std::ostream& out, DftSymmetries const& symmetries);

   private:
    bool existsInFirstSymmetry(size_t index, size_t value) const;

    bool existsInSymmetry(size_t index, size_t value) const;

    /**
     * Apply symmetry and get bijection. Result is symmetry(value)[index]
     *
     * @param symmetry The symmetry.
     * @param value The value to apply the bijection to.
     * @param index The index of the symmetry group to apply.
     * @return Symmetric value, -1 if the bijection could not be applied.
     */
    int applySymmetry(std::vector<std::vector<size_t>> const& symmetry, size_t value, size_t index) const;

    /**
     * Apply symmetry given by parentSymmetry and index to childSymmetry to obtain new symmetry.
     *
     * @param parentSymmetry Symmetry to apply.
     * @param childSymmetry Symmetry which should be changed according to parentSymmetry.
     * @param index Index in parent symmetry to use.
     * @return Child symmetry after applying the parent symmetry at the given index.
     */
    std::vector<std::vector<size_t>> createSymmetry(std::vector<std::vector<size_t>> parentSymmetry, std::vector<std::vector<size_t>> childSymmetry,
                                                    size_t index);

    void sortHierarchical(size_t parent, std::vector<size_t>& candidates);

    // Symmetry groups: top level element of group -> symmetry
    // Each symmetry is given by a list of the equivalence classes
    // Each equivalence class is given by all symmetric elements
    std::map<size_t, std::vector<std::vector<size_t>>> groups;

    // Top element of groups sorted by their size in increasing order
    std::vector<size_t> sortedSymmetries;
};

}  // namespace storage
}  // namespace storm::dft
