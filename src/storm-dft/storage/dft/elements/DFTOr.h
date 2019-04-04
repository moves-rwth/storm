#pragma once

#include "DFTGate.h"

namespace storm {
    namespace storage {

        /*!
         * OR gate.
         * Fails if at least one child has failed.
         */
        template<typename ValueType>
        class DFTOr : public DFTGate<ValueType> {

        public:
            /*!
             * Constructor.
             * @param id Id.
             * @param name Name.
             * @param children Children.
             */
            DFTOr(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) : DFTGate<ValueType>(id, name, children) {
                // Intentionally empty
            }

            DFTElementType type() const override {
                return DFTElementType::OR;
            }

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                STORM_LOG_ASSERT(this->hasFailedChild(state), "No failed child.");
                if (state.isOperational(this->mId)) {
                    this->fail(state, queues);
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                for (auto const& child : this->children()) {
                    if (!state.isFailsafe(child->id())) {
                        return;
                    }
                }
                // All chidren are failsafe
                this->failsafe(state, queues);
            }

        };

    }
}
       
