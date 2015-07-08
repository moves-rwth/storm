
#ifndef DFTBUILDER_H
#define	DFTBUILDER_H

#include "DFTElements.h"
#include <iostream>
#include <unordered_map>
#include <map>

namespace storm {
    namespace storage {
        class DFT;
        
        class DFTBuilder {
            
            std::size_t mNextId = 0;
            std::string topLevelIdentifier;
            std::unordered_map<std::string, std::shared_ptr<DFTElement>> mElements;
            std::unordered_map<std::shared_ptr<DFTElement>, std::vector<std::string>> mChildNames;
            
        public:
            DFTBuilder() {
                
            }
            
            bool addAndElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementTypes::AND);
            }
            
            bool addOrElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementTypes::OR);
            }
            
            bool addPandElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementTypes::PAND);
            }
            
            bool addPorElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementTypes::POR);
            }
            
            bool addSpareElement(std::string const& name, std::vector<std::string> const& children) {
                return addStandardGate(name, children, DFTElementTypes::SPARE);
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
                std::shared_ptr<DFTElement> element = std::make_shared<DFTVot>(mNextId++, name, threshold);
                
                mElements[name] = element;
                mChildNames[element] = children;
                return true;
            }
            
            bool addBasicElement(std::string const& name, double failureRate, double dormancyFactor) {
                if(failureRate <= 0.0) {
                    std::cerr << "Failure rate must be positive." << std::endl;
                    return false;
                }
                
                if(dormancyFactor < 0.0 || dormancyFactor > 1.0) {
                    std::cerr << "Dormancy factor must be between 0 and 1." << std::endl;
                    return false;
                }
                
                mElements[name] = std::make_shared<DFTBE<double>>(mNextId++, name, failureRate, dormancyFactor);
                return true;
            }
            
            bool setTopLevel(std::string const& tle) {
                topLevelIdentifier = tle;
                return mElements.count(tle) > 0;
            }
            
            DFT build();

         
        private:
            
            unsigned computeRank(std::shared_ptr<DFTElement> const& elem);
            
            bool addStandardGate(std::string const& name, std::vector<std::string> const& children, DFTElementTypes tp);
            
            enum class topoSortColour {WHITE, BLACK, GREY}; 
            
            void topoVisit(std::shared_ptr<DFTElement> const& n, std::map<std::shared_ptr<DFTElement>, topoSortColour>& visited, std::vector<std::shared_ptr<DFTElement>>& L);
            std::vector<std::shared_ptr<DFTElement>> topoSort();

            
        };
    }
}



#endif	/* DFTBUILDER_H */

