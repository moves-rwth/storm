#pragma once

#include "../DFTElements.h"

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


            virtual std::string typestring() const = 0;

            virtual void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const = 0;

            virtual void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const = 0;

            virtual void extendSpareModule(std::set<size_t>& elementsInSpareModule) const override {
                DFTElement<ValueType>::extendSpareModule(elementsInSpareModule);
                for(auto const& child : mChildren) {
                    if(elementsInSpareModule.count(child->id()) == 0) {
                        elementsInSpareModule.insert(child->id());
                        child->extendSpareModule(elementsInSpareModule);
                    }
                }
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

            virtual std::vector<size_t> independentSubDft() const override {
                auto prelRes = DFTElement<ValueType>::independentSubDft();
                if(prelRes.empty()) {
                    // No elements (especially not this->id) in the prelimanry result, so we know already that it is not a subdft.
                    return prelRes;
                }
                std::set<size_t> unit(prelRes.begin(), prelRes.end());
                std::vector<size_t> pids = this->parentIds();
                for(auto const& child : mChildren) {
                    child->extendSubDft(unit, pids);
                    if(unit.empty()) {
                        // Parent in the subdft, ie it is *not* a subdft
                        break;
                    }
                }
                return std::vector<size_t>(unit.begin(), unit.end());
            }

            virtual void extendSubDft(std::set<size_t>& elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot) const override {
                if(elemsInSubtree.count(this->id()) > 0) return;
                DFTElement<ValueType>::extendSubDft(elemsInSubtree, parentsOfSubRoot);
                if(elemsInSubtree.empty()) {
                    // Parent in the subdft, ie it is *not* a subdft
                    return;
                }
                for(auto const& child : mChildren) {
                    child->extendSubDft(elemsInSubtree, parentsOfSubRoot);
                    if(elemsInSubtree.empty()) {
                        // Parent in the subdft, ie it is *not* a subdft
                        return;
                    }
                }
            }


            virtual std::string toString() const override {
                std::stringstream stream;
                stream << "{" << this->name() << "} " << typestring() << "( ";
                typename DFTElementVector::const_iterator it = mChildren.begin();
                stream << (*it)->name();
                ++it;
                while(it != mChildren.end()) {
                    stream <<  ", " << (*it)->name();
                    ++it;
                }
                stream << ")";
                return stream.str();
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




        };

        template<typename ValueType>
        class DFTSeq : public DFTRestriction<ValueType> {


        public:
            DFTSeq(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTRestriction<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(queues.failurePropagationDone());
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
            
            bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                
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