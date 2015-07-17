#ifndef DFTELEMENTS_H
#define	DFTELEMENTS_H

#include <set>
#include <vector>
#include <memory>
#include <string>
#include <cassert>
#include <cstdlib>
#include <iostream>

#include "DFTState.h"
#include "DFTStateSpaceGenerationQueues.h"

using std::size_t;

namespace storm {
    namespace storage {
        class DFTGate;
        
        class DFTElement {
        protected:
            size_t mId;
            std::string mName;
            size_t mRank = -1;
            std::vector<std::shared_ptr<DFTGate>> mParents;
            

            

        public:
            DFTElement(size_t id, std::string const& name) : mId(id), mName(name)
            {}
            virtual ~DFTElement() {}

            
            virtual size_t id() const {
                return mId;
            }
            
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
            
            virtual bool isBasicElement() const {
                return false;
            }
            
            virtual bool isColdBasicElement() const {
                return false;
            }
            
            virtual bool isSpareGate() const {
                return false;
            }
            
            virtual void setId(size_t newId) {
                mId = newId;
            }
            
            virtual std::string const& name() const {
                return mName;
            }
            
            bool addParent(std::shared_ptr<DFTGate> const& e) {
                if(std::find(mParents.begin(), mParents.end(), e) != mParents.end()) {
                    return false;
                }
                else 
                {
                    mParents.push_back(e);
                    return true;
                }
            }

            bool hasParents() const {
                return !mParents.empty();
            }
            
            std::vector<std::shared_ptr<DFTGate>> const& parents() const {
                return mParents;
            }
            
            virtual void extendSpareModule(std::set<size_t>& elementsInModule) const;
            
            virtual size_t nrChildren() const = 0;
            virtual void print(std::ostream& = std::cout) const = 0;
            
            virtual bool checkDontCareAnymore(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const;
            
            virtual std::vector<size_t> independentUnit() const = 0;
        
            virtual void  extendUnit(std::set<size_t>& unit) const;
        };


        enum class DFTElementTypes {AND, COUNTING, OR, VOT, BE, CONSTF, CONSTS, PAND, SPARE, POR, FDEP, SEQAND};

        inline bool isGateType(DFTElementTypes const& tp) {
            switch(tp) {
                case DFTElementTypes::AND:
                case DFTElementTypes::COUNTING:
                case DFTElementTypes::OR:
                case DFTElementTypes::VOT:
                case DFTElementTypes::PAND:
                case DFTElementTypes::SPARE:
                case DFTElementTypes::POR:
                case DFTElementTypes::SEQAND:
                    return true;
                case DFTElementTypes::BE:
                case DFTElementTypes::CONSTF:
                case DFTElementTypes::CONSTS:
                case DFTElementTypes::FDEP:
                    return false;
                default:
                    assert(false);
            }
        }
        
        class DFTGate : public DFTElement {
        protected:
            std::vector<std::shared_ptr<DFTElement>> mChildren;
        public:
            DFTGate(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement>> const& children) :
            DFTElement(id, name), mChildren(children)
            {}
            
            virtual ~DFTGate() {}
            
            void pushBackChild(std::shared_ptr<DFTElement> elem) {
                return mChildren.push_back(elem);
            }

            size_t nrChildren() const {
                return mChildren.size();
            }
            
            std::vector<std::shared_ptr<DFTElement>> const& children() const {
                return mChildren;
            }
            
