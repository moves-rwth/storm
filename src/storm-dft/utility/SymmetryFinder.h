#pragma

#include <vector>

#include "storm-dft/storage/DFT.h"
#include "storm-dft/storage/DftSymmetries.h"

namespace storm::dft {

// Forward declaration
namespace storage {
template<typename T>
class DFTColouring;
}  // namespace storage

namespace utility {

template<typename ValueType>
class SymmetryFinder {
   public:
    /*!
     * Find symmetries in the given DFT.
     *
     * @param dft The DFT.
     * @return The symmetries found for the DFT.
     */
    static storm::dft::storage::DftSymmetries findSymmetries(storm::dft::storage::DFT<ValueType> const& dft);

   private:
    static std::map<size_t, size_t> findBijection(storm::dft::storage::DFT<ValueType> const& dft, size_t index1, size_t index2,
                                                  storm::dft::storage::DFTColouring<ValueType> const& colouring, bool sparesAsLeaves);

    static void findSymmetriesHelper(storm::dft::storage::DFT<ValueType> const& dft, std::vector<size_t> const& candidates,
                                     storm::dft::storage::DFTColouring<ValueType> const& colouring, std::map<size_t, std::vector<std::vector<size_t>>>& result);

    static std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>> getInfluencedIds(
        storm::dft::storage::DFT<ValueType> const& dft, size_t index);

    static bool hasSeqRestriction(std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const> elem);
};

}  // namespace utility
}  // namespace storm::dft
