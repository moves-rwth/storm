#ifndef DFTELEMENTS_H
#define	DFTELEMENTS_H

#include <set>
#include <vector>
#include <memory>
#include <string>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <functional>

#include "DFTElementType.h"
#include "DFTState.h"
#include "DFTStateSpaceGenerationQueues.h"
#include "src/utility/constants.h"
#include "src/adapters/CarlAdapter.h"

using std::size_t;

namespace storm {
    namespace storage {

        template<typename ValueType>
        class DFTGate;
        
        template<typename ValueType>
        class DFTDependency;

        template<typename ValueType>
        class DFTElement {

            using DFTGatePointer = std::shared_ptr<DFTGate<ValueType>>;
            using DFTGateVector = std::vector<DFTGatePointer>;
            using DFTDependencyPointer = std::shared_ptr<DFTDependency<ValueType>>;
            using DFTDependencyVector = std::vector<DFTDependencyPointer>;

        protected:
            size_t mId;
            std::string mName;
            size_t mRank = -1;
            DFTGateVector mParents;
            DFTDependencyVector mOutgoingDependencies;

        public:
            DFTElement(size_t id, std::string const& name) :
                    mId(id), mName(name)
            {}

            virtual ~DFTElement() {}

            /**
             * Returns the id
             */
            virtual size_t id() const {
                return mId;
            }

            virtual DFTElementType type() const = 0;

            virtual void setRank(size_t rank) {
                mRank = rank;
            }
            
            virtual size_t rank() const {
                return mRank;
            }
            
            virtual bool isConstant() const {
                return false;
            }
            
            virtual bool isGate() const {
                return false;
            }

            /**
             *  Returns true if the element is a BE
             */
            virtual bool isBasicElement() const {
                return false;
            }
            
            virtual bool isColdBasicElement() const {
                return false;
            }

            /**
             * Returns true if the element is a spare gate
             */
            virtual bool isSpareGate() const {
                return false;
            }

            virtual bool isDependency() const {
                return false;
            }

            virtual void setId(size_t newId) {
                mId = newId;
            }

            /**
             * Returns the name
             */
            virtual std::string const& name() const {
                return mName;
            }
            
            bool addParent(DFTGatePointer const& e) {
                if(std::find(mParents.begin(), mParents.end(), e) != mParents.end()) {
                    return false;
                }
                else 
                {
                    mParents.push_back(e);
                    return true;
                }
            }

             bool hasOnlyStaticParents() const {
                for(auto const& parent : mParents) {
                    if(!isStaticGateType(parent->type())) {
                        return false;
                    }
                }
                return true;
            }

            
            bool hasParents() const {
                return !mParents.empty();
            }
            
            size_t nrParents() const {
                return mParents.size();
            }

            DFTGateVector const& parents() const {
                return mParents;
            }

            std::vector<size_t> parentIds() const {
                std::vector<size_t> res;
                for(auto parent : parents()) {
                    res.push_back(parent->id());
                }
                return res;
            }
            
            bool addOutgoingDependency(DFTDependencyPointer const& e) {
                assert(e->triggerEvent()->id() == this->id());
                if(std::find(mOutgoingDependencies.begin(), mOutgoingDependencies.end(), e) != mOutgoingDependencies.end()) {
                    return false;
                }
                else
                {
                    mOutgoingDependencies.push_back(e);
                    return true;
                }
            }
            
            bool hasOutgoingDependencies() const {
                return !mOutgoingDependencies.empty();
            }
            
            size_t nrOutgoingDependencies() const {
                return mOutgoingDependencies.size();
            }
            
            DFTDependencyVector const& outgoingDependencies() const {
                return mOutgoingDependencies;
            }
            
            virtual void extendSpareModule(std::set<size_t>& elementsInModule) const;
            
            virtual size_t nrChildren() const = 0;

            virtual std::string toString() const = 0;

            virtual bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const;

            /**
             *  Computes the independent unit of this element, that is, all elements which are direct or indirect successors of an element.
             */
            virtual std::vector<size_t> independentUnit() const;

            /**
             *  Helper to independent unit computation
             *  @see independentUnit
             */
            virtual void extendUnit(std::set<size_t>& unit) const;

            /**
             *  Computes independent subtrees starting with this element (this), that is, all elements (x) which are connected to either
             *  - one of the children of the element,
             *  - a propabilisistic dependency
             *  such that there exists a  path from x to a child of this does not go through this.
             */
            virtual std::vector<size_t> independentSubDft() const;
            /**
             * Helper to the independent subtree computation
             * @see independentSubDft
             */
            virtual void extendSubDft(std::set<size_t> elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot) const;

