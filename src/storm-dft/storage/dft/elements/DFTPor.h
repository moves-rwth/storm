#pragma once 

#include "DFTGate.h"
namespace storm {
    namespace storage {
        template<typename ValueType>
        class DFTPor : public DFTGate<ValueType> {
        public:
            DFTPor(size_t id, std::string const& name, bool inclusive, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children),
                    inclusive(inclusive)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(inclusive);
                if(state.isOperational(this->mId)) {
                    if (state.hasFailed(this->mChildren.front()->id())) {
                        // First child has failed before others
                        this->fail(state, queues);
                    } else {
                        for (size_t i = 1; i < this->nrChildren(); ++i) {
                            if (state.hasFailed(this->mChildren[i]->id())) {
                                // Child has failed before first child
                                this->failsafe(state, queues);
                                this->childrenDontCare(state, queues);
                            }
                        }
                    }
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(inclusive);
                if (state.isFailsafe(this->mChildren.front()->id())) {
                    this->failsafe(state, queues);
                    this->childrenDontCare(state, queues);
                }
            }

            virtual DFTElementType type() const override {
                return DFTElementType::POR;
            }
            
            std::string  typestring() const override {
                return inclusive ? "POR-inc" : "POR-ex";
            }
            
            bool isInclusive() const {
                return inclusive;
            }
        protected:
            bool inclusive;
        };

    }
}
