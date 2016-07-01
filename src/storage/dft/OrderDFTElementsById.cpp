#include "OrderDFTElementsById.h"
#include "DFTElements.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        bool OrderElementsById<ValueType>::operator()(std::shared_ptr<DFTGate<ValueType>> const& a , std::shared_ptr<DFTGate<ValueType>> const& b) const {
            return a->id() < b->id();
        }

        template<typename ValueType>
        bool OrderElementsById<ValueType>::operator ()(const std::shared_ptr<DFTElement<ValueType>>& a, const std::shared_ptr<DFTElement<ValueType>>& b) const {
            return a->id() < b->id();
        }


        template<typename ValueType>
        bool OrderElementsByRank<ValueType>::operator ()(const std::shared_ptr<DFTGate<ValueType>>& a, const std::shared_ptr<DFTGate<ValueType>>& b) const {
            return a->rank() < b->rank();
        }


        // Explicitly instantiate the class.
        template class OrderElementsById<double>;
        template class OrderElementsByRank<double>;

#ifdef STORM_HAVE_CARL
        template class OrderElementsById<RationalFunction>;
        template class OrderElementsByRank<RationalFunction>;
#endif
    }
}
