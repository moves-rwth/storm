#pragma once

#include "storm-dft/storage/DFT.h"
#include "storm-dft/storage/DFTState.h"

namespace storm::dft {
namespace simulator {

/*!
 * Abstract class for importance functions.
 */
template<typename ValueType>
class ImportanceFunction {
   protected:
    using DFTStatePointer = std::shared_ptr<storm::dft::storage::DFTState<ValueType>>;

   public:
    /*!
     * Constructor.
     *
     * @param dft DFT.
     */
    ImportanceFunction(storm::dft::storage::DFT<ValueType> const& dft);

    /*!
     * Get the importance for a given state.
     * @param state DFT state.
     * @return Importance value.
     */
    virtual double getImportance(DFTStatePointer state) const = 0;

    /*!
     * Get the lower and upper bounds of possible importance values computed by this importance function.
     *
     * @return Lower, upper bound of possible importance values.
     */
    virtual std::pair<double, double> getImportanceRange() const = 0;

   protected:
    // The DFT used for the generation of next states.
    storm::dft::storage::DFT<ValueType> const& dft;
};

/*!
 * Importance function based on counting the number of currently failed BEs.
 */
template<typename ValueType>
class BECountImportanceFunction : public ImportanceFunction<ValueType> {
   public:
    /*!
     * Constructor.
     *
     * @param dft DFT.
     */
    BECountImportanceFunction(storm::dft::storage::DFT<ValueType> const& dft);

    /*!
     * Get the importance for a given state.
     * @param state DFT state.
     * @return Importance value.
     */
    double getImportance(typename ImportanceFunction<ValueType>::DFTStatePointer state) const override;

    /*!
     * Get the lower and upper bounds of possible importance values computed by this importance function.
     *
     * @return Lower, upper bound of possible importance values.
     */
    std::pair<double, double> getImportanceRange() const override;
};

}  // namespace simulator
}  // namespace storm::dft
