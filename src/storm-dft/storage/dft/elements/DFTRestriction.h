#pragma once

#include "DFTElement.h"
namespace storm {
    namespace storage {
        template<typename ValueType>
        class DFTRestriction : public DFTElement<ValueType> {
            using DFTElementPointer = std::shared_ptr<DFTElement<ValueType>>;
            using DFTElementVector = std::vector<DFTElementPointer>;
        protected:
            DFTElementVector mChildren;

        public:
            DFTRestriction(size_t id, std::string const& name, DFTElementVector const& children) :
            DFTElement<ValueType>(id, name), mChildren(children)
                    {}

            virtual ~DFTRestriction() {}

            void pushBackChild(DFTElementPointer elem) {
                return mChildren.push_back(elem);
            }

            size_t nrChildren() const override {
                return mChildren.size();
            }

            DFTElementVector const& children() const {
                return mChildren;
            }

            virtual bool isRestriction() const override {
                return true;
            }
            
            virtual bool isSeqEnforcer() const {
                return false;
            }
            
            bool allChildrenBEs() const {
                for(auto const& elem : mChildren) {
                    if (!elem->isBasicElement()) {
                        return false;
                    }
                }
                return true;
            }


            virtual std::string typestring() const = 0;

            virtual void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const = 0;

            virtual void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const = 0;

            virtual void extendSpareModule(std::set<size_t>& elementsInSpareModule) const override {
                // Do nothing
            }

            virtual std::vector<size_t> independentUnit() const override {
                std::set<size_t> unit = {this->mId};
                for(auto const& child : mChildren) {
                    child->extendUnit(unit);
                }
                return std::vector<size_t>(unit.begin(), unit.end());
            }

            virtual void extendUnit(std::set<size_t>& unit) const override {
                DFTElement<ValueType>::extendUnit(unit);
                for(auto const& child : mChildren) {
                    child->extendUnit(unit);
                }
            }

            virtual std::vector<size_t> independentSubDft(bool blockParents, bool sparesAsLeaves) const override {
                auto prelRes = DFTElement<ValueType>::independentSubDft(blockParents);
                if(prelRes.empty()) {
                    // No elements (especially not this->id) in the prelimanry result, so we know already that it is not a subdft.
                    return prelRes;
                }
                std::set<size_t> unit(prelRes.begin(), prelRes.end());
                std::vector<size_t> pids = this->parentIds();
                for(auto const& child : mChildren) {
                    child->extendSubDft(unit, pids, blockParents, sparesAsLeaves);
                    if(unit.empty()) {
                        // Parent in the subdft, ie it is *not* a subdft
                        break;
                    }
                }
                return std::vector<size_t>(unit.begin(), unit.end());
            }

            virtual void extendSubDft(std::set<size_t>& elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot, bool blockParents, bool sparesAsLeaves) const override {
                if(elemsInSubtree.count(this->id()) > 0) return;
                DFTElement<ValueType>::extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                if(elemsInSubtree.empty()) {
                    // Parent in the subdft, ie it is *not* a subdft
                    return;
                }
                for(auto const& child : mChildren) {
                    child->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                    if(elemsInSubtree.empty()) {
                        // Parent in the subdft, ie it is *not* a subdft
                        return;
                    }
                }
            }

            virtual bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                return false;
            }


        protected:

            void fail(DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
                state.markAsInvalid();
            }

            void failsafe(DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {

            }

            bool hasFailsafeChild(DFTState<ValueType>& state) const {
                for(auto const& child : mChildren) {
                    if(state.isFailsafe(child->id()))
                    {
                        return true;
                    }
                }
                return false;
            }

            bool hasFailedChild(DFTState<ValueType>& state) const {
                for(auto const& child : mChildren) {
                    if(state.hasFailed(child->id())) {
                        return true;
                    }
                }
                return false;
            }

            virtual std::string toString() const override {
                std::stringstream stream;
                stream << "{" << this->name() << "} " << this->typestring() << "( ";
                auto it = this->children().begin();
                stream << (*it)->name();
                ++it;
                while(it != this->children().end()) {
                    stream <<  ", " << (*it)->name();
                    ++it;
                }
                stream << ")";
                return stream.str();
            }


        };


        template<typename ValueType>
        class DFTSeq : public DFTRestriction<ValueType> {


        public:
            DFTSeq(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTRestriction<ValueType>(id, name, children)
            {}

            virtual bool isSeqEnforcer() const override {
                return true;
            }


            
            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                STORM_LOG_ASSERT(queues.failurePropagationDone(), "Failure propagation not finished.");
                bool childOperationalBefore = false;
                for(auto const& child : this->mChildren)
                {
                    if(!state.hasFailed(child->id())) {
                        childOperationalBefore = true;
                    } else if(childOperationalBefore && state.hasFailed(child->id())){
                        this->fail(state, queues);
                        return;
                    }
                }

            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                

            }
            
            virtual bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                // Actually, it doesnt matter what we return here..
                return false;
            }

            virtual DFTElementType type() const override {
                return DFTElementType::SEQ;
            }

            std::string typestring() const override {
                return "SEQ";
            }
        };

    }
}
