#pragma  once
#include <iostream>
#include <unordered_map>
#include <map>

#include "storm/utility/macros.h"

#include "storm-dft/storage/dft/DFTElements.h"
#include "storm-dft/storage/dft/elements/DFTRestriction.h"
#include "storm-dft/storage/dft/DFTLayoutInfo.h"
#include "storm/exceptions/NotSupportedException.h"


namespace storm {
    namespace storage {
        // Forward declaration
        template<typename ValueType>
        class DFT;
    }

    namespace builder {

        template<typename ValueType>
        class DFTBuilder {

            using DFTElementPointer = std::shared_ptr<storm::storage::DFTElement<ValueType>>;
            using DFTElementVector = std::vector<DFTElementPointer>;
            using DFTBEPointer = std::shared_ptr<storm::storage::DFTBE<ValueType>>;
            using DFTGatePointer = std::shared_ptr<storm::storage::DFTGate<ValueType>>;
            using DFTGateVector = std::vector<DFTGatePointer>;
            using DFTDependencyPointer = std::shared_ptr<storm::storage::DFTDependency<ValueType>>;
            using DFTRestrictionPointer = std::shared_ptr<storm::storage::DFTRestriction<ValueType>>;

        private:
            std::size_t mNextId = 0;
            static std::size_t mUniqueOffset;
            std::string mTopLevelIdentifier;
            std::unordered_map<std::string, DFTElementPointer> mElements;
            std::unordered_map<DFTElementPointer, std::vector<std::string>> mChildNames;
            std::unordered_map<DFTRestrictionPointer, std::vector<std::string>> mRestrictionChildNames;
            std::unordered_map<DFTDependencyPointer, std::vector<std::string>> mDependencyChildNames;
            std::vector<DFTDependencyPointer> mDependencies;
            std::vector<DFTRestrictionPointer> mRestrictions;
            std::unordered_map<std::string, storm::storage::DFTLayoutInfo> mLayoutInfo;
            
        public:
            DFTBuilder(bool defaultInclusive = true) : pandDefaultInclusive(defaultInclusive),
                                                       porDefaultInclusive(defaultInclusive) {
                
            }
            
            bool addAndElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, storm::storage::DFTElementType::AND);
            }
            
