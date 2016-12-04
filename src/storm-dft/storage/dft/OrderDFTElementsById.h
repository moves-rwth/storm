#pragma  once
#include <memory>

namespace storm {
    namespace storage {

        template<typename ValueType>
        class DFTGate;
        template<typename ValueType>
        class DFTElement;

        template<typename ValueType>
        struct OrderElementsById {
            bool operator()(std::shared_ptr<DFTGate<ValueType>> const& a , std::shared_ptr<DFTGate<ValueType>> const& b) const;

            bool operator()(std::shared_ptr<DFTElement<ValueType>> const& a, std::shared_ptr<DFTElement<ValueType>> const& b) const;
        };

        template<typename ValueType>
        struct OrderElementsByRank {
            bool operator()(std::shared_ptr<DFTGate<ValueType>> const& a, std::shared_ptr<DFTGate<ValueType>> const& b) const;
        };

    }
}
