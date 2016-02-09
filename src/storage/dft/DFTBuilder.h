
#ifndef DFTBUILDER_H
#define	DFTBUILDER_H

#include "DFTElements.h"
#include <iostream>
#include <unordered_map>
#include <map>

namespace storm {
    namespace storage {
        template<typename ValueType>
        class DFT;

        template<typename ValueType>
        class DFTBuilder {

            using DFTElementPointer = std::shared_ptr<DFTElement<ValueType>>;
            using DFTElementVector = std::vector<DFTElementPointer>;
            using DFTGatePointer = std::shared_ptr<DFTGate<ValueType>>;
            using DFTGateVector = std::vector<DFTGatePointer>;
            using DFTDependencyPointer = std::shared_ptr<DFTDependency<ValueType>>;

        private:
            std::size_t mNextId = 0;
            std::string topLevelIdentifier;
            std::unordered_map<std::string, DFTElementPointer> mElements;
            std::unordered_map<DFTElementPointer, std::vector<std::string>> mChildNames;
            std::vector<DFTDependencyPointer> mDependencies;
            
        public:
            DFTBuilder() {
                
            }
            
            bool addAndElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementType::AND);
            }
            
            bool addOrElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementType::OR);
            }
            
            bool addPandElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementType::PAND);
            }
            
            bool addPorElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementType::POR);
            }
            
            bool addSpareElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementType::SPARE);
            }
            
            bool addDepElement(std::string const& name, std::vector<std::string> const& children, ValueType probability) {
                assert(children.size() > 1);
                if(mElements.count(name) != 0) {
                    // Element with that name already exists.
                    return false;
                }

                if (storm::utility::isZero(probability)) {
                    // Element is superfluous
                    return true;
                }
                std::string trigger = children[0];

                //TODO Matthias: collect constraints for SMT solving
                //0 <= probability <= 1
                if (!storm::utility::isOne(probability) && children.size() > 2) {
                    //TODO Matthias: introduce additional element for probability and then add pdeps with probability 1 to children
                    std::cerr << "Probability != 1 for more than one child currently not supported." << std::endl;
                    return false;
                }

                for (size_t i = 1; i < children.size(); ++i) {
                    std::string nameDep = name + "_" + std::to_string(i);
                    if(mElements.count(nameDep) != 0) {
                        // Element with that name already exists.
                        std::cerr << "Element with name: " << nameDep << " already exists." << std::endl;
                        return false;
                    }
                    assert(storm::utility::isOne(probability) || children.size() == 2);
                    DFTDependencyPointer element = std::make_shared<DFTDependency<ValueType>>(mNextId++, nameDep, trigger, children[i], probability);
                    mElements[element->name()] = element;
                    mDependencies.push_back(element);
                }
                return true;
            }

            bool addVotElement(std::string const& name, unsigned threshold, std::vector<std::string> const& children) {
                assert(children.size() > 0);
                if(mElements.count(name) != 0) {
                    std::cerr << "Element with name: " << name << " already exists." << std::endl;
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
                    std::cerr << "Voting gates with threshold higher than the number of children is not supported." << std::endl;
                    return false;
                }
                DFTElementPointer element = std::make_shared<DFTVot<ValueType>>(mNextId++, name, threshold);
                
                mElements[name] = element;
                mChildNames[element] = children;
                return true;
            }
            
            bool addBasicElement(std::string const& name, ValueType failureRate, ValueType dormancyFactor) {
                //TODO Matthias: collect constraints for SMT solving
                //failureRate > 0
                //0 <= dormancyFactor <= 1

                mElements[name] = std::make_shared<DFTBE<ValueType>>(mNextId++, name, failureRate, dormancyFactor);
                return true;
            }
            
            bool setTopLevel(std::string const& tle) {
                topLevelIdentifier = tle;
                return mElements.count(tle) > 0;
            }
            
            DFT<ValueType> build();

         
        private:
            
            unsigned computeRank(DFTElementPointer const& elem);
            
            bool addStandardGate(std::string const& name, std::vector<std::string> const& children, DFTElementType tp);
            
            enum class topoSortColour {WHITE, BLACK, GREY}; 
            
            void topoVisit(DFTElementPointer const& n, std::map<DFTElementPointer, topoSortColour>& visited, DFTElementVector& L);

            DFTElementVector topoSort();
            
        };
    }
}



#endif	/* DFTBUILDER_H */

