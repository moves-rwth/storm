#pragma once

#include <list>
#include <memory>

#include "storm/storage/BitVector.h"

namespace storm::dft {
namespace storage {

// Forward declarations
template<typename ValueType>
class DFT;

namespace elements {

template<typename ValueType>
class DFTBE;
template<typename ValueType>
class DFTDependency;

}  // namespace elements

/*!
 * Handling of currently failable elements (BEs) either due to their own failure or because of dependencies.
 *
 * We distinguish between failures of BEs and failures of dependencies.
 * For dependencies, we further distinguish between non-conflicting and conflicting dependencies.
 * Non-conflicting dependencies will lead to spurious non-determinism.
 *
 * The class supports iterators for all failable elements (either BE or dependencies).
 *
 */
class FailableElements {
   public:
    /*!
     * Iterator for failable elements.
     *
     */
    class const_iterator {
       public:
        // Define iterator
        using iterator_category = std::input_iterator_tag;
        using value_type = size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = size_t*;
        using reference = size_t&;

        /*!
         * Construct a new iterator.
         * We either iterate over all failable BEs or over all dependencies (if dependency is true).
         * For dependencies, we start with non-conflicting dependencies before iterating over conflicting ones.
         *
         * @param dependency Whether dependencies should be iterated (or BEs if false).
         * @param conflicting Whether conflicting dependencies should be iterated (or non-conflicting if false).
         * @param iterBE Iterator for BEs.
         * @param iterDependency Iterator for Dependencies.
         * @param nonConflictEnd Iterator to end of non-conflicting dependencies.
         * @param conflictBegin Iterator to begin of conflicting dependencies.
         */
        const_iterator(bool dependency, bool conflicting, storm::storage::BitVector::const_iterator const& iterBE,
                       std::list<size_t>::const_iterator const& iterDependency, std::list<size_t>::const_iterator nonConflictEnd,
                       std::list<size_t>::const_iterator conflictBegin);

        /*!
         * Constructs an iterator by copying the given iterator.
         *
         * @param other The iterator to copy.
         */
        const_iterator(const_iterator const& other) = default;

        /*!
         * Assigns the contents of the given iterator to the current one via copying the former's contents.
         *
         * @param other The iterator from which to copy-assign.
         */
        const_iterator& operator=(const_iterator const& other) = default;

        /*!
         * Increment the iterator.
         *
         * @return A reference to this iterator.
         */
        const_iterator& operator++();

        /*!
         * Returns the id of the current failable element.
         *
         * @return Element id.
         */
        uint_fast64_t operator*() const;

        /*!
         * Compares the iterator with another iterator for inequality.
         *
         * @param other The iterator with respect to which inequality is checked.
         * @return True if the two iterators are unequal.
         */
        bool operator!=(const_iterator const& other) const;

        /*!
         * Compares the iterator with another iterator for equality.
         *
         * @param other The iterator with respect to which equality is checked.
         * @return True if the two iterators are equal.
         */
        bool operator==(const_iterator const& other) const;

        /*!
         * Return whether the current failure is due to a dependency (or the BE itself).
         *
         * @return true iff current failure is due to dependency.
         */
        bool isFailureDueToDependency() const;

        /*!
         * Return whether the current dependency failure is conflicting.
         *
         * @return true iff current dependency failure is conflicting.
         */
        bool isConflictingDependency() const;

        /*!
         * Obtain the BE which fails from the current iterator.
         * Optionally returns the dependency which triggered the BE failure.
         *
         * @tparam ValueType Value type.
         * @param dft DFT.
         * @return Pair of the BE which fails and the dependency which triggered the failure (or nullptr if the BE fails on its own).
         */
        template<typename ValueType>
        std::pair<std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>,
                  std::shared_ptr<storm::dft::storage::elements::DFTDependency<ValueType> const>>
        getFailBE(storm::dft::storage::DFT<ValueType> const& dft) const;

       private:
        // Whether dependencies are currently considered.
        bool dependency;
        // Whether the iterator currently points to a conflicting dependency.
        bool conflicting;
        // Iterators for underlying data structures
        storm::storage::BitVector::const_iterator itBE;
        std::list<size_t>::const_iterator itDep;

        // Pointers marking end of non-conflict list and beginning of conflict list
        // Used for sequential iteration over all dependencies
        std::list<size_t>::const_iterator nonConflictEnd;
        std::list<size_t>::const_iterator conflictBegin;
    };

    /*!
     * Creator.
     *
     * @param maxBEId Maximal id of a BE.
     */
    FailableElements(size_t maxBEId) : currentlyFailableBE(maxBEId) {}

    /*!
     * Add failable BE.
     * Note that this invalidates the iterator.
     *
     * @param id Id of BE.
     */
    void addBE(size_t id);

    /*!
     * Add failable dependency.
     * Note that this invalidates the iterator.
     *
     * @param id Id of dependency.
     * @param isConflicting Whether the dependency is in conflict to other dependencies. In this case
     *      the conflict needs to be resolved non-deterministically.
     */
    void addDependency(size_t id, bool isConflicting);

    /*!
     * Remove BE from list of failable elements.
     * Note that this invalidates the iterator.
     *
     * @param id Id of BE.
     */
    void removeBE(size_t id);

    /*!
     * Remove dependency from list of failable elements.
     * Note that this invalidates the iterator.
     *
     * @param id Id of dependency.
     */
    void removeDependency(size_t id);

    /*!
     * Clear list of currently failable elements.
     * Note that this invalidates the iterator.
     */
    void clear();

    /*!
     * Iterator to first failable element.
     * If dependencies are present, the iterator is for dependencies. Otherwise it is for BEs.
     * For dependencies, the iterator considers non-conflicting dependencies first.
     *
     * @param forceBE If true, failable dependencies are ignored and only BEs are considered.
     * @return Iterator.
     */
    FailableElements::const_iterator begin(bool forceBE = false) const;

    /*!
     * Iterator after last failable element.
     *
     * @param forceBE If true, failable dependencies are ignored and only BEs are considered.
     * @return Iterator.
     */
    FailableElements::const_iterator end(bool forceBE = false) const;

    /*!
     * Whether failable dependencies are present.
     *
     * @return true iff dependencies can fail.
     */
    bool hasDependencies() const;

    /*!
     * Whether failable BEs are present.
     *
     * @return true iff BEs can fail.
     */
    bool hasBEs() const;

    /*!
     * Get a string representation of the currently failable elements.
     *
     * @return std::string Enumeration of currently failable elements.
     */
    std::string getCurrentlyFailableString() const;

   private:
    // We use a BitVector for BEs but a list for dependencies, because usually only a few dependencies are failable at the same time.
    // In contrast, usually most BEs are failable.
    // The lists of failable elements are sorted by increasing id.
    storm::storage::BitVector currentlyFailableBE;
    std::list<size_t> failableConflictingDependencies;
    std::list<size_t> failableNonconflictingDependencies;
};

}  // namespace storage
}  // namespace storm::dft
