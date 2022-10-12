#pragma once

#include <set>
#include <string>

#include "storm/utility/macros.h"

namespace storm::dft {
namespace storage {

// Forward declaration
template<typename ValueType>
class DFT;

/**
 * Represents a module/subtree in a DFT.
 */
class DftModule {
   public:
    /*!
     * Constructor.
     * @param Id of representative, ie top element of the subtree.
     * @param elements Set of element ids forming the module.
     */
    DftModule(size_t representative, std::set<size_t> const& elements);

    /*!
     * Get representative (top element of subtree).
     * @return Id of representative.
     */
    size_t getRepresentative() const {
        return representative;
    }

    /*!
     * Return elements of module.
     * @return Set of element ids.
     */
    std::set<size_t> const& getElements() const {
        return elements;
    }

    /*!
     * Clear list of elements.
     */
    void clear() {
        elements.clear();
    }

    /*!
     * Get string representation of module.
     * @param dft DFT.
     * @return Module representative with list of elements in the module.
     */
    template<typename ValueType>
    std::string toString(storm::dft::storage::DFT<ValueType> const& dft) const;

   protected:
    size_t representative;
    std::set<size_t> elements;
};

/**
 * Represents an independent module/subtree.
 */
class DftIndependentModule : public DftModule {
   public:
    /*!
     * Constructor. The elements within the module are a union of elements and the elements of submodules.
     * @param Id of representative, ie top element of the subtree.
     * @param elements Set of elements forming the module. Representative must be contained.
     * @param submodules Set of sub-modules contained in the module.
     * @param staticElements Whether the module contains only static elements. Dynamic elements in sub-modules are allowed.
     * @param fullyStatic Whether the independent module contains only static static elements and all sub-modules also contain only static elements.
     * @param singleBE Whether the independent module consists of a single BE.
     */
    DftIndependentModule(size_t representative, std::set<size_t> const& elements, std::set<DftIndependentModule> const& submodules, bool staticElements,
                         bool fullyStatic, bool singleBE);

    /*!
     * Returns whether the module contains only static elements (except in sub-modules).
     * Dynamic elements in sub-modules are allowed.
     * @return True iff the module contains no dynamic element.
     */
    bool isStatic() const {
        return staticElements;
    }

    /*!
     * Returns whether the module contains only static elements (also in sub-modules).
     * @return True iff the module contains no dynamic element at all.
     */
    bool isFullyStatic() const {
        return fullyStatic;
    }

    /*!
     * Returns sub-modules.
     * @return Set of sub-modules.
     */
    std::set<DftIndependentModule> const& getSubModules() const {
        return submodules;
    }

    /*!
     * Returns whether the module is a single BE, i.e., a trivial module.
     * @return Whether it is a single BE.
     */
    bool isSingleBE() const {
        return singleBE;
    }

    /*!
     * Returns all elements contained in the module (including sub-modules).
     * @return Set of all elements.
     */
    std::set<size_t> getAllElements() const;

    /*!
     * Create subtree corresponding to module.
     * @param dft (Complete) DFT.
     * @return Subtree corresponding to independent module.
     */
    template<typename ValueType>
    storm::dft::storage::DFT<ValueType> getSubtree(storm::dft::storage::DFT<ValueType> const& dft) const;

    /*!
     * Get string representation of module.
     * @param dft DFT.
     * @param indentation Whitespace indentation giving better layout for sub-modules.
     * @return Module representative with list of elements and sub-modules in the module.
     */
    template<typename ValueType>
    std::string toString(storm::dft::storage::DFT<ValueType> const& dft, std::string const& indentation = "") const;

    bool operator<(DftIndependentModule const& other) const {
        return this->getRepresentative() < other.getRepresentative();
    }

   private:
    bool staticElements;
    bool fullyStatic;
    bool singleBE;
    std::set<DftIndependentModule> submodules;
};

}  // namespace storage
}  // namespace storm::dft
