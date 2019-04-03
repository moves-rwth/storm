#pragma once 

#include "DFTGate.h"
namespace storm {
    namespace storage {
        template<typename ValueType>
        class DFTOr : public DFTGate<ValueType> {

        public:
            DFTOr(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                STORM_LOG_ASSERT(this->hasFailedChild(state), "No failed child.");
                if(state.isOperational(this->mId)) {
                    this->fail(state, queues);
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                 for(auto const& child : this->mChildren) {
                     if(!state.isFailsafe(child->id())) {
                         return;
                     }
                 }
                 this->failsafe(state, queues);
            }

            virtual DFTElementType type() const override {
                return DFTElementType::OR;
            }
            
            std::string typestring() const override {
                return "OR";
            }
        };

    }
}
       
