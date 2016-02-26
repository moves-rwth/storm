#pragma once 


#include "DFTGate.h"
namespace storm {
    namespace storage {
        

        template<typename ValueType>
        class DFTSpare : public DFTGate<ValueType> {

        public:
            DFTSpare(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}
            
            std::string typestring() const override {
                return "SPARE";
            }

            virtual DFTElementType type() const override {
                return DFTElementType::SPARE;
            }

            bool isSpareGate() const override {
                return true;
            }
            
            void fail(DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
                DFTGate<ValueType>::fail(state, queues);
                state.finalizeUses(this->mId);
            }
            
            void failsafe(DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
                DFTGate<ValueType>::failsafe(state, queues);
                state.finalizeUses(this->mId);
            }
            
            bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if (DFTGate<ValueType>::checkDontCareAnymore(state, queues)) {
                    state.finalizeUses(this->mId);
                    return true;
                }
                return false;
            }
            
            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    size_t uses = state.uses(this->mId);
                    if(!state.isOperational(uses)) {
                        bool claimingSuccessful = state.claimNew(this->mId, uses, this->mChildren);
                        if(!claimingSuccessful) {
                            this->fail(state, queues);
                        }
                    }                  
                } 
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    if(state.isFailsafe(state.uses(this->mId))) {
                        this->failsafe(state, queues);
                        this->childrenDontCare(state, queues);
                    }
                }
            }

        };
    }
}