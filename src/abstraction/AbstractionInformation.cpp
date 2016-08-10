#include "src/abstraction/AbstractionInformation.h"

#include "src/storage/dd/DdManager.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace abstraction {

        template<storm::dd::DdType DdType>
        AbstractionInformation<DdType>::AbstractionInformation(storm::expressions::ExpressionManager& expressionManager, std::shared_ptr<storm::dd::DdManager<DdType>> ddManager) : expressionManager(expressionManager), ddManager(ddManager), allPredicateIdentities(ddManager->getBddOne()) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType>
        void AbstractionInformation<DdType>::addExpressionVariable(storm::expressions::Variable const& variable) {
            variables.insert(variable);
        }
        
        template<storm::dd::DdType DdType>
        void AbstractionInformation<DdType>::addExpressionVariable(storm::expressions::Variable const& variable, storm::expressions::Expression const& constraint) {
            addExpressionVariable(variable);
            addConstraint(constraint);
        }
        
        template<storm::dd::DdType DdType>
        std::set<storm::expressions::Variable> AbstractionInformation<DdType>::getExpressionVariables() const {
            return variables;
        }
        
        template<storm::dd::DdType DdType>
        void AbstractionInformation<DdType>::addConstraint(storm::expressions::Expression const& constraint) {
            constraints.push_back(constraint);
        }
        
        template<storm::dd::DdType DdType>
        uint_fast64_t AbstractionInformation<DdType>::addPredicate(storm::expressions::Expression const& predicate) {
            std::size_t predicateIndex = predicates.size();
            predicateToIndexMap[predicate] = predicateIndex;
            
            // Add the new predicate to the list of known predicates.
            predicates.push_back(predicate);
            
            // Add DD variables for the new predicate.
            std::stringstream stream;
            stream << predicate;
            std::pair<storm::expressions::Variable, storm::expressions::Variable> newMetaVariable = ddManager->addMetaVariable(stream.str());
            
            predicateDdVariables.push_back(newMetaVariable);
            predicateBdds.emplace_back(ddManager->getEncoding(newMetaVariable.first, 1), ddManager->getEncoding(newMetaVariable.second, 1));
            predicateIdentities.push_back(ddManager->getEncoding(newMetaVariable.first, 1).iff(ddManager->getEncoding(newMetaVariable.second, 1)));
            allPredicateIdentities &= predicateIdentities.back();
            sourceVariables.insert(newMetaVariable.first);
            successorVariables.insert(newMetaVariable.second);
            ddVariableIndexToPredicateIndexMap[predicateIdentities.back().getIndex()] = predicateIndex;
            return predicateIndex;
        }

        template<storm::dd::DdType DdType>
        std::vector<uint_fast64_t> AbstractionInformation<DdType>::addPredicates(std::vector<storm::expressions::Expression> const& predicates) {
            std::vector<uint_fast64_t> predicateIndices;
            for (auto const& predicate : predicates) {
                predicateIndices.push_back(this->addPredicate(predicate));
            }
            return predicateIndices;
        }
        
        template<storm::dd::DdType DdType>
        std::vector<storm::expressions::Expression> const& AbstractionInformation<DdType>::getConstraints() const {
            return constraints;
        }
        
        template<storm::dd::DdType DdType>
        storm::expressions::ExpressionManager& AbstractionInformation<DdType>::getExpressionManager() {
            return expressionManager.get();
        }
        
        template<storm::dd::DdType DdType>
        storm::expressions::ExpressionManager const& AbstractionInformation<DdType>::getExpressionManager() const {
            return expressionManager.get();
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::DdManager<DdType>& AbstractionInformation<DdType>::getDdManager() {
            return *ddManager;
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::DdManager<DdType> const& AbstractionInformation<DdType>::getDdManager() const {
            return *ddManager;
        }
        
        template<storm::dd::DdType DdType>
        std::shared_ptr<storm::dd::DdManager<DdType>> AbstractionInformation<DdType>::getDdManagerAsSharedPointer() {
            return ddManager;
        }
        
        template<storm::dd::DdType DdType>
        std::shared_ptr<storm::dd::DdManager<DdType> const> AbstractionInformation<DdType>::getDdManagerAsSharedPointer() const {
            return ddManager;
        }
        
        template<storm::dd::DdType DdType>
        std::vector<storm::expressions::Expression> const& AbstractionInformation<DdType>::getPredicates() const {
            return predicates;
        }
        
        template<storm::dd::DdType DdType>
        storm::expressions::Expression const& AbstractionInformation<DdType>::getPredicateByIndex(uint_fast64_t index) const {
            return predicates[index];
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> AbstractionInformation<DdType>::getPredicateSourceVariable(storm::expressions::Expression const& predicate) const {
            auto indexIt = predicateToIndexMap.find(predicate);
            STORM_LOG_THROW(indexIt != predicateToIndexMap.end(), storm::exceptions::InvalidOperationException, "Cannot retrieve BDD for unknown predicate.");
            return predicateBdds[indexIt->second].first;
        }
        
        template<storm::dd::DdType DdType>
        std::size_t AbstractionInformation<DdType>::getNumberOfPredicates() const {
            return predicates.size();
        }
        
        template<storm::dd::DdType DdType>
        std::set<storm::expressions::Variable> const& AbstractionInformation<DdType>::getVariables() const {
            return variables;
        }
        
        template<storm::dd::DdType DdType>
        void AbstractionInformation<DdType>::createEncodingVariables(uint64_t player1VariableCount, uint64_t player2VariableCount, uint64_t auxVariableCount) {
            STORM_LOG_THROW(player1Variables.empty() && player2Variables.empty() && auxVariables.empty(), storm::exceptions::InvalidOperationException, "Variables have already been created.");
            
            for (uint64_t index = 0; index < player1VariableCount; ++index) {
                storm::expressions::Variable newVariable = ddManager->addMetaVariable("pl1." + std::to_string(index)).first;
                player1Variables.push_back(newVariable);
                player1VariableBdds.push_back(ddManager->getEncoding(newVariable, 1));
            }
            STORM_LOG_DEBUG("Created " << player1VariableCount << " player 1 variables.");

            for (uint64_t index = 0; index < player2VariableCount; ++index) {
                storm::expressions::Variable newVariable = ddManager->addMetaVariable("pl2." + std::to_string(index)).first;
                player2Variables.push_back(newVariable);
                player2VariableBdds.push_back(ddManager->getEncoding(newVariable, 1));
            }
            STORM_LOG_DEBUG("Created " << player2VariableCount << " player 2 variables.");

            for (uint64_t index = 0; index < auxVariableCount; ++index) {
                storm::expressions::Variable newVariable = ddManager->addMetaVariable("aux_" + std::to_string(index)).first;
                auxVariables.push_back(newVariable);
                auxVariableBdds.push_back(ddManager->getEncoding(newVariable, 1));
            }
            STORM_LOG_DEBUG("Created " << auxVariableCount << " auxiliary variables.");
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> AbstractionInformation<DdType>::encodePlayer1Choice(uint_fast64_t index, uint_fast64_t numberOfVariables) const {
            return encodeChoice(index, 0, numberOfVariables, player1VariableBdds);
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> AbstractionInformation<DdType>::encodePlayer2Choice(uint_fast64_t index, uint_fast64_t numberOfVariables) const {
            return encodeChoice(index, 0, numberOfVariables, player2VariableBdds);
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> AbstractionInformation<DdType>::encodeAux(uint_fast64_t index, uint_fast64_t offset, uint_fast64_t numberOfVariables) const {
            return encodeChoice(index, offset, numberOfVariables, auxVariableBdds);
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> AbstractionInformation<DdType>::getPlayer2ZeroCube(uint_fast64_t numberOfVariables, uint_fast64_t offset) const {
            storm::dd::Bdd<DdType> result = ddManager->getBddOne();
            for (uint_fast64_t index = offset; index < numberOfVariables; ++index) {
                result &= player2VariableBdds[index];
            }
            STORM_LOG_ASSERT(!result.isZero(), "Zero cube must not be zero.");
            return result;
        }
        
        template<storm::dd::DdType DdType>
        std::vector<storm::expressions::Variable> const& AbstractionInformation<DdType>::getPlayer1Variables() const {
            return player1Variables;
        }
        
        template<storm::dd::DdType DdType>
        std::set<storm::expressions::Variable> AbstractionInformation<DdType>::getPlayer1VariableSet(uint_fast64_t count) const {
            return std::set<storm::expressions::Variable>(player1Variables.begin(), player1Variables.begin() + count);
        }
        
        template<storm::dd::DdType DdType>
        std::vector<storm::expressions::Variable> const& AbstractionInformation<DdType>::getPlayer2Variables() const {
            return player2Variables;
        }
        
        template<storm::dd::DdType DdType>
        std::set<storm::expressions::Variable> AbstractionInformation<DdType>::getPlayer2VariableSet(uint_fast64_t count) const {
            return std::set<storm::expressions::Variable>(player2Variables.begin(), player2Variables.begin() + count);
        }
        
        template<storm::dd::DdType DdType>
        std::vector<storm::expressions::Variable> const& AbstractionInformation<DdType>::getAuxVariables() const {
            return auxVariables;
        }
        
        template<storm::dd::DdType DdType>
        std::set<storm::expressions::Variable> AbstractionInformation<DdType>::getAuxVariableSet(uint_fast64_t offset, uint_fast64_t count) const {
            return std::set<storm::expressions::Variable>(auxVariables.begin() + offset, auxVariables.begin() + offset + count);
        }
        
        template<storm::dd::DdType DdType>
        std::set<storm::expressions::Variable> const& AbstractionInformation<DdType>::getSourceVariables() const {
            return sourceVariables;
        }
        
        template<storm::dd::DdType DdType>
        std::set<storm::expressions::Variable> const& AbstractionInformation<DdType>::getSuccessorVariables() const {
            return successorVariables;
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> const& AbstractionInformation<DdType>::getAllPredicateIdentities() const {
            return allPredicateIdentities;
        }
        
        template<storm::dd::DdType DdType>
        std::size_t AbstractionInformation<DdType>::getPlayer1VariableCount() const {
            return player1Variables.size();
        }
        
        template<storm::dd::DdType DdType>
        std::size_t AbstractionInformation<DdType>::getPlayer2VariableCount() const {
            return player2Variables.size();
        }
        
        template<storm::dd::DdType DdType>
        std::size_t AbstractionInformation<DdType>::getAuxVariableCount() const {
            return auxVariables.size();
        }
        
        template<storm::dd::DdType DdType>
        std::map<storm::expressions::Expression, storm::dd::Bdd<DdType>> AbstractionInformation<DdType>::getPredicateToBddMap() const {
            std::map<storm::expressions::Expression, storm::dd::Bdd<DdType>> result;
            
            for (uint_fast64_t index = 0; index < predicates.size(); ++index) {
                result[predicates[index]] = predicateBdds[index].first;
            }
            
            return result;
        }
        
        template<storm::dd::DdType DdType>
        std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& AbstractionInformation<DdType>::getSourceSuccessorVariablePairs() const {
            return predicateDdVariables;
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> const& AbstractionInformation<DdType>::encodePredicateAsSource(uint_fast64_t predicateIndex) const {
            return predicateBdds[predicateIndex].first;
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> const& AbstractionInformation<DdType>::encodePredicateAsSuccessor(uint_fast64_t predicateIndex) const {
            return predicateBdds[predicateIndex].second;
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> const& AbstractionInformation<DdType>::getPredicateIdentity(uint_fast64_t predicateIndex) const {
            return predicateIdentities[predicateIndex];
        }

        template<storm::dd::DdType DdType>
        storm::expressions::Expression const& AbstractionInformation<DdType>::getPredicateForDdVariableIndex(uint_fast64_t ddVariableIndex) const {
            auto indexIt = ddVariableIndexToPredicateIndexMap.find(ddVariableIndex);
            STORM_LOG_THROW(indexIt != ddVariableIndexToPredicateIndexMap.end(), storm::exceptions::InvalidOperationException, "Unknown DD variable index.");
            return predicates[indexIt->second];
        }
        
        template <storm::dd::DdType DdType>
        std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> AbstractionInformation<DdType>::declareNewVariables(std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> const& oldPredicates, std::set<uint_fast64_t> const& newPredicates) const {
            std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> result;
            
            auto oldIt = oldPredicates.begin();
            auto oldIte = oldPredicates.end();
            auto newIt = newPredicates.begin();
            auto newIte = newPredicates.end();
            
            for (; newIt != newIte; ++newIt) {
                if (oldIt == oldIte || oldIt->second != *newIt) {
                    result.push_back(std::make_pair(expressionManager.get().declareFreshBooleanVariable(), *newIt));
                } else {
                    ++oldIt;
                }
            }
            
            return result;
        }
        
        template<storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> AbstractionInformation<DdType>::encodeChoice(uint_fast64_t index, uint_fast64_t offset, uint_fast64_t numberOfVariables, std::vector<storm::dd::Bdd<DdType>> const& variables) const {
            storm::dd::Bdd<DdType> result = ddManager->getBddOne();
            numberOfVariables += offset;
            for (uint_fast64_t bitIndex = numberOfVariables; bitIndex > offset; --bitIndex) {
                if ((index & 1) != 0) {
                    result &= variables[bitIndex - 1];
                } else {
                    result &= !variables[bitIndex - 1];
                }
                index >>= 1;
            }
            STORM_LOG_ASSERT(!result.isZero(), "BDD encoding must not be zero.");
            return result;
        }
        
        template class AbstractionInformation<storm::dd::DdType::CUDD>;
        template class AbstractionInformation<storm::dd::DdType::Sylvan>;
    }
}