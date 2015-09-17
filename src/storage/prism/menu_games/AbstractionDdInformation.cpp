#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include <sstream>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/Expression.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddAdd.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionDdInformation<DdType, ValueType>::AbstractionDdInformation(std::shared_ptr<storm::dd::DdManager<DdType>> const& manager) : manager(manager), allPredicateIdentities(manager->getBddOne()) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractionDdInformation<DdType, ValueType>::encodeDistributionIndex(uint_fast64_t numberOfVariables, uint_fast64_t distributionIndex) const {
                storm::dd::Bdd<DdType> result = manager->getBddOne();
                for (uint_fast64_t bitIndex = 0; bitIndex < numberOfVariables; ++bitIndex) {
                    if ((distributionIndex & 1) != 0) {
                        result &= optionDdVariables[bitIndex].second;
                    } else {
                        result &= !optionDdVariables[bitIndex].second;
                    }
                    distributionIndex >>= 1;
                }
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractionDdInformation<DdType, ValueType>::addPredicate(storm::expressions::Expression const& predicate) {
                std::stringstream stream;
                stream << predicate;
                std::pair<storm::expressions::Variable, storm::expressions::Variable> newMetaVariable = manager->addMetaVariable(stream.str());
                predicateDdVariables.push_back(newMetaVariable);
                predicateBdds.emplace_back(manager->getEncoding(newMetaVariable.first, 1), manager->getEncoding(newMetaVariable.second, 1));
                predicateIdentities.push_back(manager->getIdentity(newMetaVariable.first).equals(manager->getIdentity(newMetaVariable.second)).toBdd());
                allPredicateIdentities &= predicateIdentities.back();
                sourceVariables.insert(newMetaVariable.first);
                successorVariables.insert(newMetaVariable.second);
            }
         
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractionDdInformation<DdType, ValueType>::getMissingOptionVariableCube(uint_fast64_t lastUsed, uint_fast64_t lastToBe) const {
                storm::dd::Bdd<DdType> result = manager->getBddOne();
                
                for (uint_fast64_t index = lastUsed + 1; index <= lastToBe; ++index) {
                    result &= optionDdVariables[index].second;
                }
                
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> AbstractionDdInformation<DdType, ValueType>::declareNewVariables(storm::expressions::ExpressionManager& manager, std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> const& oldRelevantPredicates, std::set<uint_fast64_t> const& newRelevantPredicates) {
                std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> result;
                
                auto oldIt = oldRelevantPredicates.begin();
                auto oldIte = oldRelevantPredicates.end();
                for (auto newIt = newRelevantPredicates.begin(), newIte = newRelevantPredicates.end(); newIt != newIte; ++newIt) {
                    // If the new variable does not yet exist as a source variable, we create it now.
                    if (oldIt == oldIte || oldIt->second != *newIt) {
                        result.push_back(std::make_pair(manager.declareFreshBooleanVariable(), *newIt));
                    }
                }
                
                return result;
            }
            
            template struct AbstractionDdInformation<storm::dd::DdType::CUDD, double>;
            
        }
    }
}