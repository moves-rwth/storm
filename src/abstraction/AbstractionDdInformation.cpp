#include "src/abstraction/AbstractionDdInformation.h"

#include <sstream>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/Expression.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Bdd.h"
#include "src/storage/dd/Add.h"

#include "src/utility/macros.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType DdType, typename ValueType>
        AbstractionDdInformation<DdType, ValueType>::AbstractionDdInformation(std::shared_ptr<storm::dd::DdManager<DdType>> const& manager, std::vector<storm::expressions::Expression> const& initialPredicates) : manager(manager), allPredicateIdentities(manager->getBddOne()), bddVariableIndexToPredicateMap() {
            for (auto const& predicate : initialPredicates) {
                this->addPredicate(predicate);
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        storm::dd::Bdd<DdType> AbstractionDdInformation<DdType, ValueType>::encodeDistributionIndex(uint_fast64_t numberOfVariables, uint_fast64_t distributionIndex) const {
            storm::dd::Bdd<DdType> result = manager->getBddOne();
            for (uint_fast64_t bitIndex = 0; bitIndex < numberOfVariables; ++bitIndex) {
                STORM_LOG_ASSERT(!(optionDdVariables[bitIndex].second.isZero() || optionDdVariables[bitIndex].second.isOne()), "Option variable is corrupted.");
                if ((distributionIndex & 1) != 0) {
                    result &= optionDdVariables[bitIndex].second;
                } else {
                    result &= !optionDdVariables[bitIndex].second;
                }
                distributionIndex >>= 1;
            }
            STORM_LOG_ASSERT(!result.isZero(), "Update BDD encoding must not be zero.");
            return result;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void AbstractionDdInformation<DdType, ValueType>::addPredicate(storm::expressions::Expression const& predicate) {
            std::stringstream stream;
            stream << predicate;
            std::pair<storm::expressions::Variable, storm::expressions::Variable> newMetaVariable;
            
            // Create the new predicate variable below all other predicate variables.
            if (predicateDdVariables.empty()) {
                newMetaVariable = manager->addMetaVariable(stream.str());
            } else {
                newMetaVariable = manager->addMetaVariable(stream.str(), std::make_pair(storm::dd::MetaVariablePosition::Below, predicateDdVariables.back().second));
            }
            
            predicateDdVariables.push_back(newMetaVariable);
            predicateBdds.emplace_back(manager->getEncoding(newMetaVariable.first, 1), manager->getEncoding(newMetaVariable.second, 1));
            predicateIdentities.push_back(manager->getEncoding(newMetaVariable.first, 1).iff(manager->getEncoding(newMetaVariable.second, 1)));
            allPredicateIdentities &= predicateIdentities.back();
            sourceVariables.insert(newMetaVariable.first);
            successorVariables.insert(newMetaVariable.second);
            expressionToBddMap[predicate] = predicateBdds.back().first;
            bddVariableIndexToPredicateMap[predicateIdentities.back().getIndex()] = predicate;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        storm::dd::Bdd<DdType> AbstractionDdInformation<DdType, ValueType>::getMissingOptionVariableCube(uint_fast64_t begin, uint_fast64_t end) const {
            storm::dd::Bdd<DdType> result = manager->getBddOne();
            
            for (uint_fast64_t index = begin; index < end; ++index) {
                result &= optionDdVariables[index].second;
            }
            
            STORM_LOG_ASSERT(!result.isZero(), "Update variable cube must not be zero.");
            
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
                } else {
                    ++oldIt;
                }
            }
            
            return result;
        }
        
        template struct AbstractionDdInformation<storm::dd::DdType::CUDD, double>;
        
    }
}