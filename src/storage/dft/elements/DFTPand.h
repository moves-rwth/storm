#pragma once 

#include "DFTGate.h"
namespace storm {
    namespace storage {
          template<typename ValueType>
        class DFTPand : public DFTGate<ValueType> {

        public:
            DFTPand(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state,  DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    bool childOperationalBefore = false;
                    for(auto const& child : this->mChildren)
                    {
                        if(!state.hasFailed(child->id())) {
                            childOperationalBefore = true;
                        } else if(childOperationalBefore && state.hasFailed(child->id())){
                            this->failsafe(state, queues);
                            this->childrenDontCare(state, queues);
                            return;
                        }
                    }
                    if(!childOperationalBefore) {
                        this->fail(state, queues);
                    }
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(this->hasFailsafeChild(state));
                if(state.isOperational(this->mId)) {
                    this->failsafe(state, queues);
                    this->childrenDontCare(state, queues);
                }
            }

            virtual DFTElementType type() const override {
                return DFTElementType::PAND;
            }
            
            std::string typestring() const override {
                return "PAND";
            }
        };
        
        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTPand<ValueType> const& gate) {
             return os << gate.toString();
        }

    }
}