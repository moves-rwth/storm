#pragma once

#include "DFTElement.h"
namespace storm {
    namespace storage {
    template<typename ValueType>
        class DFTGate : public DFTElement<ValueType> {

            using DFTElementPointer = std::shared_ptr<DFTElement<ValueType>>;
            using DFTElementVector = std::vector<DFTElementPointer>;

        protected:
            DFTElementVector mChildren;

        public:
            DFTGate(size_t id, std::string const& name, DFTElementVector const& children) :
                    DFTElement<ValueType>(id, name), mChildren(children)
            {}
            
            virtual ~DFTGate() {}
            
            void pushBackChild(DFTElementPointer elem) {
                return mChildren.push_back(elem);
            }

            size_t nrChildren() const override {
                return mChildren.size();
            }

            DFTElementVector const& children() const {
                return mChildren;
            }
            
            virtual bool isGate() const override {
                return true;
            }
            
            bool isDynamicGate() const {
                return !isStaticGateType(this->type());
            }
            
            
            virtual std::string typestring() const {
                return storm::storage::toString(this->type());
            }

            virtual void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const = 0;

            virtual void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const = 0;
            
            virtual void extendSpareModule(std::set<size_t>& elementsInSpareModule) const override {
                if (!this->isSpareGate()) {
                    DFTElement<ValueType>::extendSpareModule(elementsInSpareModule);
                    for (auto const& child : mChildren) {
                        if (elementsInSpareModule.count(child->id()) == 0) {
                            elementsInSpareModule.insert(child->id());
                            child->extendSpareModule(elementsInSpareModule);
                        }
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

            virtual std::vector<size_t> independentSubDft(bool blockParents, bool sparesAsLeaves = false) const override {
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
                if(DFTElement<ValueType>::checkDontCareAnymore(state, queues)) {
                    childrenDontCare(state, queues);
                    return true;
                }
                return false;
            }


        protected:

            void fail(DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
                for(std::shared_ptr<DFTGate> parent : this->mParents) {
                    if(state.isOperational(parent->id())) {
                        queues.propagateFailure(parent);
                    }
                }
                for(std::shared_ptr<DFTRestriction<ValueType>> restr : this->mRestrictions) {
                    queues.checkRestrictionLater(restr);
                }
                state.setFailed(this->mId);
                this->childrenDontCare(state, queues);
            }

            void failsafe(DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
                for(std::shared_ptr<DFTGate> parent : this->mParents) {
                    if(state.isOperational(parent->id())) {
                        queues.propagateFailsafe(parent);
                    }
                }
                state.setFailsafe(this->mId);
                this->childrenDontCare(state, queues);
            }
            
            void childrenDontCare(DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
                queues.propagateDontCare(mChildren);
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
    }
}
