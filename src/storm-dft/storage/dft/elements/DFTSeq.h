#pragma once

#include "DFTRestriction.h"

namespace storm {
    namespace storage {

        /*!
         * Sequence enforcer (SEQ).
         * All children can only fail in order from first to last child.
         * A child which has not failed yet prevents the failure of all children to the right of it.
         */
        template<typename ValueType>
        class DFTSeq : public DFTRestriction<ValueType> {

        public:
            /*!
             * Constructor.
             * @param id Id.
             * @param name Name.
             * @param children Children.
             */
            DFTSeq(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) : DFTRestriction<ValueType>(id, name, children) {
                // Intentionally left empty.
            }

            DFTElementType type() const override {
                return DFTElementType::SEQ;
            }

            bool isSeqEnforcer() const override {
                return true;
            }

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                STORM_LOG_ASSERT(queues.failurePropagationDone(), "Failure propagation not finished.");
                bool childOperationalBefore = false;
                for (auto const& child : this->children()) {
                    if (!state.hasFailed(child->id())) {
                        childOperationalBefore = true;
                    } else if (childOperationalBefore && state.hasFailed(child->id())) {
                        // Child has failed before left sibling.
                        this->fail(state, queues);
                        return;
                    }
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
            }

            bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                // Actually, it doesnt matter what we return here..
                return false;
            }
        };

    }
}
