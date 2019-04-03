#pragma once 
#include "DFTGate.h"

namespace storm {
    namespace storage {
        template<typename ValueType>
        class DFTAnd : public DFTGate<ValueType> {
            
        public:
            DFTAnd(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    for(auto const& child : this->mChildren)
                    {
                        if(!state.hasFailed(child->id())) {
                            return;
                        }
                    }
                    this->fail(state, queues);
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                STORM_LOG_ASSERT(this->hasFailsafeChild(state), "No failsafe child.");
                if(state.isOperational(this->mId)) {
                    this->failsafe(state, queues);
                    this->childrenDontCare(state, queues);
                }
            }

            virtual DFTElementType type() const override {
                return DFTElementType::AND;
            }
            
            std::string typestring() const override {
                return "AND";
            }
        };
              
    }
}
