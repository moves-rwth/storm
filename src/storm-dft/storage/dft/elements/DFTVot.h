#pragma once 
#include "DFTGate.h"
namespace storm {
    namespace storage {
        

        template<typename ValueType>
        class DFTVot : public DFTGate<ValueType> {

        private:
            unsigned mThreshold;

        public:
            DFTVot(size_t id, std::string const& name, unsigned threshold, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children), mThreshold(threshold)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    unsigned nrFailedChildren = 0;
                    for(auto const& child : this->mChildren)
                    {
                        if(state.hasFailed(child->id())) {
                            ++nrFailedChildren;
                            if(nrFailedChildren >= mThreshold) 
                            {
                                this->fail(state, queues);
                                return;
                            }
                        }
                    }
                  
                } 
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                STORM_LOG_ASSERT(this->hasFailsafeChild(state), "No failsafe child.");
                if(state.isOperational(this->mId)) {
                    unsigned nrFailsafeChildren = 0;
                    for(auto const& child : this->mChildren)
                    {
                        if(state.isFailsafe(child->id())) {
                            ++nrFailsafeChildren;
                            if(nrFailsafeChildren > this->nrChildren() - mThreshold)
                            {
                                this->failsafe(state, queues);
                                this->childrenDontCare(state, queues);
                                return;
                            }
                        }
                    }
                }
            }
            
            unsigned threshold() const {
                return mThreshold;
            }

            virtual DFTElementType type() const override {
                return DFTElementType::VOT;
            }
            
            std::string typestring() const override{
                return "VOT (" + std::to_string(mThreshold) + ")";
            }
            
            virtual bool isTypeEqualTo(DFTElement<ValueType> const& other) const override {
                if(!DFTElement<ValueType>::isTypeEqualTo(other)) return false;
                DFTVot<ValueType> const& otherVOT = static_cast<DFTVot<ValueType> const&>(other);
                return (mThreshold == otherVOT.mThreshold);
            }
        };

    }
}
