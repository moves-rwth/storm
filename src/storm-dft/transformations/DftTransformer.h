#pragma once

#include "storm-dft/storage/DFT.h"

namespace storm::dft {
namespace transformations {

/*!
 * Transformer for operations on DFT.
 */
template<typename ValueType>
class DftTransformer {
   public:
    /*!
     * Check whether at most one constant failed BE is present in the DFT.
     * @param dft DFT.
     * @return True iff at most one constant failed BE exists.
     */
    static bool hasUniqueFailedBE(storm::dft::storage::DFT<ValueType> const &dft);

    /*!
     * Introduce unique BE which is always failed (instead of multiple ones).
     * The transformation introduces an FDEP from the new unique BE to the previously failed BEs.
     * Note that this operation can therefore introduce non-binary dependencies.
     *
     * @param dft Original DFT.
     * @return DFT with a unique always failed BE.
     */
    static std::shared_ptr<storm::dft::storage::DFT<ValueType>> transformUniqueFailedBE(storm::dft::storage::DFT<ValueType> const &dft);

    /*!
     * Check whether the DFT has dependencies with multiple dependent events.
     * @param dft DFT.
     * @return True iff at least one dependency has more than one dependent event.
     */
    static bool hasNonBinaryDependency(storm::dft::storage::DFT<ValueType> const &dft);

    /*!
     * Introduce binary dependencies (with only one dependent event) instead of dependencies with multiple dependent events.
     *
     * @param dft Original DFT.
     * @return DFT in which all dependencies are binary.
     */
    static std::shared_ptr<storm::dft::storage::DFT<ValueType>> transformBinaryDependencies(storm::dft::storage::DFT<ValueType> const &dft);

    /*!
     * Check whether the DFT contains only BEs with exponential distributions (or constant failed/failsafe BEs).
     * @param dft DFT.
     * @return True iff all BEs have exponential (or constant failed/failsafe) distributions.
     */
    static bool hasOnlyExponentialDistributions(storm::dft::storage::DFT<ValueType> const &dft);

    /*!
     * Replace certain BE distributions by DFT constructs using only exponential distributions to make them amenable for Markovian analysis.
     * - constant probability distributions are replaced using PDEPs
     * - Erlang distributions are replaced using SEQs
     *
     * @param dft Original DFT.
     * @return DFT in which certain BE distributions are replaced by DFT constructs.
     */
    static std::shared_ptr<storm::dft::storage::DFT<ValueType>> transformExponentialDistributions(storm::dft::storage::DFT<ValueType> const &dft);
};

}  // namespace transformations
}  // namespace storm::dft
