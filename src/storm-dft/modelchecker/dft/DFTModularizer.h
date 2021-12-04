#pragma once

#include <memory>
#include <vector>

#include "storm-dft/storage/SylvanBddManager.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm/logic/Formula.h"

namespace storm {
namespace modelchecker {

/**
 * Main class for BDD accelerated DFT checking
 *
 * \note
 * All public functions must make sure that workDFT
 * is set correctly and should assume
 * workDFT to be in an erroneous state.
 */
class DFTModularizer {
   public:
    using ValueType = double;
    using ElementId = size_t;
    using DFTElementCPointer =
        std::shared_ptr<storm::storage::DFTElement<ValueType> const>;
    using FormulaCPointer = std::shared_ptr<storm::logic::Formula const>;
    using FormulaVector = std::vector<FormulaCPointer>;

    /**
     * Calculates Modules
     */
    DFTModularizer(std::shared_ptr<storm::storage::DFT<ValueType>> dft);

    /**
     * Calculate the properties specified by the formulas
     * \param formuals
     * The Properties to check for.
     *
     * \note Does not work with events in dynamic modules.
     */
    std::vector<ValueType> check(FormulaVector const &formulas,
                                 size_t const chunksize = 0);

    /**
     * \return
     * The Probabilities that the top level event fails at the given timepoints.
     */
    std::vector<ValueType> getProbabilitiesAtTimepoints(
        std::vector<ValueType> const &timepoints, size_t const chunksize = 0);

    /**
     * \return
     * The Probability that the top level event fails at the given timebound.
     */
    ValueType getProbabilityAtTimebound(ValueType const timebound) {
        // workDFT will be set in getProbabilitiesAtTimepoints()
        auto const result{getProbabilitiesAtTimepoints({timebound})};
        return result.at(0);
    }

   private:
    std::shared_ptr<storm::storage::DFT<ValueType>> dft;
    std::shared_ptr<storm::storage::DFT<ValueType>> workDFT{};

    /**
     * \return All connected DFTElements of the given element
     */
    static std::vector<DFTElementCPointer> getDecendants(
        DFTElementCPointer const element);

    /**
     * \return whether the given element is static i.e. not dynamic
     * \note This is the case for all static gates and all basic elements.
     */
    static bool isElementStatic(DFTElementCPointer const element);

    struct DfsCounter {
        uint64_t firstVisit{0};
        uint64_t secondVisit{0};
        uint64_t lastVisit{0};

        uint64_t minFirstVisit{0};
        uint64_t maxLastVisit{0};
    };

    struct ElementInfo {
        bool isModule{false};
        bool isStatic{true};
    };

    std::map<ElementId, DfsCounter> dfsCounters{};
    std::map<ElementId, ElementInfo> elementInfos{};

    uint64_t lastDate{};

    /**
     * Populates firstVisit, secondVisit and lastVisit.
     *
     * \note
     * This corresponds to the first depth first search
     * of the LTA/DR algorithm found in doi:10.1109/24.537011.
     */
    void populateDfsCounters();

    /**
     * Internal recursive implementation of populateDfsCounters.
     *
     * \note
     * Should not be called manually.
     */
    void populateDfsCounters(DFTElementCPointer const element);

    /**
     * Populates elementInfos.
     * Frees dfsCounters.
     *
     * \note
     * This corresponds to the second depth first search
     * of the LTA/DR algorithm found in doi:10.1109/24.537011.
     */
    void populateElementInfos();

    /**
     * Internal recursive implementation of populateElementInfos.
     *
     * \note
     * Should not be called manually.
     */
    void populateElementInfos(DFTElementCPointer const element);

    /**
     * Calculate dynamic Modules and replace them with BE's in workDFT
     */
    void replaceDynamicModules(DFTElementCPointer const element,
                               std::vector<ValueType> const &timepoints);
    /**
     * \return DFT with the given element as the root
     */
    std::shared_ptr<storm::storage::DFT<ValueType>> getSubDFT(
        DFTElementCPointer const element);

    /**
     * Update the workdDFT.
     * Replace the given element with a sample BE
     */
    void updateWorkDFT(DFTElementCPointer const element,
                       std::map<ValueType, ValueType> activeSamples);

    /**
     * Analyse the static Module with the given element as the root.
     *
     * \note
     * Updates the workDFT with the calculated probability
     */
    void analyseDynamic(DFTElementCPointer const element,
                        std::vector<ValueType> const &timepoints);

    // don't reinitialize Sylvan BDD
    // temporary
    std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager;
};

}  // namespace modelchecker
}  // namespace storm