            virtual bool isGate() const {
                return true;
            }
            
            
            virtual std::string typestring() const = 0;
            virtual void checkFails(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const = 0;
            virtual void checkFailsafe(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const = 0;
            
            virtual void extendSpareModule(std::set<size_t>& elementsInSpareModule) const override {
                DFTElement::extendSpareModule(elementsInSpareModule);
                for(auto const& child : mChildren) {
                    if(elementsInSpareModule.count(child->id()) == 0) {
                        elementsInSpareModule.insert(child->id());
                        child->extendSpareModule(elementsInSpareModule);
                    }
                }
            }
            
            virtual std::vector<size_t> independentUnit() const {
                std::set<size_t> unit = {mId};
                for(auto const& child : mChildren) {
                    child->extendUnit(unit);
                }
                for(auto const& parent : mParents) {
                    if(unit.count(parent->id()) != 0) {
                        return {};
                    } 
                }
                return std::vector<size_t>(unit.begin(), unit.end()); 
           }

            
            
            virtual void print(std::ostream& os = std::cout) const {
                os << "{" << name() << "} " << typestring() << "( ";
                std::vector<std::shared_ptr<DFTElement>>::const_iterator it = mChildren.begin();
                os << (*it)->name();
                ++it;
                while(it != mChildren.end()) {
                    os <<  ", " << (*it)->name();
                    ++it;
                }
                os << ")";
            }    
            
            virtual bool checkDontCareAnymore(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                if(DFTElement::checkDontCareAnymore(state, queues)) {
                    childrenDontCare(state, queues);
                    return true;
                }
                return false;
            }
            
            virtual void extendUnit(std::set<size_t>& unit) const {
                DFTElement::extendUnit(unit);
                for(auto const& child : mChildren) {
                    child->extendUnit(unit);
                }
            }
            
        protected:
            
            void fail(DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                for(std::shared_ptr<DFTGate> parent : mParents) {
                    if(state.isOperational(parent->id())) {
                        queues.propagateFailure(parent);
                    }
                }
                state.setFailed(mId);        
            }
            
            void failsafe(DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                for(std::shared_ptr<DFTGate> parent : mParents) {
                    if(state.isOperational(parent->id())) {
                        queues.propagateFailsafe(parent);
                    }
                }
                state.setFailsafe(mId);
            }
            
            void childrenDontCare(DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                queues.propagateDontCare(mChildren);
            }
            
            bool hasFailsafeChild(DFTState& state) const {
                for(auto const& child : mChildren) {
                    if(state.isFailsafe(child->id()))
                    {
                        return true;
                    }
                }
                return false;
            }
            
            
            bool hasFailedChild(DFTState& state) const {
                for(auto const& child : mChildren) {
                    if(state.hasFailed(child->id())) {
                        return true;
                    }
                }
                return false;
            }

        };
        
       

        template<typename FailureRateType>
        class DFTBE : public DFTElement {

            
            FailureRateType mActiveFailureRate;
            FailureRateType mPassiveFailureRate;
        public:
            DFTBE(size_t id, std::string const& name, FailureRateType failureRate, FailureRateType dormancyFactor) :
            DFTElement(id, name), mActiveFailureRate(failureRate), mPassiveFailureRate(dormancyFactor * failureRate)
            {
                
            }
                        
            virtual size_t nrChildren() const {
                return 0;
            }
            
            FailureRateType const& activeFailureRate() const {
                return mActiveFailureRate;
            }
            
            FailureRateType const& passiveFailureRate() const {
                return mPassiveFailureRate;
            }
        
            void print(std::ostream& os = std::cout) const {
                os << *this;
            }
            
            bool isBasicElement() const {
                return true;
            }
            
            bool isColdBasicElement() const {
                return mPassiveFailureRate == 0;
            }
            
            virtual std::vector<size_t> independentUnit() const {
                return {mId};
            }
            virtual bool checkDontCareAnymore(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const;
        };
        
        inline std::ostream& operator<<(std::ostream& os, DFTBE<double> const& be) {
            return os << "{" << be.name() << "} BE(" << be.activeFailureRate() << ", " << be.passiveFailureRate() << ")";
        }

        
        

        class DFTConst : public DFTElement {
            bool mFailed;
        public:
            DFTConst(size_t id, std::string const& name, bool failed) : DFTElement(id, name), mFailed(failed) 
            {
                
            }
            
            bool failed() const {
                return mFailed;
            }
            
            virtual bool isConstant() const {
                return true;
            }
            
            virtual size_t nrChildren() const {
                return 0;
            }
            
        };

        class DFTAnd : public DFTGate {
            
        public:
            DFTAnd(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement>> const& children = {}) :
            DFTGate(id, name, children) 
            {}
            
            void checkFails(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                
                if(state.isOperational(mId)) {
                    for(auto const& child : mChildren)
                    {
                        if(!state.hasFailed(child->id())) {
                            return;// false;
                        }
                    }
                    fail(state, queues);
                    //return true;
                } 
                //return false;
            }
            
            void checkFailsafe(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const{
                assert(hasFailsafeChild(state));
                if(state.isOperational(mId)) {
                    failsafe(state, queues);
                    childrenDontCare(state, queues);
                    //return true;
                }
                //return false;
            }
            
            std::string typestring() const {
                return "AND";
            }
        };
        
        inline std::ostream& operator<<(std::ostream& os, DFTAnd const& gate) {
            gate.print(os);
            return os;
        }
       
        

        class DFTOr : public DFTGate {
        public:
            DFTOr(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement>> const& children = {}) :
            DFTGate(id, name, children) 
            {}
            
             void checkFails(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                 assert(hasFailedChild(state));
                if(state.isOperational(mId)) {
                    fail(state, queues);
                    //return true;
                } 
                // return false;
            }
             
            void checkFailsafe(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const{
                 for(auto const& child : mChildren) {
                     if(!state.isFailsafe(child->id())) {
                         return;// false;
                     }
                 }
                 failsafe(state, queues);
                 //return true;
            }
            
            std::string typestring() const {
                return "OR";
            }
        private:
            //static const std::string typestring = "or";
        };
        
         inline std::ostream& operator<<(std::ostream& os, DFTOr const& gate) {
            gate.print(os);
            return os;
        }

        class DFTSeqAnd : public DFTGate {
        public:
            DFTSeqAnd(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement>> const& children = {}) :
            DFTGate(id, name, children) 
            {}
            
            void checkFails(storm::storage::DFTState& state,  DFTStateSpaceGenerationQueues& queues) const {
                
                if(!state.hasFailed(mId)) {
                    bool childOperationalBefore = false;
                    for(auto const& child : mChildren)
                    {
                        if(!state.hasFailed(child->id())) {
                            childOperationalBefore = true;
                        }
                        else {
                            if(childOperationalBefore) {
                                state.markAsInvalid();
                                return; //false;
                            }
                        }
                    }
                    if(!childOperationalBefore) {
                       fail(state, queues);
                       //return true;
                    }
                  
                } 
                //return false;
            }
            
            void checkFailsafe(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const{
                assert(hasFailsafeChild(state));
                if(state.isOperational(mId)) {
                    failsafe(state, queues);
                    //return true;
                }
                //return false;
            }
            
            std::string  typestring() const {
                return "SEQAND";
            }
        private:
            //static const std::string typestring = "seqand"; 
        };
        
         inline std::ostream& operator<<(std::ostream& os, DFTSeqAnd const& gate) {
            gate.print(os);
            return os;
        }
        
        class DFTPand : public DFTGate {
        public:
            DFTPand(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement>> const& children = {}) :
            DFTGate(id, name, children) 
            {}
            
            void checkFails(storm::storage::DFTState& state,  DFTStateSpaceGenerationQueues& queues) const {
                if(state.isOperational(mId)) {
                    bool childOperationalBefore = false;
                    for(auto const& child : mChildren)
                    {
                        if(!state.hasFailed(child->id())) {
                            childOperationalBefore = true;
                        } else if(childOperationalBefore && state.hasFailed(child->id())){
                            failsafe(state, queues);
                            childrenDontCare(state, queues);
                            return; //false;
                        }
                    }
                    if(!childOperationalBefore) {
                        fail(state, queues);
                        //return true;
                    }
                }
                // return false;
            }
             
            void checkFailsafe(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const{
                assert(hasFailsafeChild(state));
                if(state.isOperational(mId)) {
                    failsafe(state, queues);
                    childrenDontCare(state, queues);
                    //return true;
                }
                //return false;
            } 
            
            std::string typestring() const {
                return "PAND";
            }
        };
        
         inline std::ostream& operator<<(std::ostream& os, DFTPand const& gate) {
            gate.print(os);
            return os;
        }
        
        class DFTPor : public DFTGate {
        public:
            DFTPor(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement>> const& children = {}) :
            DFTGate(id, name, children) 
            {}
            
            void checkFails(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                 assert(false);
            }
             
            void checkFailsafe(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const{
                assert(false);
            }
            
            std::string  typestring() const {
                return "POR";
            }
        };
        
         inline std::ostream& operator<<(std::ostream& os, DFTPor const& gate) {
            gate.print(os);
            return os;
        }

        class DFTVot : public DFTGate {
        private:
            unsigned mThreshold;
        public:
            DFTVot(size_t id, std::string const& name, unsigned threshold, std::vector<std::shared_ptr<DFTElement>> const& children = {}) :
            DFTGate(id, name, children), mThreshold(threshold)
            {}
            
            void checkFails(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                
                if(state.isOperational(mId)) {
                    unsigned nrFailedChildren = 0;
                    for(auto const& child : mChildren)
                    {
                        if(state.hasFailed(child->id())) {
                            ++nrFailedChildren;
                            if(nrFailedChildren >= mThreshold) 
                            {
                                fail(state, queues);
                                return;// true;
                            }
                        }
                    }
                  
                } 
                //    return false;
               
            }
            
            void checkFailsafe(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const{
                assert(hasFailsafeChild(state));
                if(state.isOperational(mId)) {
                    unsigned nrFailsafeChildren = 0;
                    for(auto const& child : mChildren)
                    {
                        if(state.isFailsafe(child->id())) {
                            ++nrFailsafeChildren;
                            if(nrFailsafeChildren > nrChildren() - mThreshold) 
                            {
                                failsafe(state, queues);
                                childrenDontCare(state, queues);
                                return;// true;
                            }
                        }
                    }
                }
                //return false;
            }
            
            std::string typestring() const {
                return "VOT (" + std::to_string(mThreshold) + ")";
            }

        };

        inline std::ostream& operator<<(std::ostream& os, DFTVot const& gate) {
            gate.print(os);
            return os;
        }        
        
        class DFTSpare : public DFTGate {
            size_t mUseIndex;
            size_t mActiveIndex;
            
        public:
            DFTSpare(size_t id, std::string const& name, std::vector<std::shared_ptr<DFTElement>> const& children = {}) :
            DFTGate(id, name, children)
            {
                
            }
            
            
            std::string typestring() const {
                return "SPARE";
            }
            
            bool isSpareGate() const {
                return true;
            }
            
            void setUseIndex(size_t useIndex) {
                mUseIndex = useIndex; 
            }
            
            void setActiveIndex(size_t activeIndex) {
                mActiveIndex = activeIndex;
            }
            
            void initializeUses(storm::storage::DFTState& state) {
                assert(mChildren.size() > 0);
                state.setUsesAtPosition(mUseIndex, mChildren[0]->id());
            }
            
            void checkFails(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                if(state.isOperational(mId)) {
                    size_t uses = state.extractUses(mUseIndex);
                    if(!state.isOperational(uses)) {
                        // TODO compute children ids before.
                        std::vector<size_t> childrenIds;
                        for(auto const& child : mChildren) {
                            childrenIds.push_back(child->id());
                        }
                        
                        bool claimingSuccessful = state.claimNew(mId, mUseIndex, uses, childrenIds);
                        if(!claimingSuccessful) {
                            fail(state, queues);
                        }
                    }                  
                } 
            }
            
            void checkFailsafe(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
                if(state.isOperational(mId)) {
                    if(state.isFailsafe(state.extractUses((mUseIndex)))) {
                        failsafe(state, queues);
                        childrenDontCare(state, queues);
                    }
                }
            }
        };
    }
}



#endif	/* DFTELEMENTS_H */

