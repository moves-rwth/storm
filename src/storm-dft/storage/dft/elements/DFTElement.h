#pragma once 

#include <string>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <set>
#include <vector>
#include <memory>
#include <map>

#include "storm-dft/storage/dft/DFTElementType.h"
#include "storm-dft/storage/dft/DFTState.h"
#include "storm-dft/storage/dft/DFTStateSpaceGenerationQueues.h"
#include "storm/utility/constants.h"
#include "storm/adapters/RationalFunctionAdapter.h"




namespace storm {
    namespace storage {
        
        using std::size_t;

        // Forward declarations
        template<typename ValueType>
        class DFTGate;
        
        template<typename ValueType>
        class DFTDependency;
        template<typename ValueType>
        class DFTRestriction;

        
        template<typename ValueType>
        class DFTElement {

            using DFTGatePointer = std::shared_ptr<DFTGate<ValueType>>;
            using DFTGateVector = std::vector<DFTGatePointer>;
            using DFTDependencyPointer = std::shared_ptr<DFTDependency<ValueType>>;
            using DFTDependencyVector = std::vector<DFTDependencyPointer>;
            using DFTRestrictionPointer = std::shared_ptr<DFTRestriction<ValueType>>;
            using DFTRestrictionVector = std::vector<DFTRestrictionPointer>;


        protected:
            std::size_t mId;
            std::string mName;
            std::size_t mRank = -1;
            DFTGateVector mParents;
            DFTDependencyVector mOutgoingDependencies;
            DFTRestrictionVector mRestrictions;


        public:
            /*!
             * Constructor.
             * @param id Id.
             * @param name Name.
             */
            DFTElement(size_t id, std::string const& name) : mId(id), mName(name) {
            }

            /*!
             * Destructor.
             */
            virtual ~DFTElement() {
            }

            /*!
             * Get id.
             * @return Id.
             */
            virtual size_t id() const {
                return mId;
            }

            /*!
             * Set id.
             * @param id Id.
             */
            virtual void setId(size_t id) {
                this->mId = id;
            }

            /*!
             * Get name.
             * @return Name.
             */
            virtual std::string const& name() const {
                return mName;
            }


            /*!
             * Get type.
             * @return Type.
             */
            virtual DFTElementType type() const = 0;

            virtual std::string typestring() const {
                return storm::storage::toString(this->type());
            }

            /*!
             * Get rank.
             * @return Rank.
             */
            virtual size_t rank() const {
                return mRank;
            }

            /*!
             * Set rank.
             * @param rank Rank.
             */
            virtual void setRank(size_t rank) {
                this->mRank = rank;
            }

            virtual bool isConstant() const {
                return false;
            }

            /*!
             * Checks whether the element is a basic element.
             * @return True iff element is a BE.
             */
            virtual bool isBasicElement() const {
                return false;
            }

            /*!
             * Check wether the element is a gate.
             * @return True iff element is a gate.
             */
            virtual bool isGate() const {
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
            
            virtual bool isRestriction() const {
                return false;
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

            bool addRestriction(DFTRestrictionPointer const& e) {
                if (std::find(mRestrictions.begin(), mRestrictions.end(), e) != mRestrictions.end()) {
                    return false;
                } else {
                    mRestrictions.push_back(e);
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
            
            bool hasRestrictions() const {
                return !mRestrictions.empty();
            }
            
            size_t nrRestrictions() const {
                return mRestrictions.size();
            }
            
            DFTRestrictionVector const& restrictions() const {
                return mRestrictions;
            }

            std::vector<size_t> parentIds() const {
                std::vector<size_t> res;
                for(auto parent : parents()) {
                    res.push_back(parent->id());
                }
                return res;
            }
            
            bool addOutgoingDependency(DFTDependencyPointer const& e) {
                STORM_LOG_ASSERT(e->triggerEvent()->id() == this->id(), "Ids do not match.");
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
            
            /**
             * Obtains ids of elements which are the direct successor in the list of children of a restriction
             * @return A vector of ids
             */
            std::vector<size_t> seqRestrictionPosts() const {
                std::vector<size_t> res;
                for (auto const& restr : mRestrictions) {
                    if(!restr->isSeqEnforcer()) {
                        continue;
                    }
                    auto it = restr->children().cbegin();
                    for(; it != restr->children().cend(); ++it) {
                        if((*it)->id() == mId) {
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(it != restr->children().cend(), "Child not found.");
                    ++it;
                    if(it != restr->children().cend()) {
                        res.push_back((*it)->id());
                    }
                }
                return res;
            }
            
            /**
             * Obtains ids of elements which are the direct predecessor in the list of children of a restriction
             * @return A vector of ids
             */
            std::vector<size_t> seqRestrictionPres() const {
                std::vector<size_t> res;
                for (auto const& restr : mRestrictions) {
                    if(!restr->isSeqEnforcer()) {
                        continue;
                    }
                    auto it = restr->children().cbegin();
                    for(; it != restr->children().cend(); ++it) {
                        if((*it)->id() == mId) {
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(it != restr->children().cend(), "Child not found.");
                    if(it != restr->children().cbegin()) {
                        --it;
                        res.push_back((*it)->id());
                    }
                }
                return res;
            }
            
            DFTDependencyVector const& outgoingDependencies() const {
                return mOutgoingDependencies;
            }
            
            virtual void extendSpareModule(std::set<size_t>& elementsInModule) const;

           // virtual void extendImmediateFailureCausePathEvents(std::set<size_t>& ) const;
            /*!
             * Get number of children.
             * @return Nr of children.
             */
            virtual std::size_t nrChildren() const = 0;

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
             *  - a probabilistic dependency
             *  such that there exists a  path from x to a child of this does not go through this.
             */
            virtual std::vector<size_t> independentSubDft(bool blockParents, bool sparesAsLeaves = false) const;
            /**
             * Helper to the independent subtree computation
             * @see independentSubDft
             */
            virtual void extendSubDft(std::set<size_t>& elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot, bool blockParents, bool sparesAsLeaves) const;

            virtual bool isTypeEqualTo(DFTElement<ValueType> const& other) const {
                return type() == other.type();
            }

            /*!
             * Print information about element to string.
             * @return Element information.
             */
            virtual std::string toString() const = 0;


        protected:
           // virtual bool checkIsomorphicSubDftHelper(DFTElement const& otherElem, std::vector<std::pair<DFTElement const&, DFTElement const&>>& mapping, std::vector<DFTElement const&> const& order ) const = 0;

        };


        template<typename ValueType>
        inline std::ostream& operator<<(std::ostream& os, DFTElement<ValueType> const& element) {
            return os << element.toString();
        }
          
        template<typename ValueType>
        bool equalType(DFTElement<ValueType> const& e1, DFTElement<ValueType> const& e2) {
            return e1.isTypeEqualTo(e2);
        }
    }
}