            virtual bool isTypeEqualTo(DFTElement<ValueType> const& other) const {
                return type() == other.type();
            }


        protected:
           // virtual bool checkIsomorphicSubDftHelper(DFTElement const& otherElem, std::vector<std::pair<DFTElement const&, DFTElement const&>>& mapping, std::vector<DFTElement const&> const& order ) const = 0;

        };




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

            virtual void extendSubDft(std::set<size_t> elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot) const override {
                DFTElement<ValueType>::extendSubDft(elemsInSubtree, parentsOfSubRoot);
                if(!elemsInSubtree.empty()) {
                    // Parent in the subdft, ie it is *not* a subdft
                    return;
                }
                for(auto const& child : mChildren) {
                    child->extendSubDft(elemsInSubtree, parentsOfSubRoot);
                    if(elemsInSubtree.empty()) {
                        // Parent in the subdft, ie it is *not* a subdft
                        break;
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
                state.setFailed(this->mId);
            }

            void failsafe(DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
                for(std::shared_ptr<DFTGate> parent : this->mParents) {
                    if(state.isOperational(parent->id())) {
                        queues.propagateFailsafe(parent);
                    }
                }
                state.setFailsafe(this->mId);
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
        
       

        template<typename ValueType>
        class DFTBE : public DFTElement<ValueType> {
            
            using DFTDependencyPointer = std::shared_ptr<DFTDependency<ValueType>>;
            using DFTDependencyVector = std::vector<DFTDependencyPointer>;

        protected:
            ValueType mActiveFailureRate;
            ValueType mPassiveFailureRate;
            DFTDependencyVector mIngoingDependencies;

        public:
            DFTBE(size_t id, std::string const& name, ValueType failureRate, ValueType dormancyFactor) :
                    DFTElement<ValueType>(id, name), mActiveFailureRate(failureRate), mPassiveFailureRate(dormancyFactor * failureRate)
            {}

            DFTElementType type() const override {
                return DFTElementType::BE;
            }

            virtual size_t nrChildren() const override {
                return 0;
            }

            ValueType const& activeFailureRate() const {
                return mActiveFailureRate;
            }

            ValueType const& passiveFailureRate() const {
                return mPassiveFailureRate;
            }
            
            bool addIngoingDependency(DFTDependencyPointer const& e) {
                assert(e->dependentEvent()->id() == this->id());
                if(std::find(mIngoingDependencies.begin(), mIngoingDependencies.end(), e) != mIngoingDependencies.end()) {
                    return false;
                }
                else
                {
                    mIngoingDependencies.push_back(e);
                    return true;
                }
            }
            
            bool hasIngoingDependencies() const {
                return !mIngoingDependencies.empty();
            }
            
            size_t nrIngoingDependencies() const {
                return mIngoingDependencies.size();
            }
            
            DFTDependencyVector const& getIngoingDependencies() const {
                return mIngoingDependencies;
            }
        
            std::string toString() const override {
                std::stringstream stream;
                stream << *this;
                return stream.str();
            }
            
            bool isBasicElement() const override{
                return true;
            }
            
            bool isColdBasicElement() const override{
                return storm::utility::isZero(mPassiveFailureRate);
            }
            
            bool isTypeEqualTo(DFTElement<ValueType> const& other) const override {
                if(!DFTElement<ValueType>::isTypeEqualTo(other)) return false;
                DFTBE<ValueType> const&  otherBE = static_cast<DFTBE<ValueType> const&>(other);
                return (mActiveFailureRate == otherBE.mActiveFailureRate) && (mPassiveFailureRate == otherBE.mPassiveFailureRate);
            }
            
            virtual bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override;
        };

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTBE<ValueType> const& be) {
            return os << "{" << be.name() << "} BE(" << be.activeFailureRate() << ", " << be.passiveFailureRate() << ")";
        }



        template<typename ValueType>
        class DFTConst : public DFTElement<ValueType> {

            bool mFailed;

        public:
            DFTConst(size_t id, std::string const& name, bool failed) :
                    DFTElement<ValueType>(id, name), mFailed(failed)
            {}

            DFTElementType type() const override {
                if(mFailed) {
                    return DFTElementType::CONSTF;
                } else {
                    return DFTElementType::CONSTS;
                }
            }


            bool failed() const {
                return mFailed;
            }
            
            virtual bool isConstant() const {
                return true;
            }
            
            virtual size_t nrChildren() const override {
                return 0;
            }
            
            bool isTypeEqualTo(DFTElement<ValueType> const& other) const override {
                if(!DFTElement<ValueType>::isTypeEqualTo(other)) return false;
                DFTConst<ValueType> const& otherCNST = static_cast<DFTConst<ValueType> const&>(other);
                return (mFailed == otherCNST.mFailed);
            }
            
        };


        template<typename ValueType>
        class DFTDependency : public DFTElement<ValueType> {

            using DFTGatePointer = std::shared_ptr<DFTGate<ValueType>>;
            using DFTBEPointer = std::shared_ptr<DFTBE<ValueType>>;
            
        protected:
            std::string mNameTrigger;
            std::string mNameDependent;
            ValueType mProbability;
            DFTGatePointer mTriggerEvent;
            DFTBEPointer mDependentEvent;

        public:
            DFTDependency(size_t id, std::string const& name, std::string const& trigger, std::string const& dependent, ValueType probability) :
                DFTElement<ValueType>(id, name), mNameTrigger(trigger), mNameDependent(dependent), mProbability(probability)
            {
            }

            virtual ~DFTDependency() {}

            void initialize(DFTGatePointer triggerEvent, DFTBEPointer dependentEvent) {
                assert(triggerEvent->name() == mNameTrigger);
                assert(dependentEvent->name() == mNameDependent);
                mTriggerEvent = triggerEvent;
                mDependentEvent = dependentEvent;
            }

            std::string nameTrigger() const {
                return mNameTrigger;
            }

            std::string nameDependent() const {
                return mNameDependent;
            }

            ValueType const& probability() const {
                return mProbability;
            }

            DFTGatePointer const& triggerEvent() const {
                assert(mTriggerEvent);
                return mTriggerEvent;
            }

            DFTBEPointer const& dependentEvent() const {
                assert(mDependentEvent);
                return mDependentEvent;
            }

            DFTElementType type() const override {
                return DFTElementType::PDEP;
            }

            virtual size_t nrChildren() const override {
                return 1;
            }

            virtual bool isDependency() const override {
                return true;
            }
            
            virtual bool isTypeEqualTo(DFTElement<ValueType> const& other) const override {
                if(!DFTElement<ValueType>::isTypeEqualTo(other)) return false;
                DFTDependency<ValueType> const& otherDEP= static_cast<DFTDependency<ValueType> const&>(other);
                return (mProbability == otherDEP.mProbability);
            }
            

            virtual std::vector<size_t> independentUnit() const override {
                std::set<size_t> unit = {this->mId};
                mDependentEvent->extendUnit(unit);
                if(unit.count(mTriggerEvent->id()) != 0) {
                    return {};
                }
                return std::vector<size_t>(unit.begin(), unit.end());
            }

            virtual std::string toString() const override {
                std::stringstream stream;
                bool fdep = storm::utility::isOne(mProbability);
                stream << "{" << this->name() << "} " << (fdep ? "FDEP" : "PDEP") << "(" << mTriggerEvent->name() << " => " << mDependentEvent->name() << ")";
                if (!fdep) {
                    stream << " with probability " << mProbability;
                }
                return stream.str();
            }

        protected:

        };


        template<typename ValueType>
        class DFTAnd : public DFTGate<ValueType> {
            
        public:
            DFTAnd(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    for(auto const& child : this->mChildren)
                    {
                        if(!state.hasFailed(child->id())) {
                            return;
                        }
                    }
                    this->fail(state, queues);
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(this->hasFailsafeChild(state));
                if(state.isOperational(this->mId)) {
                    this->failsafe(state, queues);
                    this->childrenDontCare(state, queues);
                }
            }

            virtual DFTElementType type() const override {
                return DFTElementType::AND;
            }
            
            std::string typestring() const override {
                return "AND";
            }
        };

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTAnd<ValueType> const& gate) {
            return os << gate.toString();
        }

        

        template<typename ValueType>
        class DFTOr : public DFTGate<ValueType> {

        public:
            DFTOr(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(this->hasFailedChild(state));
                if(state.isOperational(this->mId)) {
                    this->fail(state, queues);
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                 for(auto const& child : this->mChildren) {
                     if(!state.isFailsafe(child->id())) {
                         return;
                     }
                 }
                 this->failsafe(state, queues);
            }

            virtual DFTElementType type() const override {
                return DFTElementType::OR;
            }
            
            std::string typestring() const override {
                return "OR";
            }
        };

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTOr<ValueType> const& gate) {
            return os << gate.toString();
        }



        template<typename ValueType>
        class DFTSeqAnd : public DFTGate<ValueType> {

        public:
            DFTSeqAnd(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state,  DFTStateSpaceGenerationQueues<ValueType>& queues) const {
                if(!state.hasFailed(this->mId)) {
                    bool childOperationalBefore = false;
                    for(auto const& child : this->mChildren)
                    {
                        if(!state.hasFailed(child->id())) {
                            childOperationalBefore = true;
                        }
                        else {
                            if(childOperationalBefore) {
                                state.markAsInvalid();
                                return;
                            }
                        }
                    }
                    if(!childOperationalBefore) {
                       fail(state, queues);
                    }
                  
                } 
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const{
                assert(hasFailsafeChild(state));
                if(state.isOperational(this->mId)) {
                    failsafe(state, queues);
                    //return true;
                }
                //return false;
            }

            virtual DFTElementType type() const override {
                return DFTElementType::SEQAND;
            }

            std::string  typestring() const override {
                return "SEQAND";
            }
        };

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTSeqAnd<ValueType> const& gate) {
             return os << gate.toString();
        }



        template<typename ValueType>
        class DFTPand : public DFTGate<ValueType> {

        public:
            DFTPand(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state,  DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    bool childOperationalBefore = false;
                    for(auto const& child : this->mChildren)
                    {
                        if(!state.hasFailed(child->id())) {
                            childOperationalBefore = true;
                        } else if(childOperationalBefore && state.hasFailed(child->id())){
                            this->failsafe(state, queues);
                            this->childrenDontCare(state, queues);
                            return;
                        }
                    }
                    if(!childOperationalBefore) {
                        this->fail(state, queues);
                    }
                }
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(this->hasFailsafeChild(state));
                if(state.isOperational(this->mId)) {
                    this->failsafe(state, queues);
                    this->childrenDontCare(state, queues);
                }
            }

            virtual DFTElementType type() const override {
                return DFTElementType::PAND;
            }
            
            std::string typestring() const override {
                return "PAND";
            }
        };
        
        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTPand<ValueType> const& gate) {
             return os << gate.toString();
        }



