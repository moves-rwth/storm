#include "storm/storage/dd/Odd.h"

#include <set>
#include <fstream>
#include <boost/algorithm/string/join.hpp>

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/file.h"

#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace dd {
        Odd::Odd(std::shared_ptr<Odd> elseNode, uint_fast64_t elseOffset, std::shared_ptr<Odd> thenNode, uint_fast64_t thenOffset) : elseNode(elseNode), thenNode(thenNode), elseOffset(elseOffset), thenOffset(thenOffset) {
            // Intentionally left empty.
        }
        
        Odd const& Odd::getThenSuccessor() const {
            return *this->thenNode;
        }
        
        Odd const& Odd::getElseSuccessor() const {
            return *this->elseNode;
        }
        
        uint_fast64_t Odd::getElseOffset() const {
            return this->elseOffset;
        }
        
        void Odd::setElseOffset(uint_fast64_t newOffset) {
            this->elseOffset = newOffset;
        }
        
        uint_fast64_t Odd::getThenOffset() const {
            return this->thenOffset;
        }
        
        void Odd::setThenOffset(uint_fast64_t newOffset) {
            this->thenOffset = newOffset;
        }
        
        uint_fast64_t Odd::getTotalOffset() const {
            return this->elseOffset + this->thenOffset;
        }
        
        uint_fast64_t Odd::getNodeCount() const {
            // If the ODD contains a constant (and thus has no children), the size is 1.
            if (this->elseNode == nullptr && this->thenNode == nullptr) {
                return 1;
            }
            
            // If the two successors are actually the same, we need to count the subnodes only once.
            if (this->elseNode == this->thenNode) {
                return this->elseNode->getNodeCount();
            } else {
                return this->elseNode->getNodeCount() + this->thenNode->getNodeCount();
            }
        }
        
        uint_fast64_t Odd::getHeight() const {
            if (this->isTerminalNode()) {
                return 1;
            } else {
                // Since both subtrees have the same height, we only count the height of the else-tree.
                return 1 + this->getElseSuccessor().getHeight();
            }
        }
        
        bool Odd::isTerminalNode() const {
            return this->elseNode == nullptr && this->thenNode == nullptr;
        }
        
        template <typename ValueType>
        void Odd::expandExplicitVector(storm::dd::Odd const& newOdd, std::vector<ValueType> const& oldValues, std::vector<ValueType>& newValues) const {
            expandValuesToVectorRec(0, *this, oldValues, 0, newOdd, newValues);
        }
        
        template <typename ValueType>
        void Odd::expandValuesToVectorRec(uint_fast64_t oldOffset, storm::dd::Odd const& oldOdd, std::vector<ValueType> const& oldValues, uint_fast64_t newOffset, storm::dd::Odd const& newOdd, std::vector<ValueType>& newValues) {
            if (oldOdd.isTerminalNode()) {
                STORM_LOG_THROW(newOdd.isTerminalNode(), storm::exceptions::InvalidArgumentException, "The ODDs for the translation must have the same height.");
                if (oldOdd.getThenOffset() != 0) {
                    newValues[newOffset] += oldValues[oldOffset];
                }
            } else {
                expandValuesToVectorRec(oldOffset, oldOdd.getElseSuccessor(), oldValues, newOffset, newOdd.getElseSuccessor(), newValues);
                expandValuesToVectorRec(oldOffset + oldOdd.getElseOffset(), oldOdd.getThenSuccessor(), oldValues, newOffset + newOdd.getElseOffset(), newOdd.getThenSuccessor(), newValues);
            }
        }
        
        void Odd::exportToDot(std::string const& filename) const {
            std::ofstream dotFile;
            storm::utility::openFile(filename, dotFile);
            
            // Print header.
            dotFile << "digraph \"ODD\" {" << std::endl << "center=true;" << std::endl << "edge [dir = none];" << std::endl;
            
            // Print levels as ranks.
            dotFile << "{ node [shape = plaintext];" << std::endl << "edge [style = invis];" << std::endl;
            std::vector<std::string> levelNames;
            for (uint_fast64_t level = 0; level < this->getHeight(); ++level) {
                levelNames.push_back("\"" + std::to_string(level) + "\"");
            }
            dotFile << boost::join(levelNames, " -> ") << ";";
            dotFile << "}" << std::endl;
            
            std::map<uint_fast64_t, std::vector<std::reference_wrapper<storm::dd::Odd const>>> levelToOddNodesMap;
            this->addToLevelToOddNodesMap(levelToOddNodesMap);
            
            for (auto const& levelNodes : levelToOddNodesMap) {
                dotFile << "{ rank = same; \"" << levelNodes.first << "\"" << std::endl;;
                for (auto const& node : levelNodes.second) {
                    dotFile << "\"" << &node.get() << "\";" << std::endl;
                }
                dotFile << "}" << std::endl;
            }
            
            std::set<storm::dd::Odd const*> printedNodes;
            for (auto const& levelNodes : levelToOddNodesMap) {
                for (auto const& node : levelNodes.second) {
                    if (printedNodes.find(&node.get()) != printedNodes.end()) {
                        continue;
                    } else {
                        printedNodes.insert(&node.get());
                    }
                    
                    dotFile << "\"" << &node.get() << "\" [label=\"" << levelNodes.first << "\"];" << std::endl;
                    if (!node.get().isTerminalNode()) {
                        dotFile << "\"" << &node.get() << "\" -> \"" << &node.get().getElseSuccessor() << "\" [style=dashed, label=\"0\"];" << std::endl;
                        dotFile << "\"" << &node.get() << "\" -> \"" << &node.get().getThenSuccessor() << "\" [style=solid, label=\"" << node.get().getElseOffset() << "\"];" << std::endl;
                    }
                }
            }
            
            dotFile << "}" << std::endl;
            storm::utility::closeFile(dotFile);
        }
        
        void Odd::addToLevelToOddNodesMap(std::map<uint_fast64_t, std::vector<std::reference_wrapper<storm::dd::Odd const>>>& levelToOddNodesMap, uint_fast64_t level) const {
            levelToOddNodesMap[level].push_back(*this);
            if (!this->isTerminalNode()) {
                this->getElseSuccessor().addToLevelToOddNodesMap(levelToOddNodesMap, level + 1);
                this->getThenSuccessor().addToLevelToOddNodesMap(levelToOddNodesMap, level + 1);
            }
        }
        
        template void Odd::expandExplicitVector(storm::dd::Odd const& newOdd, std::vector<double> const& oldValues, std::vector<double>& newValues) const;
        template void Odd::expandExplicitVector(storm::dd::Odd const& newOdd, std::vector<storm::RationalNumber> const& oldValues, std::vector<storm::RationalNumber>& newValues) const;
        template void Odd::expandExplicitVector(storm::dd::Odd const& newOdd, std::vector<storm::RationalFunction> const& oldValues, std::vector<storm::RationalFunction>& newValues) const;
    }
}
