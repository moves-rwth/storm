#pragma once

#include "DFTElement.h"

namespace storm {
    namespace storage {

        /*!
         * Abstract base class for basic elements (BEs) in DFTs.
         */
        template<typename ValueType>
        class DFTBE : public DFTElement<ValueType> {

        public:
            /*!
             * Constructor.
             * @param id Id.
             * @param name Name.
             */
            DFTBE(size_t id, std::string const& name) : DFTElement<ValueType>(id, name) {
                // Intentionally empty
            }

            size_t nrChildren() const override {
                return 0;
            }

            /*!
             * Return whether the BE can fail.
             * @return True iff BE is not failsafe.
             */
            virtual bool canFail() const = 0;

            /*!
             * Add dependency which can trigger this BE.
             * @param dependency Ingoing dependency.
             */
            void addIngoingDependency(std::shared_ptr<DFTDependency<ValueType>> const& dependency) {
                // TODO write this assertion for n-ary dependencies, probably by adding a method to the dependencies to support this.
                //STORM_LOG_ASSERT(e->dependentEvent()->id() == this->id(), "Ids do not match.");
                STORM_LOG_ASSERT(std::find(mIngoingDependencies.begin(), mIngoingDependencies.end(), dependency) == mIngoingDependencies.end(),
                                 "Ingoing Dependency " << dependency << " already present.");
                mIngoingDependencies.push_back(dependency);
            }

            /*!
             * Return whether the BE has ingoing dependencies.
             * @return True iff BE can be triggered by dependencies.
             */
            bool hasIngoingDependencies() const {
                return !mIngoingDependencies.empty();
            }

            /*!
             * Return ingoing dependencies.
             * @return List of dependencies which can trigger this BE.
             */
            std::vector<std::shared_ptr<DFTDependency<ValueType>>> const& ingoingDependencies() const {
                return mIngoingDependencies;
            }

            bool isBasicElement() const override {
                return true;
            }

            void extendSubDft(std::set<size_t>& elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot, bool blockParents, bool sparesAsLeaves) const override {
                if (elemsInSubtree.count(this->id())) {
                    return;
                }
                DFTElement<ValueType>::extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                if (elemsInSubtree.empty()) {
                    // Parent in the subdft, ie it is *not* a subdft
                    return;
                }
                for (auto const& incDep : ingoingDependencies()) {
                    incDep->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                    if (elemsInSubtree.empty()) {
                        // Parent in the subdft, ie it is *not* a subdft
                        return;
                    }
                }
            }

            bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues <ValueType>& queues) const override {
                if (DFTElement<ValueType>::checkDontCareAnymore(state, queues)) {
                    state.beNoLongerFailable(this->id());
                    return true;
                }
                return false;
            }

        private:
            std::vector<std::shared_ptr<DFTDependency<ValueType>>> mIngoingDependencies;

        };

    }
}
