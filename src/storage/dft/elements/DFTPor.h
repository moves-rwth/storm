#pragma once 

#include "DFTGate.h"
namespace storm {
    namespace storage {
        template<typename ValueType>
        class DFTPor : public DFTGate<ValueType> {
        public:
            DFTPor(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                 assert(false);
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(false);
            }

            virtual DFTElementType type() const override {
                return DFTElementType::POR;
            }
            
            std::string  typestring() const override {
                return "POR";
            }
        };

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTPor<ValueType> const& gate) {
             return os << gate.toString();
        }

    }
}