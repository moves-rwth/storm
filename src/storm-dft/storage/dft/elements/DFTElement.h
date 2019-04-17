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

        /*!
         * Abstract base class for DFT elements.
         * It is the most general class.
         */
        template<typename ValueType>
        class DFTElement {

            using DFTGatePointer = std::shared_ptr<DFTGate<ValueType>>;
            using DFTGateVector = std::vector<DFTGatePointer>;
            using DFTDependencyPointer = std::shared_ptr<DFTDependency<ValueType>>;
            using DFTDependencyVector = std::vector<DFTDependencyPointer>;
            using DFTRestrictionPointer = std::shared_ptr<DFTRestriction<ValueType>>;
            using DFTRestrictionVector = std::vector<DFTRestrictionPointer>;

        public:
            /*!
             * Constructor.
             * @param id Id.
             * @param name Name.
             */
            DFTElement(size_t id, std::string const& name) : mId(id), mName(name), mRank(-1), mRelevant(false), mAllowDC(true) {
                // Intentionally left empty.
            }

            /*!
             * Destructor.
             */
            virtual ~DFTElement() {
                // Intentionally left empty.
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

            /*!
             * Get type as string.
             * @return String with type information.
             */
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

            /*!
             * Get whether the element is relevant.
             * Relevant elements are for example not set to Don't Care and their status is stored as a label in the generated Markov Chain.
             * @return True iff the element is relevant.
             */
            virtual bool isRelevant() const {
                return mRelevant;
            }

            /*!
             * Set the relevancy of the element.
             * @param relevant If true, the element is relevant.
             */
            virtual void setRelevance(bool relevant) const {
                this->mRelevant = relevant;
            }

            /*!
             * Set whether Don't Care propagation is allowed for this element.
             * @param allowDC If true, the element is allowed to be set to Don't Care.
             */
            virtual void setAllowDC(bool allowDC) const {
                this->mAllowDC = allowDC;
            }

            /*!
             * Checks whether the element is a basic element.
             * @return True iff element is a BE.
             */
            virtual bool isBasicElement() const {
                return false;
            }

            /*!
             * Check whether the element is a gate.
             * @return True iff element is a gate.
             */
            virtual bool isGate() const {
                return false;
            }

            /*!
             * Check whether the element is a SPARE gate.
             * @return True iff element is a SPARE gate.
             */
            virtual bool isSpareGate() const {
                return false;
            }

            /*!
             * Check whether the element is a dependency.
             * @return True iff element is a dependency.
             */
            virtual bool isDependency() const {
                return false;
            }

            /*!
             * Check whether the element is a restriction.
             * @return True iff element is a restriction.
             */
            virtual bool isRestriction() const {
                return false;
            }

            /*!
             * Return whether the element has parents.
             * @return True iff at least one parent exists.
             */
            bool hasParents() const {
                return !mParents.empty();
            }

            /*!
             * Return the number of parents.
             * @return Number of parents.
             */
            size_t nrParents() const {
                return mParents.size();
            }

            /*!
             * Get parents.
             * @return Parents.
             */
            DFTGateVector const& parents() const {
                return mParents;
            }

            /*!
             * Add parent.
             * @param parent Parent.
             */
            void addParent(DFTGatePointer const& parent) {
                if (std::find(this->parents().begin(), this->parents().end(), parent) == this->parents().end()) {
                    // Parent does not exist yet
                    mParents.push_back(parent);
                };
            }

            /*!
             * Return Ids of parents.
             * @return Parent ids.
             */
            std::vector<size_t> parentIds() const {
                std::vector<size_t> ids;
                for (auto parent : this->parents()) {
                    ids.push_back(parent->id());
                }
                return ids;
            }

            /*!
             * Check whether the element has only static gates as parents.
             * @return True iff all parents are static gates.
             */
            bool hasOnlyStaticParents() const {
                for (auto const& parent : this->parents()) {
                    if (!isStaticGateType(parent->type())) {
                        return false;
                    }
                }
                return true;
            }

            /*!
             * Return whether the element has restrictions.
             * @return True iff at least one restriction exists.
             */
            bool hasRestrictions() const {
                return !mRestrictions.empty();
            }

            /*!
             * Return the number of restrictions.
             * @return Number of restrictions.
             */
            size_t nrRestrictions() const {
                return mRestrictions.size();
            }

            /*!
             * Get restrictions.
             * @return Restrictions.
             */
            DFTRestrictionVector const& restrictions() const {
                return mRestrictions;
            }

            /*!
             * Add restriction.
             * @param restrictions Restriction.
             */
            void addRestriction(DFTRestrictionPointer const& restriction) {
                if (std::find(this->restrictions().begin(), this->restrictions().end(), restriction) == this->restrictions().end()) {
                    // Restriction does not exist yet
                    mRestrictions.push_back(restriction);
                }
            }

            /*!
             * Return whether the element has outgoing dependencies.
             * @return True iff at least one restriction exists.
             */
            bool hasOutgoingDependencies() const {
                return !mOutgoingDependencies.empty();
            }

            /*!
             * Return the number of outgoing dependencies.
             * @return Number of outgoing dependencies.
             */
            size_t nrOutgoingDependencies() const {
                return mOutgoingDependencies.size();
            }

            /*!
             * Get outgoing dependencies.
             * @return Outgoing dependencies.
             */
            DFTDependencyVector const& outgoingDependencies() const {
                return mOutgoingDependencies;
            }

            /*!
             * Add outgoing dependency.
             * @param outgoingDependency Outgoing dependency.
             */
            void addOutgoingDependency(DFTDependencyPointer const& outgoingDependency) {
                STORM_LOG_ASSERT(outgoingDependency->triggerEvent()->id() == this->id(), "Ids do not match.");
                if (std::find(this->outgoingDependencies().begin(), this->outgoingDependencies().end(), outgoingDependency) == this->outgoingDependencies().end()) {
                    // Outgoing dependency does not exist yet
                    mOutgoingDependencies.push_back(outgoingDependency);
                };
            }

            /**
             * Obtains ids of elements which are the direct successor in the list of children of a restriction
             * @return A vector of ids
             */
            std::vector<size_t> seqRestrictionPosts() const {
                std::vector<size_t> res;
                for (auto const& restr : mRestrictions) {
                    if (!restr->isSeqEnforcer()) {
                        continue;
                    }
                    auto it = restr->children().cbegin();
                    for (; it != restr->children().cend(); ++it) {
                        if ((*it)->id() == mId) {
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(it != restr->children().cend(), "Child not found.");
                    ++it;
                    if (it != restr->children().cend()) {
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
                    if (!restr->isSeqEnforcer()) {
                        continue;
                    }
                    auto it = restr->children().cbegin();
                    for (; it != restr->children().cend(); ++it) {
                        if ((*it)->id() == mId) {
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(it != restr->children().cend(), "Child not found.");
                    if (it != restr->children().cbegin()) {
                        --it;
                        res.push_back((*it)->id());
                    }
                }
                return res;
            }

            /**
             * Obtains ids of elements which are under the same mutex.
             * @return A vector of ids
             */
            std::vector<size_t> mutexRestrictionElements() const {
                std::vector<size_t> res;
                for (auto const& restr : mRestrictions) {
                    if (!restr->isMutex()) {
                        continue;
                    }
                    bool found = false;
                    for (auto it = restr->children().cbegin(); it != restr->children().cend(); ++it) {
                        if ((*it)->id() != mId) {
                            res.push_back((*it)->id());
                        } else {
                            found = true;
                        }
                    }
                    STORM_LOG_ASSERT(found, "Child " << mId << " is not included in restriction " << *restr);
                }
                return res;
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

            /*!
             * Check whether two elements have the same type.
             * @param other Other element.
             * @return True iff this and other have the same type.
             */
            virtual bool isTypeEqualTo(DFTElement<ValueType> const& other) const {
                return type() == other.type();
            }

            /*!
             * Print information about element to string.
             * @return Element information.
             */
            virtual std::string toString() const = 0;


        protected:
            std::size_t mId;
            std::string mName;
            std::size_t mRank;
            DFTGateVector mParents;
            DFTDependencyVector mOutgoingDependencies;
            DFTRestrictionVector mRestrictions;
            mutable bool mRelevant; // Must be mutable to allow changes later on. TODO: avoid mutable
            mutable bool mAllowDC; // Must be mutable to allow changes later on. TODO: avoid mutable
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
