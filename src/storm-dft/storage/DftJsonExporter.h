#pragma once

#include "storm-dft/storage/DFT.h"
#include "storm/adapters/JsonAdapter.h"

namespace storm::dft {
namespace storage {

/**
 * Exports a DFT into the JSON format.
 */
template<typename ValueType>
class DftJsonExporter {
    typedef typename storm::json<double> Json;

    using DFTElementCPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const>;
    using DFTBECPointer = std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>;

   public:
    /*!
     * Export DFT to given file.
     * @param dft DFT.
     * @param filepath File.
     */
    static void toFile(storm::dft::storage::DFT<ValueType> const& dft, std::string const& filepath);

    /*!
     * Export DFT to given stream.
     * @param dft DFT.
     * @param os Output stream.
     */
    static void toStream(storm::dft::storage::DFT<ValueType> const& dft, std::ostream& os);

   private:
    /*!
     * Export DFT into JSON format.
     * @param dft DFT.
     * @return JSON representation of DFT.
     */
    static Json translate(storm::dft::storage::DFT<ValueType> const& dft);

    /*!
     * Export DFT element into JSON format.
     * @param element DFT element.
     * @return JSON representation of DFT element.
     */
    static Json translateElement(DFTElementCPointer element);

    /*!
     * Export BE into JSON format.
     * The translations adds BE specific information to the given nodeData.
     * @param be BE.
     * @param nodeData Existing information on BE (name, type, etc.).
     * @return JSON representation of BE.
     */
    static Json translateBE(DFTBECPointer element, Json nodeData);

    /*!
     * Export parameters of (parametric) DFT into JSON list.
     * @param dft DFT.
     * @return List of parameters. Returns empty list, if DFT is not parametric.
     */
    static Json translateParameters(storm::dft::storage::DFT<ValueType> const& dft);
};

}  // namespace storage
}  // namespace storm::dft
