#pragma  once

#include "storm/utility/macros.h"

#include "storm-dft/storage/dft/DFT.h"

// JSON parser
#include "json.hpp"
namespace modernjson {
    using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, double, std::allocator>;
}

namespace storm {
    namespace storage {

        /**
         * Exports a DFT into the JSON format for visualizing it.
         */
        template<typename ValueType>
        class DftJsonExporter {

            using DFTElementPointer = std::shared_ptr<DFTElement<ValueType>>;
            using DFTElementCPointer = std::shared_ptr<DFTElement<ValueType> const>;
            using DFTGatePointer = std::shared_ptr<DFTGate<ValueType>>;

        public:

            static void toFile(storm::storage::DFT<ValueType> const& dft, std::string const& filepath);

            static void toStream(storm::storage::DFT<ValueType> const& dft, std::ostream& os);

        private:

            static modernjson::json translate(storm::storage::DFT<ValueType> const& dft);

            static modernjson::json translateNode(DFTElementCPointer const& element);

        };
    }
}