            bool addOrElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, storm::storage::DFTElementType::OR);
            }
            
            bool addPandElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, storm::storage::DFTElementType::PAND);
            }
            
            bool addPandElement(std::string const& name, std::vector<std::string> const& children, bool inclusive) {
                bool tmpDefault = pandDefaultInclusive;
                pandDefaultInclusive = inclusive;
                bool result = addStandardGate(name, children, storm::storage::DFTElementType::PAND);
                pandDefaultInclusive = tmpDefault;
                return result;
            }
            
            bool addPorElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, storm::storage::DFTElementType::POR);
            }
            
            bool addPorElement(std::string const& name, std::vector<std::string> const& children, bool inclusive) {
                bool tmpDefault = porDefaultInclusive;
                porDefaultInclusive = inclusive;
                bool result = addStandardGate(name, children, storm::storage::DFTElementType::POR);
                pandDefaultInclusive = tmpDefault;
                return result;
            }
            
            bool addSpareElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, storm::storage::DFTElementType::SPARE);
            }

            bool addSequenceEnforcer(std::string const& name, std::vector<std::string> const& children) {
                return addRestriction(name, children, storm::storage::DFTElementType::SEQ);
            }

            bool addMutex(std::string const& name, std::vector<std::string> const& children) {
                return addRestriction(name, children, storm::storage::DFTElementType::MUTEX);
            }
            
            bool addDepElement(std::string const& name, std::vector<std::string> const& children, ValueType probability) {
                if(children.size() <= 1) {
                    STORM_LOG_ERROR("Dependencies require at least two children");
                }
                if (nameInUse(name)) {
                    STORM_LOG_ERROR("Element with name '" << name << "' already exists.");
                    return false;
                }

                if (storm::utility::isZero(probability)) {
                    // Element is superfluous
                    return true;
                }
                std::string trigger = children[0];

                //TODO: collect constraints for SMT solving
                DFTDependencyPointer element = std::make_shared<storm::storage::DFTDependency<ValueType>>(mNextId++,
                                                                                                          name,
                                                                                                          probability);
                mElements[element->name()] = element;
                mDependencyChildNames[element] = children;
                mDependencies.push_back(element);
                return true;
            }

            bool addVotElement(std::string const& name, unsigned threshold, std::vector<std::string> const& children) {
                STORM_LOG_ASSERT(children.size() > 0, "Has no child.");
                if (nameInUse(name)) {
                    STORM_LOG_ERROR("Element with name '" << name << "' already exists.");
                    return false;
                }
                // It is an and-gate
                if(children.size() == threshold) {
                    return addAndElement(name, children);
                }
                // It is an or-gate
                if(threshold == 1) {
                    return addOrElement(name, children);
                }
                
                if(threshold > children.size()) {
                    STORM_LOG_ERROR("Voting gates with threshold higher than the number of children is not supported.");
                    return false;
                }
                DFTElementPointer element = std::make_shared<storm::storage::DFTVot<ValueType>>(mNextId++, name, threshold);
                
                mElements[name] = element;
                mChildNames[element] = children;
                return true;
            }

            bool addBasicElementConst(std::string const& name, bool failed) {
                if (nameInUse(name)) {
                    STORM_LOG_ERROR("Element with name '" << name << "' already exists.");
                    return false;
                }
                mElements[name] = std::make_shared<storm::storage::BEConst<ValueType>>(mNextId++, name, failed);
                return true;
            }

            bool addBasicElementProbability(std::string const& name, ValueType probability, ValueType dormancyFactor, bool transient = false) {
                //0 <= dormancyFactor <= 1
                if (nameInUse(name)) {
                    STORM_LOG_ERROR("Element with name '" << name << "' already exists.");
                    return false;
                }
                if (storm::utility::isZero<ValueType>(probability)) {
                    return addBasicElementConst(name, false);
                } else if (storm::utility::isOne<ValueType>(probability)) {
                    return addBasicElementConst(name, true);
                }
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Constant probability distribution is not supported for basic element '" << name << "'.");
                return false;
            }

            bool addBasicElementExponential(std::string const& name, ValueType failureRate, ValueType dormancyFactor, bool transient = false) {
                //TODO: collect constraints for SMT solving
                //0 <= dormancyFactor <= 1
                if (nameInUse(name)) {
                    STORM_LOG_ERROR("Element with name '" << name << "' already exists.");
                    return false;
                }
                if (storm::utility::isZero<ValueType>(failureRate)) {
                    return addBasicElementConst(name, false);
                }

                mElements[name] = std::make_shared<storm::storage::BEExponential<ValueType>>(mNextId++, name, failureRate, dormancyFactor, transient);
                return true;
            }

            void addLayoutInfo(std::string const& name, double x, double y) {
                if (!nameInUse(name)) {
                    STORM_LOG_ERROR("Element with name '" << name << "' not found.");
                }
                mLayoutInfo[name] = storm::storage::DFTLayoutInfo(x, y);
            }
            
            bool setTopLevel(std::string const& tle) {
                mTopLevelIdentifier = tle;
                return nameInUse(tle);
            }

            /**
             * Check whether the name is already used.
             * @param name Element name.
             * @return True iff name is already in use.
             */
            bool nameInUse(std::string const& name) {
                return mElements.find(name) != mElements.end();
            }
            
            std::string getUniqueName(std::string name);
            
            storm::storage::DFT<ValueType> build();
            
            /**
             * Copy element and insert it again in the builder.
             *
             * @param element Element to copy.
             */
            void copyElement(DFTElementPointer element);

            /**
             * Copy BE and insert it again in the builder.i
             *
             * @param be BE to copy.
             */
            void copyBE(DFTBEPointer be);

            /**
             * Copy gate with given children and insert it again in the builder. The current children of the element
             * are discarded.
             *
             * @param gate Gate to copy.
             * @param children New children of copied element.
             */
            void copyGate(DFTGatePointer gate, std::vector<std::string> const& children);

        private:
            
            unsigned computeRank(DFTElementPointer const& elem);
            
            bool addStandardGate(std::string const& name, std::vector<std::string> const& children, storm::storage::DFTElementType tp);

            bool addRestriction(std::string const& name, std::vector<std::string> const& children, storm::storage::DFTElementType tp);

            enum class topoSortColour {WHITE, BLACK, GREY}; 
            
            void topoVisit(DFTElementPointer const& n, std::map<DFTElementPointer, topoSortColour, storm::storage::OrderElementsById<ValueType>>& visited, DFTElementVector& L);

            DFTElementVector topoSort();

            std::vector<bool> computeHasDynamicBehavior(DFTElementVector elements);
            
            // If true, the standard gate adders make a pand inclusive, and exclusive otherwise.
            bool pandDefaultInclusive;
            // If true, the standard gate adders make a pand inclusive, and exclusive otherwise.
            bool porDefaultInclusive;
            
        };
    }
}
