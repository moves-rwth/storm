#include "OrderDFTElementsById.h"
#include "DFTElements.h"

namespace storm {
    namespace storage {
        bool OrderElementsById::operator()(std::shared_ptr<DFTGate> const& a , std::shared_ptr<DFTGate> const& b) const {
            return a->id() < b->id();
        }
        bool OrderElementsById::operator ()(const std::shared_ptr<DFTElement>& a, const std::shared_ptr<DFTElement>& b) const {
            return a->id() < b->id();
        }
        
        
        bool OrderElementsByRank::operator ()(const std::shared_ptr<DFTGate>& a, const std::shared_ptr<DFTGate>& b) const {
            return a->rank() < b->rank();
        }
    }
}
