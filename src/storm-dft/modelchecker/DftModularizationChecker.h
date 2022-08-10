#pragma once

#include <memory>
#include <vector>

#include "storm-dft/storage/DFT.h"
#include "storm-dft/storage/DftModule.h"
#include "storm-dft/storage/SylvanBddManager.h"
#include "storm/logic/Formula.h"

namespace storm::dft {
namespace modelchecker {

/*!
 * DFT analysis via modularization.
 * Dynamic modules are analyzed via model checking and replaced by a single BE capturing the probabilities of the module.
 * The resulting (static) fault tree is then analyzed via BDDs.
 *
 * @note All public functions must make sure that workDFT is set correctly and should assume workDFT to be in an erroneous state.
 */
template<typename ValueType>
class DftModularizationChecker {
   public:
    using ElementId = size_t;
    using DFTElementCPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const>;
    using FormulaCPointer = std::shared_ptr<storm::logic::Formula const>;
    using FormulaVector = std::vector<FormulaCPointer>;

    /*!
     * Initializes and computes all modules.
     * @param dft DFT.
     */
    DftModularizationChecker(std::shared_ptr<storm::dft::storage::DFT<ValueType>> dft);

    /*!
     * Calculate the properties specified by the formulas.
     * @param formulas List of formulas to check.
     * @param chunksize Chunk size used by the BDD checker.
     * @return Results corresponding to the given formulas.
     * @note Does not work with events in dynamic modules.
     */
    std::vector<ValueType> check(FormulaVector const &formulas, size_t chunksize = 0);

    /*!
     * Calculate the probability of failure for the given time points.
     * @param timepoints Time points.
     * @param chunksize Chunk size used by the BDD checker.
     * @return Probabilities that the top level event fails at the given time points.
     */
    std::vector<ValueType> getProbabilitiesAtTimepoints(std::vector<ValueType> const &timepoints, size_t chunksize = 0);

    /*!
     * Calculate the probability of failure for the given time bound.
     * @param timebound Time bound.
     * @return The Probability that the top level event fails at the given time bound.
     */
    ValueType getProbabilityAtTimebound(ValueType const timebound) {
        // workDFT will be set in getProbabilitiesAtTimepoints()
        return getProbabilitiesAtTimepoints({timebound}).at(0);
    }

   private:
    // Complete DFT.
    std::shared_ptr<storm::dft::storage::DFT<ValueType>> dft;
    // Current DFT.
    std::shared_ptr<storm::dft::storage::DFT<ValueType>> workDFT{};

    /*!
     * Calculate results for dynamic modules and replace them with BE's in workDFT.
     * @param element Current DFT element.
     * @param timepoints Time points for which the failure probability should be computed.
     */
    void replaceDynamicModules(DFTElementCPointer const element, std::vector<ValueType> const &timepoints);

    /*!
     * Return DFT with the given element as the root.
     * @param element Root element.
     * @return Sub-DFT with given element as root.
     */
    std::shared_ptr<storm::dft::storage::DFT<ValueType>> getSubDFT(DFTElementCPointer const element);

    /*!
     * Update the workDFT.
     * Replace the given element with a BE for which the failure probabilities correspond to the given sample points.
     * @param element Element to replace.
     * @param activeSamples Sample points for the new BE.
     */
    void updateWorkDFT(DFTElementCPointer const element, std::map<ValueType, ValueType> activeSamples);

    /*!
     * Analyse the dynamic module with the given element as the root.
     * @param element Root element of the module.
     * @param timepoints Time points for which the failure probability of element should be computed.
     * @note Updates the workDFT with the calculated probability.
     */
    void analyseDynamic(DFTElementCPointer const element, std::vector<ValueType> const &timepoints);

    // don't reinitialize Sylvan BDD
    // temporary
    std::shared_ptr<storm::dft::storage::SylvanBddManager> sylvanBddManager;
    // Modules with their top element
    std::map<ElementId, storm::dft::storage::DftModule> modules;
};

}  // namespace modelchecker
}  // namespace storm::dft
