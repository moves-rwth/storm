#include "DFTBE.h"

#include "storm-dft/storage/dft/elements/DFTGate.h"
#include "storm-dft/storage/dft/elements/DFTDependency.h"

namespace storm {
    namespace storage {

        template <typename ValueType>
        void DFTBE<ValueType>::extendSubDft(std::set<size_t>& elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot, bool blockParents, bool sparesAsLeaves) const {
            if (elemsInSubtree.count(this->id())) {
                return;
            }
            DFTElement<ValueType>::extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
            if (elemsInSubtree.empty()) {
                // Parent in the subDFT, i.e., it is *not* a subDFT
                return;
            }
            for (auto const& inDep : ingoingDependencies()) {
                inDep->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                if (elemsInSubtree.empty()) {
                    // Parent in the subDFT, i.e., it is *not* a subDFT
                    return;
                }
            }
        }

        // Explicitly instantiate the class.
        template class DFTBE<double>;
        template class DFTBE<RationalFunction>;

    }
}
