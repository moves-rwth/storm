#pragma once
namespace storm {
    namespace storage {
        template<typename ValueType>
        class DFTBE : public DFTElement<ValueType> {
            
            using DFTDependencyPointer = std::shared_ptr<DFTDependency<ValueType>>;
            using DFTDependencyVector = std::vector<DFTDependencyPointer>;

        protected:
            ValueType mActiveFailureRate;
            ValueType mPassiveFailureRate;
            DFTDependencyVector mIngoingDependencies;
            bool mTransient;

        public:
            DFTBE(size_t id, std::string const& name, ValueType failureRate, ValueType dormancyFactor, bool transient = false) :
                    DFTElement<ValueType>(id, name), mActiveFailureRate(failureRate), mPassiveFailureRate(dormancyFactor * failureRate), mTransient(transient)
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

            ValueType dormancyFactor() const {
                if (storm::utility::isZero<ValueType>(this->activeFailureRate())) {
                    // Return default value of 0
                    return storm::utility::zero<ValueType>();
                } else {
                    return this->passiveFailureRate() / this->activeFailureRate();
                }
            }

            bool isTransient() const {
                return mTransient;
            }
            
            bool canFail() const {
                return !storm::utility::isZero(mActiveFailureRate);
            }
            
            bool addIngoingDependency(DFTDependencyPointer const& e) {
                // TODO write this assertion for n-ary dependencies, probably by addign a method to the dependencies to support this.
                //STORM_LOG_ASSERT(e->dependentEvent()->id() == this->id(), "Ids do not match.");
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
            
            DFTDependencyVector const& ingoingDependencies() const {
                return mIngoingDependencies;
            }
        
            std::string toString() const override {
                std::stringstream stream;
                stream << *this;
                return stream.str();
            }
            
            bool isBasicElement() const override {
                return true;
            }
            
            bool isColdBasicElement() const {
                return storm::utility::isZero(mPassiveFailureRate);
            }
            
            virtual void extendSubDft(std::set<size_t>& elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot, bool blockParents, bool sparesAsLeaves) const override {
                if(elemsInSubtree.count(this->id())) {
                    return;
                }
                DFTElement<ValueType>::extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                if(elemsInSubtree.empty()) {
                    // Parent in the subdft, ie it is *not* a subdft
                    return;
                }
                for(auto const& incDep : mIngoingDependencies) {
                    incDep->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents, sparesAsLeaves);
                    if(elemsInSubtree.empty()) {
                        // Parent in the subdft, ie it is *not* a subdft
                        return;
                    }
                }
            }
            
            bool isTypeEqualTo(DFTElement<ValueType> const& other) const override {
                if(!DFTElement<ValueType>::isTypeEqualTo(other)) return false;
                DFTBE<ValueType> const&  otherBE = static_cast<DFTBE<ValueType> const&>(other);
                return (mActiveFailureRate == otherBE.mActiveFailureRate) && (mPassiveFailureRate == otherBE.mPassiveFailureRate);
            }
            
            virtual bool checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const override {
                if(DFTElement<ValueType>::checkDontCareAnymore(state, queues)) {
                    state.beNoLongerFailable(this->mId);
                    return true;
                }
                return false;
            }
        };

        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTBE<ValueType> const& be) {
            return os << "{" << be.name() << "} BE(" << be.activeFailureRate() << ", " << be.passiveFailureRate() << ")";
        }
    }
}
