#include "src/storage/prism/menu_games/VariablePartition.h"

#include "src/utility/macros.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            VariablePartition::VariablePartition(std::set<storm::expressions::Variable> const& relevantVariables, std::vector<storm::expressions::Expression> const& expressions) : relevantVariables(relevantVariables), expressionBlocks(expressions.size()) {
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

                // Add aexpression and relate all the appearing variables.
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
                // Determine which block to keep (to merge the other blocks into).
                uint_fast64_t blockToKeep = *blocksToMerge.begin();
                
                // Merge all blocks into the block to keep.
                std::vector<std::set<storm::expressions::Variable>> newVariableBlocks;
                std::vector<std::set<uint_fast64_t>> newExpressionBlocks;
                for (uint_fast64_t blockIndex = 0; blockIndex < variableBlocks.size(); ++blockIndex) {
                    
                    // If the block is the one into which the others are to be merged, we do so.
                    if (blockIndex == blockToKeep) {
                        for (auto const& blockToMerge : blocksToMerge) {
                            if (blockToMerge == blockToKeep) {
                                continue;
                            }
                            
                            for (auto const& variable : variableBlocks[blockToMerge]) {
                                variableBlocks[blockToKeep].insert(variable);
                            }
                            
                            for (auto const& expression : expressionBlocks[blockToMerge]) {
                                expressionBlocks[blockToKeep].insert(expression);
                            }
                        }
                    }
                    
                    // Adjust the mapping for all variables we are moving to the new block.
                    for (auto const& variable : variableBlocks[blockIndex]) {
                        variableToBlockMapping[variable] = newVariableBlocks.size();
                    }
                    
                    // Move the current block to the new partition.
                    newVariableBlocks.emplace_back(std::move(variableBlocks[blockIndex]));
                    newExpressionBlocks.emplace_back(std::move(expressionBlocks[blockIndex]));
                }
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
        }
    }
}