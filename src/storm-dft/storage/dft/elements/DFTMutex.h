#pragma once

#include "DFTRestriction.h"

namespace storm {
    namespace storage {

        /*!
         * Mutex restriction (MUTEX).
         * Only one of the children can fail.
         * A child which has failed prevents the failure of all other children.
         */
        template<typename ValueType>
        class DFTMutex : public DFTRestriction<ValueType> {

        public:
            /*!
             * Constructor.
             * @param id Id.
             * @param name Name.
             * @param children Children.
             */
            DFTMutex(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) : DFTRestriction<ValueType>(id, name, children) {
                // Intentionally left empty.
            }

            DFTElementType type() const override {
                return DFTElementType::MUTEX;
            }

            bool isMutex() const override {
                return true;
            }

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                STORM_LOG_ASSERT(queues.failurePropagationDone(), "Failure propagation not finished.");
                bool childFailed = false;
                for (auto const& child : this->children()) {
                    if (state.hasFailed(child->id())) {
                        if (childFailed) {
                            // Two children have failed
                            this->fail(state, queues);
                            return;
                        } else {
                            childFailed = true;
                        }
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
