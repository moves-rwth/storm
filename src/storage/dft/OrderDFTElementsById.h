#ifndef ORDERDFTELEMENTS_H
#define	ORDERDFTELEMENTS_H

#include <memory>

namespace storm {
    namespace storage {
        class DFTGate;
        class DFTElement;
        
        struct OrderElementsById {
            bool operator()(std::shared_ptr<DFTGate> const& a , std::shared_ptr<DFTGate> const& b) const;
            bool operator()(std::shared_ptr<DFTElement> const& a, std::shared_ptr<DFTElement> const& b) const;
        };
        
        struct OrderElementsByRank {
            bool operator()(std::shared_ptr<DFTGate> const& a, std::shared_ptr<DFTGate> const& b) const;
        };

    }
}

#endif	/* ORDERDFTELEMENTSBYID_H */

