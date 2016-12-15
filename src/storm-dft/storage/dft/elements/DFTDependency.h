#pragma once


#include "DFTElement.h"
namespace storm {
    namespace storage {
        
        template<typename ValueType>
        class DFTDependency : public DFTElement<ValueType> {

            using DFTGatePointer = std::shared_ptr<DFTGate<ValueType>>;
            using DFTBEPointer = std::shared_ptr<DFTBE<ValueType>>;
            
        protected:
            ValueType mProbability;
            DFTGatePointer mTriggerEvent;
            DFTBEPointer mDependentEvent;

        public:
            DFTDependency(size_t id, std::string const& name, ValueType probability) :
                DFTElement<ValueType>(id, name), mProbability(probability)
            {
            }

            virtual ~DFTDependency() {}

            void setTriggerElement(DFTGatePointer const& triggerEvent) {
                mTriggerEvent = triggerEvent;

            }

            void setDependentEvent(DFTBEPointer const& dependentEvent) {
                mDependentEvent = dependentEvent;
            }


            ValueType const& probability() const {
                return mProbability;
            }

            DFTGatePointer const& triggerEvent() const {
                STORM_LOG_ASSERT(mTriggerEvent, "Trigger does not exist.");
                return mTriggerEvent;
            }

            DFTBEPointer const& dependentEvent() const {
                STORM_LOG_ASSERT(mDependentEvent, "Dependent element does not exists.");
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

            virtual void extendSubDft(std::set<size_t>& elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot, bool blockParents, bool sparesAsLeaves) const override {
                if(elemsInSubtree.count(this->id())) return;
                DFTElement<ValueType>::extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                if(elemsInSubtree.empty()) {
                    // Parent in the subdft, ie it is *not* a subdft
                    return;
                }
                mDependentEvent->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                if(elemsInSubtree.empty()) {
                    // Parent in the subdft, ie it is *not* a subdft
                    return;
                }
                mTriggerEvent->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                
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

    }
}
