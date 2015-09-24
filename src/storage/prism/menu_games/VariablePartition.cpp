#include "src/storage/prism/menu_games/VariablePartition.h"

#include <boost/algorithm/string/join.hpp>

#include "src/utility/macros.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            VariablePartition::VariablePartition(std::set<storm::expressions::Variable> const& relevantVariables, std::vector<storm::expressions::Expression> const& expressions) : relevantVariables(relevantVariables), expressionBlocks(relevantVariables.size()) {
                // Assign each variable to a new block.
                uint_fast64_t currentBlock = 0;
                variableBlocks.resize(relevantVariables.size());
                for (auto const& variable : relevantVariables) {
                    this->variableToBlockMapping[variable] = currentBlock;
                    this->variableToExpressionsMapping[variable] = std::set<uint_fast64_t>();
                    variableBlocks[currentBlock].insert(variable);
                    ++currentBlock;
                }
                
                // Add all expressions, which might relate some variables.
                for (auto const& expression : expressions) {
                    this->addExpression(expression);
                }
            }
            
            bool VariablePartition::addExpression(storm::expressions::Expression const& expression) {
                // Register the expression for all variables that appear in it.
                std::set<storm::expressions::Variable> expressionVariables = expression.getVariables();
                for (auto const& variable : expressionVariables) {
                    variableToExpressionsMapping[variable].insert(this->expressions.size());
                }
                
                // Add the expression to the block of the first variable. When relating the variables, the blocks will
                // get merged (if necessary).
                STORM_LOG_ASSERT(!expressionVariables.empty(), "Found no variables in expression.");
                expressionBlocks[getBlockIndexOfVariable(*expressionVariables.begin())].insert(this->expressions.size());

                // Add expression and relate all the appearing variables.
                this->expressions.push_back(expression);
                return this->relate(expressionVariables);
            }
            
            bool VariablePartition::areRelated(storm::expressions::Variable const& firstVariable, storm::expressions::Variable const& secondVariable) {
                return getBlockIndexOfVariable(firstVariable) == getBlockIndexOfVariable(secondVariable);
            }
            
            bool VariablePartition::relate(storm::expressions::Variable const& firstVariable, storm::expressions::Variable const& secondVariable) {
                return this->relate({firstVariable, secondVariable});
            }
            
            bool VariablePartition::relate(std::set<storm::expressions::Variable> const& variables) {
                // Determine all blocks that need to be merged.
                std::set<uint_fast64_t> blocksToMerge;
                for (auto const& variable : variables) {
                    blocksToMerge.insert(getBlockIndexOfVariable(variable));
                }
                
                STORM_LOG_ASSERT(!blocksToMerge.empty(), "Found no blocks to merge.");
                
                // If we found a single block only, there is nothing to do.
                if (blocksToMerge.size() == 1) {
                    return false;
                }
                
                this->mergeBlocks(blocksToMerge);
                return true;
            }
            
            void VariablePartition::mergeBlocks(std::set<uint_fast64_t> const& blocksToMerge) {
                // Merge all blocks into the block to keep.
                std::vector<std::set<storm::expressions::Variable>> newVariableBlocks;
                std::vector<std::set<uint_fast64_t>> newExpressionBlocks;
                
                std::set<uint_fast64_t>::const_iterator blocksToMergeIt = blocksToMerge.begin();
                std::set<uint_fast64_t>::const_iterator blocksToMergeIte = blocksToMerge.end();

                // Determine which block to keep (to merge the other blocks into).
                uint_fast64_t blockToKeep = *blocksToMergeIt;
                ++blocksToMergeIt;
                
                for (uint_fast64_t blockIndex = 0; blockIndex < variableBlocks.size(); ++blockIndex) {
                    // If the block is the next one to merge into the block to keep, do so now.
                    if (blocksToMergeIt != blocksToMergeIte && *blocksToMergeIt == blockIndex && blockIndex != blockToKeep) {
                        // Adjust the mapping for all variables of the old block.
                        for (auto const& variable : variableBlocks[blockIndex]) {
                            variableToBlockMapping[variable] = blockToKeep;
                        }
                        
                        newVariableBlocks[blockToKeep].insert(variableBlocks[blockIndex].begin(), variableBlocks[blockIndex].end());
                        newExpressionBlocks[blockToKeep].insert(expressionBlocks[blockIndex].begin(), expressionBlocks[blockIndex].end());
                        ++blocksToMergeIt;
                    } else {
                        // Otherwise just move the current block to the new partition.

                        // Adjust the mapping for all variables of the old block.
                        for (auto const& variable : variableBlocks[blockIndex]) {
                            variableToBlockMapping[variable] = newVariableBlocks.size();
                        }
                        
                        newVariableBlocks.emplace_back(std::move(variableBlocks[blockIndex]));
                        newExpressionBlocks.emplace_back(std::move(expressionBlocks[blockIndex]));
                    }
                }
                
                variableBlocks = std::move(newVariableBlocks);
                expressionBlocks = std::move(newExpressionBlocks);
            }
            
            std::set<storm::expressions::Variable> const& VariablePartition::getBlockOfVariable(storm::expressions::Variable const& variable) const {
                return variableBlocks[getBlockIndexOfVariable(variable)];
            }
            
            uint_fast64_t VariablePartition::getNumberOfBlocks() const {
                return this->variableBlocks.size();
            }
            
            std::set<storm::expressions::Variable> const& VariablePartition::getVariableBlockWithIndex(uint_fast64_t blockIndex) const {
                return this->variableBlocks[blockIndex];
            }
            
            uint_fast64_t VariablePartition::getBlockIndexOfVariable(storm::expressions::Variable const& variable) const {
                STORM_LOG_ASSERT(this->relevantVariables.find(variable) != this->relevantVariables.end(), "Illegal variable '" << variable.getName() << "' for partition.");
                return this->variableToBlockMapping.find(variable)->second;
            }
            
            std::set<uint_fast64_t> const& VariablePartition::getRelatedExpressions(storm::expressions::Variable const& variable) const {
                return this->expressionBlocks[getBlockIndexOfVariable(variable)];
            }
            
            std::set<uint_fast64_t> VariablePartition::getRelatedExpressions(std::set<storm::expressions::Variable> const& variables) const {
                // Start by determining the indices of all expression blocks that are related to any of the variables.
                std::set<uint_fast64_t> relatedExpressionBlockIndices;
                for (auto const& variable : variables) {
                    relatedExpressionBlockIndices.insert(getBlockIndexOfVariable(variable));
                }
                
                // Then join the expressions of these blocks and return the result.
                std::set<uint_fast64_t> result;
                for (auto const& blockIndex : relatedExpressionBlockIndices) {
                    result.insert(expressionBlocks[blockIndex].begin(), expressionBlocks[blockIndex].end());
                }
                return result;
            }
            
            std::set<uint_fast64_t> const& VariablePartition::getExpressionsUsingVariable(storm::expressions::Variable const& variable) const {
                STORM_LOG_ASSERT(this->relevantVariables.find(variable) != this->relevantVariables.end(), "Illegal variable '" << variable.getName() << "' for partition.");
                return this->variableToExpressionsMapping.find(variable)->second;
            }
            
            std::set<uint_fast64_t> VariablePartition::getExpressionsUsingVariables(std::set<storm::expressions::Variable> const& variables) const {
                std::set<uint_fast64_t> result;
                
                for (auto const& variable : variables) {
                    STORM_LOG_ASSERT(this->relevantVariables.find(variable) != this->relevantVariables.end(), "Illegal variable '" << variable.getName() << "' for partition.");
                    auto it = this->variableToExpressionsMapping.find(variable);
                    result.insert(it->second.begin(), it->second.end());
                }
                
                return result;
            }
            
            storm::expressions::Expression const& VariablePartition::getExpression(uint_fast64_t expressionIndex) const {
                return this->expressions[expressionIndex];
            }

            std::ostream& operator<<(std::ostream& out, VariablePartition const& partition) {
                std::vector<std::string> blocks;
                for (uint_fast64_t index = 0; index < partition.variableBlocks.size(); ++index) {
                    auto const& variableBlock = partition.variableBlocks[index];
                    auto const& expressionBlock = partition.expressionBlocks[index];

                    std::vector<std::string> variablesInBlock;
                    for (auto const& variable : variableBlock) {
                        variablesInBlock.push_back(variable.getName());
                    }
                    
                    std::vector<std::string> expressionsInBlock;
                    for (auto const& expression : expressionBlock) {
                        std::stringstream stream;
                        stream << partition.expressions[expression];
                        expressionsInBlock.push_back(stream.str());
                    }

                    blocks.push_back("<[" + boost::algorithm::join(variablesInBlock, ", ") + "], [" + boost::algorithm::join(expressionsInBlock, ", ") + "]>");
                }
                
                out << "{";
                out << boost::join(blocks, ", ");
                out << "}";
                return out;
            }
            
        }
    }
}