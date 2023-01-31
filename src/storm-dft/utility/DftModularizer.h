#pragma once

#include "storm-dft/storage/DFT.h"
#include "storm-dft/storage/DftModule.h"

namespace storm::dft {
namespace utility {

/*!
 * Find modules (independent subtrees) in DFT.
 * The computation follows the LTA/DR algorithm from:
 * Dutuit, Rauzy: "A linear-time algorithm to find modules of fault trees"
 * @see http://doi.org/10.1109/24.537011
 * Trivial modules containing single BEs are also contained.
 */
template<typename ValueType>
class DftModularizer {
   public:
    using DFTElementCPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const>;
    using ElementId = size_t;

    /*!
     * Constructor.
     */
    DftModularizer() = default;

    /*!
     * Compute modules of DFT by applying the LTA/DR algorithm.
     * @param dft DFT.
     * @return Independent module of top-level element. All sub-modules are contained in this module.
     */
    storm::dft::storage::DftIndependentModule computeModules(storm::dft::storage::DFT<ValueType> const& dft);

   private:
    /*!
     * Recursive function to perform first depth first search of the LTA/DR algorithm.
     * The function populates dfsCounter.
     * @param element Current DFT element.
     */
    void populateDfsCounters(DFTElementCPointer const element);

    /*!
     * Recursive function to obtain modularization information.
     * The function performs a second depth first search of the LTA/DR algorithm and populates modInfos.
     * @param element Current DFT element.
     */
    void obtainModules(DFTElementCPointer const element);

    /*!
     * Return all children for an element.
     * @param element DFT element.
     * @return List of children.
     */
    static std::vector<DFTElementCPointer> getChildren(DFTElementCPointer const element);

    /*!
     * Return all restrictions and dependencies affecting the element.
     * @param element DFT element.
     * @return List of elements affecting the element.
     */
    static std::vector<DFTElementCPointer> getAffectingElements(DFTElementCPointer const element);

    /*!
     * Check whether the list contains the element.
     * @param list List of elements.
     * @param element Element to search for.
     * @return True iff element is contained in list.
     */
    static bool containsElement(std::vector<DFTElementCPointer> const& list, DFTElementCPointer const element);

    /*!
     * Counters for each DFT element.
     * Used by the LTA/DR algorithm.
     */
    struct DfsCounter {
        uint64_t firstVisit{0};
        uint64_t secondVisit{0};
        uint64_t lastVisit{0};

        uint64_t minFirstVisit{0};
        uint64_t maxLastVisit{0};
    };
    struct ModularizationInfo {
        bool isModule = false;
        std::set<ElementId> elements;
        std::set<storm::dft::storage::DftIndependentModule> submodules;
        bool isStatic = true;
        bool fullyStatic = true;
    };

    std::map<ElementId, DfsCounter> dfsCounters{};
    std::map<ElementId, ModularizationInfo> modInfos{};
    uint64_t lastDate{};
};

}  // namespace utility
}  // namespace storm::dft
