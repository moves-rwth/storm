#include "src/storage/expressions/ExpressionManager.h"

#include "src/storage/expressions/Expressions.h"
#include "src/storage/expressions/Variable.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace expressions {

        VariableIterator::VariableIterator(ExpressionManager const& manager, std::unordered_map<std::string, uint_fast64_t>::const_iterator nameIndexIterator, std::unordered_map<std::string, uint_fast64_t>::const_iterator nameIndexIteratorEnd, VariableSelection const& selection) : manager(manager), nameIndexIterator(nameIndexIterator), nameIndexIteratorEnd(nameIndexIteratorEnd), selection(selection) {
            moveUntilNextSelectedElement(false);
        }
        
        bool VariableIterator::operator==(VariableIterator const& other) {
            return this->nameIndexIterator == other.nameIndexIterator;
        }
        
        bool VariableIterator::operator!=(VariableIterator const& other) {
            return !(*this == other);
        }
        
        VariableIterator::value_type& VariableIterator::operator*() {
            return currentElement;
        }
        
        VariableIterator& VariableIterator::operator++(int) {
            moveUntilNextSelectedElement();
            return *this;
        }
        
        VariableIterator& VariableIterator::operator++() {
            moveUntilNextSelectedElement();
            return *this;
        }
        
        void VariableIterator::moveUntilNextSelectedElement(bool atLeastOneStep) {
            if (atLeastOneStep && nameIndexIterator != nameIndexIteratorEnd) {
                ++nameIndexIterator;
            }
            
            // Move the underlying iterator forward until a variable matches the selection.
            while (nameIndexIterator != nameIndexIteratorEnd
                   && (selection == VariableSelection::OnlyRegularVariables && (nameIndexIterator->second & ExpressionManager::auxiliaryMask) != 0)
                   && (selection == VariableSelection::OnlyAuxiliaryVariables && (nameIndexIterator->second & ExpressionManager::auxiliaryMask) == 0)) {
                ++nameIndexIterator;
            }
            
            ExpressionReturnType type = ExpressionReturnType::Undefined;
            if ((nameIndexIterator->second & ExpressionManager::booleanMask) != 0) {
                type = ExpressionReturnType::Bool;
            } else if ((nameIndexIterator->second & ExpressionManager::integerMask) != 0) {
                type = ExpressionReturnType::Int;
            } else if ((nameIndexIterator->second & ExpressionManager::rationalMask) != 0) {
                type = ExpressionReturnType::Double;
            }
            
            if (nameIndexIterator != nameIndexIteratorEnd) {
                currentElement = std::make_pair(Variable(manager, nameIndexIterator->second), type);
            }
        }
        
        ExpressionManager::ExpressionManager() : nameToIndexMapping(), variableTypeToCountMapping(), auxiliaryVariableTypeToCountMapping() {
            variableTypeToCountMapping[static_cast<std::size_t>(storm::expressions::ExpressionReturnType::Bool)] = 0;
            variableTypeToCountMapping[static_cast<std::size_t>(storm::expressions::ExpressionReturnType::Int)] = 0;
            variableTypeToCountMapping[static_cast<std::size_t>(storm::expressions::ExpressionReturnType::Double)] = 0;
            auxiliaryVariableTypeToCountMapping[static_cast<std::size_t>(storm::expressions::ExpressionReturnType::Bool)] = 0;
            auxiliaryVariableTypeToCountMapping[static_cast<std::size_t>(storm::expressions::ExpressionReturnType::Int)] = 0;
            auxiliaryVariableTypeToCountMapping[static_cast<std::size_t>(storm::expressions::ExpressionReturnType::Double)] = 0;
        }
        
        Expression ExpressionManager::boolean(bool value) const {
            return Expression(std::shared_ptr<BaseExpression const>(new BooleanLiteralExpression(*this, value)));
        }

        Expression ExpressionManager::integer(int_fast64_t value) const {
            return Expression(std::shared_ptr<BaseExpression const>(new IntegerLiteralExpression(*this, value)));
        }

        Expression ExpressionManager::rational(double value) const {
            return Expression(std::shared_ptr<BaseExpression const>(new DoubleLiteralExpression(*this, value)));
        }
        
        bool ExpressionManager::operator==(ExpressionManager const& other) const {
            return this == &other;
        }
        
        bool ExpressionManager::isValidVariableName(std::string const& name) {
            return name.size() < 2 || name.at(0) != '_' || name.at(1) != '_';
        }

        bool ExpressionManager::variableExists(std::string const& name) const {
            auto nameIndexPair = nameToIndexMapping.find(name);
            return nameIndexPair != nameToIndexMapping.end();
        }
        
        Variable ExpressionManager::declareVariable(std::string const& name, storm::expressions::ExpressionReturnType const& variableType) {
            STORM_LOG_THROW(!variableExists(name), storm::exceptions::InvalidArgumentException, "Variable with name '" << name << "' already exists.");
            return declareOrGetVariable(name, variableType);
        }

        Variable ExpressionManager::declareAuxiliaryVariable(std::string const& name, storm::expressions::ExpressionReturnType const& variableType) {
            STORM_LOG_THROW(!variableExists(name), storm::exceptions::InvalidArgumentException, "Variable with name '" << name << "' already exists.");
            return declareOrGetAuxiliaryVariable(name, variableType);
        }

        Variable ExpressionManager::declareOrGetVariable(std::string const& name, storm::expressions::ExpressionReturnType const& variableType) {
            STORM_LOG_THROW(isValidVariableName(name), storm::exceptions::InvalidArgumentException, "Invalid variable name '" << name << "'.");
            uint_fast64_t newIndex = 0;
            switch (variableType) {
                case ExpressionReturnType::Bool:
                    newIndex = variableTypeToCountMapping[static_cast<std::size_t>(ExpressionReturnType::Bool)]++ | booleanMask;
                    break;
                case ExpressionReturnType::Int:
                    newIndex = variableTypeToCountMapping[static_cast<std::size_t>(ExpressionReturnType::Int)]++ | integerMask;
                    break;
                case ExpressionReturnType::Double:
                    newIndex = variableTypeToCountMapping[static_cast<std::size_t>(ExpressionReturnType::Double)]++ | rationalMask;
                    break;
                case ExpressionReturnType::Undefined:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal variable type.");
            }
            
            nameToIndexMapping[name] = newIndex;
            indexToNameMapping[newIndex] = name;
            return Variable(*this, newIndex);
        }

        Variable ExpressionManager::declareOrGetAuxiliaryVariable(std::string const& name, storm::expressions::ExpressionReturnType const& variableType) {
            auto nameIndexPair = nameToIndexMapping.find(name);
            if (nameIndexPair != nameToIndexMapping.end()) {
                return Variable(*this, nameIndexPair->second);
            } else {
                STORM_LOG_THROW(isValidVariableName(name), storm::exceptions::InvalidArgumentException, "Invalid variable name '" << name << "'.");
                uint_fast64_t newIndex = auxiliaryMask;
                switch (variableType) {
                    case ExpressionReturnType::Bool:
                        newIndex |= auxiliaryVariableTypeToCountMapping[static_cast<std::size_t>(ExpressionReturnType::Bool)]++ | booleanMask;
                        break;
                    case ExpressionReturnType::Int:
                        newIndex |= auxiliaryVariableTypeToCountMapping[static_cast<std::size_t>(ExpressionReturnType::Int)]++ | integerMask;
                        break;
                    case ExpressionReturnType::Double:
                        newIndex |= auxiliaryVariableTypeToCountMapping[static_cast<std::size_t>(ExpressionReturnType::Double)]++ | rationalMask;
                        break;
                    case ExpressionReturnType::Undefined:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal variable type.");
                }
                
                nameToIndexMapping[name] = newIndex;
                indexToNameMapping[newIndex] = name;
                return Variable(*this, newIndex);
            }
        }

        Variable ExpressionManager::getVariable(std::string const& name) const {
            auto nameIndexPair = nameToIndexMapping.find(name);
            STORM_LOG_THROW(nameIndexPair != nameToIndexMapping.end(),  storm::exceptions::InvalidArgumentException, "Unknown variable '" << name << "'.");
            return Variable(*this, nameIndexPair->second);
        }
        
        Expression ExpressionManager::getVariableExpression(std::string const& name) const {
            return Expression(getVariable(name));
        }

        Variable ExpressionManager::declareFreshVariable(storm::expressions::ExpressionReturnType const& variableType) {
            std::string newName = "__x" + std::to_string(freshVariableCounter++);
            return declareVariable(newName, variableType);
        }

        Variable ExpressionManager::declareFreshAuxiliaryVariable(storm::expressions::ExpressionReturnType const& variableType) {
            std::string newName = "__x" + std::to_string(freshVariableCounter++);
            return declareAuxiliaryVariable(newName, variableType);
        }

        uint_fast64_t ExpressionManager::getNumberOfVariables(storm::expressions::ExpressionReturnType const& variableType) const {
            return variableTypeToCountMapping[static_cast<std::size_t>(variableType)];
        }
        
        uint_fast64_t ExpressionManager::getNumberOfVariables() const {
            return numberOfVariables;
        }
        
        uint_fast64_t ExpressionManager::getNumberOfBooleanVariables() const {
            return getNumberOfVariables(storm::expressions::ExpressionReturnType::Bool);
        }
        
        uint_fast64_t ExpressionManager::getNumberOfIntegerVariables() const {
            return getNumberOfVariables(storm::expressions::ExpressionReturnType::Int);
        }
        
        uint_fast64_t ExpressionManager::getNumberOfRationalVariables() const {
            return getNumberOfVariables(storm::expressions::ExpressionReturnType::Double);
        }

        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryVariables(storm::expressions::ExpressionReturnType const& variableType) const {
            return auxiliaryVariableTypeToCountMapping[static_cast<std::size_t>(variableType)];
        }

        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryVariables() const {
            return numberOfAuxiliaryVariables;
        }

        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryBooleanVariables() const {
            return getNumberOfAuxiliaryVariables(storm::expressions::ExpressionReturnType::Bool);
        }
        
        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryIntegerVariables() const {
            return getNumberOfAuxiliaryVariables(storm::expressions::ExpressionReturnType::Int);
        }
        
        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryRationalVariables() const {
            return getNumberOfAuxiliaryVariables(storm::expressions::ExpressionReturnType::Double);
        }
        
        std::string const& ExpressionManager::getVariableName(uint_fast64_t index) const {
            auto indexTypeNamePair = indexToNameMapping.find(index);
            STORM_LOG_THROW(indexTypeNamePair != indexToNameMapping.end(), storm::exceptions::InvalidArgumentException, "Unknown variable index '" << index << "'.");
            return indexTypeNamePair->second;
        }
        
        ExpressionReturnType ExpressionManager::getVariableType(uint_fast64_t index) const {
            if ((index & booleanMask) != 0) {
                return ExpressionReturnType::Bool;
            } else if ((index & integerMask) != 0) {
                return ExpressionReturnType::Int;
            } else if ((index & rationalMask) != 0) {
                return ExpressionReturnType::Double;
            } else {
                return ExpressionReturnType::Undefined;
            }
        }
        
        uint_fast64_t ExpressionManager::getOffset(uint_fast64_t index) const {
            return index & offsetMask;
        }
        
        ExpressionManager::const_iterator ExpressionManager::begin() const {
            return ExpressionManager::const_iterator(*this, this->nameToIndexMapping.end(), this->nameToIndexMapping.begin(), const_iterator::VariableSelection::OnlyRegularVariables);
        }
        
        ExpressionManager::const_iterator ExpressionManager::end() const {
            return ExpressionManager::const_iterator(*this, this->nameToIndexMapping.end(), this->nameToIndexMapping.end(), const_iterator::VariableSelection::OnlyRegularVariables);
        }
        
    } // namespace expressions
} // namespace storm