        template<typename ValueType>
        class DFTPor : public DFTGate<ValueType> {
        public:
            DFTPor(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                 assert(false);
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                assert(false);
            }

            virtual DFTElementType type() const override {
                return DFTElementType::POR;
            }
            
            std::string  typestring() const override {
                return "POR";
            }
        };

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTPor<ValueType> const& gate) {
             return os << gate.toString();
        }



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
                assert(this->hasFailsafeChild(state));
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

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTVot<ValueType> const& gate) {
            return os << gate.toString();
        }



        template<typename ValueType>
        class DFTSpare : public DFTGate<ValueType> {

        private:
            size_t mUseIndex;
            size_t mActiveIndex;
            
        public:
            DFTSpare(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children = {}) :
                    DFTGate<ValueType>(id, name, children)
            {}
            
            std::string typestring() const override {
                return "SPARE";
            }

            virtual DFTElementType type() const override {
                return DFTElementType::SPARE;
            }

            bool isSpareGate() const override {
                return true;
            }
            
            void setUseIndex(size_t useIndex) {
                mUseIndex = useIndex; 
            }
            
            void setActiveIndex(size_t activeIndex) {
                mActiveIndex = activeIndex;
            }

            void initializeUses(storm::storage::DFTState<ValueType>& state) {
                assert(this->mChildren.size() > 0);
                state.setUsesAtPosition(mUseIndex, this->mChildren[0]->id());
            }

            void checkFails(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    size_t uses = state.extractUses(mUseIndex);
                    if(!state.isOperational(uses)) {
                        bool claimingSuccessful = state.claimNew(this->mId, mUseIndex, uses, this->mChildren);
                        if(!claimingSuccessful) {
                            this->fail(state, queues);
                        }
                    }                  
                } 
            }

            void checkFailsafe(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(state.isOperational(this->mId)) {
                    if(state.isFailsafe(state.extractUses((mUseIndex)))) {
                        this->failsafe(state, queues);
                        this->childrenDontCare(state, queues);
                    }
                }
            }
        };
        
        template<typename ValueType>
        bool equalType(DFTElement<ValueType> const& e1, DFTElement<ValueType> const& e2) {
            return e1.isTypeEqualTo(e2);
        }
        
    }
}



#endif	/* DFTELEMENTS_H */

