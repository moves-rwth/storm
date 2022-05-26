#pragma once

#include "storm/utility/macros.h"

namespace storm::dft {
namespace storage {

struct DFTIndependentSymmetries {
    std::map<size_t, std::vector<std::vector<size_t>>> groups;  // Symmetry groups: top level element of group -> symmetry
    // Each symmetry is given by a list of the equivalence classes
    // Each equivalence class is given by all symmetric elements

    std::vector<size_t> sortedSymmetries;  // Top element of groups sorted by their size in increasing order

    bool existsInFirstSymmetry(size_t index, size_t value) {
        for (std::vector<size_t> symmetry : groups[index]) {
            if (symmetry.front() == value) {
                return true;
            }
        }
        return false;
    }

    bool existsInSymmetry(size_t index, size_t value) {
        for (std::vector<size_t> symmetry : groups[index]) {
            for (size_t index : symmetry) {
                if (index == value) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Apply symmetry and get bijection. Result is symmetry(value)[index]
     *
     * @param symmetry The symmetry.
     * @param value The value to apply the bijection to.
     * @param index The index of the symmetry group to apply.
     * @return Symmetric value, -1 if the bijection could not be applied.
     */
    int applySymmetry(std::vector<std::vector<size_t>> const& symmetry, size_t value, size_t index) const {
        for (std::vector<size_t> element : symmetry) {
            if (element[0] == value) {
                return element[index];
            }
        }
        return -1;
    }

    /**
     * Apply symmetry given by parentSymmetry and index to childSymmetry to obtain new symmetry.
     *
     * @param parentSymmetry Symmetry to apply.
     * @param childSymmetry Symmetry which should be changed according to parentSymmetry.
     * @param index Index in parent symmetry to use.
     * @return Child symmetry after applying the parent symmetry at the given index.
     */
    std::vector<std::vector<size_t>> createSymmetry(std::vector<std::vector<size_t>> parentSymmetry, std::vector<std::vector<size_t>> childSymmetry,
                                                    size_t index) {
        std::vector<std::vector<size_t>> result;
        for (std::vector<size_t> childSym : childSymmetry) {
            std::vector<size_t> symmetry;
            for (size_t child : childSym) {
                int bijectionValue = applySymmetry(parentSymmetry, child, index);
                if (bijectionValue >= 0) {
                    symmetry.push_back(bijectionValue);
                } else {
                    STORM_LOG_ASSERT(result.empty(), "Returning inconsistent result.");
                    return result;
                }
            }
            result.push_back(symmetry);
        }
        return result;
    }

    void sortHierarchical(size_t parent, std::vector<size_t>& candidates) {
        // Find subsymmetries of current symmetry
        std::vector<size_t> children;
        for (int i = candidates.size() - 1; i >= 0; --i) {
            size_t currentRoot = candidates[i];
            if (existsInSymmetry(parent, currentRoot)) {
                // Is child
                STORM_LOG_TRACE(currentRoot << " is child of " << parent);
                children.insert(children.begin(), currentRoot);
                candidates.erase(candidates.begin() + i);  // Removing is okay as i is decremented
            }
        }

        // Find child symmetries which are created by parent and other child symmetries
        for (size_t i = 0; i < children.size(); ++i) {
            // Iterate over all possible symmetry groups
            for (size_t index = 1; index < groups.at(parent).front().size(); ++index) {
                std::vector<std::vector<size_t>> possibleSymmetry = createSymmetry(groups.at(parent), groups.at(children[i]), index);
                if (possibleSymmetry.empty()) {
                    // No symmetry created
                    break;
                }
                for (size_t j = 0; j < children.size(); /*manual increment*/) {
                    if (j == i) {
                        // Ignore identity
                        ++j;
                        continue;
                    }
                    if (possibleSymmetry == groups.at(children[j])) {
                        STORM_LOG_TRACE("Child " << children[j] << " ignored as created by symmetries " << parent << " and " << children[i]);
                        groups.erase(children[j]);
                        children.erase(children.begin() + j);
                        // Update iterator
                        if (i > j) {
                            --i;
                        }
                    } else {
                        // Consider next element
                        ++j;
                    }
                }
            }
        }

        // Apply sorting recursively
        while (!children.empty()) {
            // Insert largest element
            size_t largestChild = children.back();
            children.pop_back();
            sortHierarchical(largestChild, children);
            sortedSymmetries.push_back(largestChild);
        }
    }

    DFTIndependentSymmetries(std::map<size_t, std::vector<std::vector<size_t>>> groups) : groups(groups) {
        std::vector<size_t> sortedGroups;
        for (auto const& cl : groups) {
            sortedGroups.push_back(cl.first);
        }
        // Sort by length of symmetry or (if equal) by lower first element
        std::sort(sortedGroups.begin(), sortedGroups.end(), [&](const size_t left, const size_t right) {
            return groups.at(left).size() < groups.at(right).size() ||
                   (groups.at(left).size() == groups.at(right).size() && groups.at(left).front().front() < groups.at(right).front().front());
        });

        // Sort hierarchical
        while (!sortedGroups.empty()) {
            // Insert largest element
            size_t currentRoot = sortedGroups.back();
            sortedGroups.pop_back();
            sortHierarchical(currentRoot, sortedGroups);
            sortedSymmetries.push_back(currentRoot);
        }
    }
};

inline std::ostream& operator<<(std::ostream& os, DFTIndependentSymmetries const& s) {
    for (size_t index : s.sortedSymmetries) {
        os << "Symmetry group for " << index << '\n';
        for (auto const& eqClass : s.groups.at(index)) {
            for (auto const& i : eqClass) {
                os << i << " ";
            }
            os << '\n';
        }
    }
    return os;
}
}  // namespace storage
}  // namespace storm::dft
