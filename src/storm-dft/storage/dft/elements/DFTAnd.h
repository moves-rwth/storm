#pragma once

#include "DFTGate.h"

namespace storm {
    namespace storage {

        /*!
         * AND gate.
         * Fails if all children have failed.
         */
        template<typename ValueType>
        class DFTAnd : public DFTGate<ValueType> {

        public:
            /*!
             * Constructor.
             * @param id Id.
             * @param name Name.
             * @param children Children.
             */
            DFTAnd(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) : DFTGate<ValueType>(id, name, children) {
                // Intentionally empty
            }

            DFTElementType type() const override {
                return DFTElementType::AND;
            }

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if (state.isOperational(this->mId)) {
                    for (auto const& child : this->children()) {
                        if (!state.hasFailed(child->id())) {
                            return;
                        }
                    }
                    // All children have failed
                    this->fail(state, queues);
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                STORM_LOG_ASSERT(this->hasFailsafeChild(state), "No failsafe child.");
                if (state.isOperational(this->mId)) {
                    this->failsafe(state, queues);
                    this->childrenDontCare(state, queues);
                }
            }

        };

    }
}
