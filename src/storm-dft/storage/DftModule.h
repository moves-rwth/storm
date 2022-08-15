#pragma once

#include <string>
#include <vector>

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
     * @param elements List of elements forming the module. Representative must be contained.
     */
    DftModule(size_t representative, std::vector<size_t> const& elements);

    /*!
     * Get representative (top element of subtree).
     * @return Id of representative.
     */
    size_t getRepresentative() const {
        return representative;
    }

    /*!
     * Begin iterator for elements.
     * @return Iterator.
     */
    std::vector<size_t>::const_iterator begin() const {
        return elements.begin();
    }

    /*!
     * End iterator for elements.
     * @return Ierator.
     */
    std::vector<size_t>::const_iterator end() const {
        return elements.end();
    }

    /*!
     * Check whether the module is empty.
     * @return True iff no elements are contained.
     */
    bool empty() const {
        return elements.empty();
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
    std::vector<size_t> elements;
};

/**
 * Represents an independent module/subtree.
 */
class DftIndependentModule : public DftModule {
   public:
    /*!
     * Constructor.
     * @param Id of representative, ie top element of the subtree.
     * @param elements List of elements forming the module. Representative must be contained.
     * @param isStatic Whether the independent module only contains static elements.
     */
    DftIndependentModule(size_t representative, std::vector<size_t> const& elements, bool isStatic);

    /*!
     * Returns whether the module is static.
     * @return True iff the module contains no dynamic element.
     */
    bool isStaticModule() const {
        return staticModule;
    }

    /*!
     * Compute the type of the module: static (only static elements) or dynamic (at least one dynamic element).
     * Sets an internal variable which allows to use isStaticModule() afterwards.
     * @param dft DFT.
     */
    template<typename ValueType>
    void computeType(storm::dft::storage::DFT<ValueType> const& dft);

   private:
    bool staticModule;
};

}  // namespace storage
}  // namespace storm::dft
