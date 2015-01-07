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
            
            if (nameIndexIterator != nameIndexIteratorEnd) {
                currentElement = std::make_pair(Variable(manager.getSharedPointer(), nameIndexIterator->second), manager.getVariableType(nameIndexIterator->second));
            }
        }
        
        ExpressionManager::ExpressionManager() : nameToIndexMapping(), indexToNameMapping(), indexToTypeMapping(), variableTypeToCountMapping(), numberOfVariables(0), auxiliaryVariableTypeToCountMapping(), numberOfAuxiliaryVariables(0), freshVariableCounter(0), booleanType(nullptr), integerType(nullptr), rationalType(nullptr) {
            // Intentionally left empty.
        }
        
        Expression ExpressionManager::boolean(bool value) const {
            return Expression(std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(*this, value)));
        }

        Expression ExpressionManager::integer(int_fast64_t value) const {
            return Expression(std::shared_ptr<BaseExpression>(new IntegerLiteralExpression(*this, value)));
        }

        Expression ExpressionManager::rational(double value) const {
            return Expression(std::shared_ptr<BaseExpression>(new DoubleLiteralExpression(*this, value)));
        }
        
        bool ExpressionManager::operator==(ExpressionManager const& other) const {
            return this == &other;
        }
        
        Type const& ExpressionManager::getBooleanType() const {
            if (booleanType == nullptr) {
                booleanType = std::unique_ptr<Type>(new Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new BooleanType())));
            }
            return *booleanType;
        }
        
        Type const& ExpressionManager::getIntegerType() const {
            if (integerType == nullptr) {
                integerType = std::unique_ptr<Type>(new Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new IntegerType())));
            }
            return *integerType;
        }
        
        Type const& ExpressionManager::getBoundedIntegerType(std::size_t width) const {
            auto boundedIntegerType = boundedIntegerTypes.find(width);
            if (boundedIntegerType == boundedIntegerTypes.end()) {
                Type newType = Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new BoundedIntegerType(width)));
                boundedIntegerTypes[width] = newType;
                return boundedIntegerTypes[width];
            } else {
                return boundedIntegerType->second;
            }
        }
        
        Type const& ExpressionManager::getRationalType() const {
            if (rationalType == nullptr) {
                rationalType = std::unique_ptr<Type>(new Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new RationalType())));
            }
            return *rationalType;
        }
        
        bool ExpressionManager::isValidVariableName(std::string const& name) {
            return name.size() < 2 || name.at(0) != '_' || name.at(1) != '_';
        }

        bool ExpressionManager::variableExists(std::string const& name) const {
            auto nameIndexPair = nameToIndexMapping.find(name);
            return nameIndexPair != nameToIndexMapping.end();
        }
        
        Variable ExpressionManager::declareVariable(std::string const& name, storm::expressions::Type const& variableType) {
            STORM_LOG_THROW(!variableExists(name), storm::exceptions::InvalidArgumentException, "Variable with name '" << name << "' already exists.");
            return declareOrGetVariable(name, variableType);
        }
        
        Variable ExpressionManager::declareBooleanVariable(std::string const& name) {
            Variable var = this->declareVariable(name, this->getBooleanType());
            return var;
        }
        
        Variable ExpressionManager::declareIntegerVariable(std::string const& name) {
            return this->declareVariable(name, this->getIntegerType());
        }

        Variable ExpressionManager::declareRationalVariable(std::string const& name) {
            return this->declareVariable(name, this->getRationalType());
        }

        Variable ExpressionManager::declareAuxiliaryVariable(std::string const& name, storm::expressions::Type const& variableType) {
            STORM_LOG_THROW(!variableExists(name), storm::exceptions::InvalidArgumentException, "Variable with name '" << name << "' already exists.");
            return declareOrGetAuxiliaryVariable(name, variableType);
        }

        Variable ExpressionManager::declareOrGetVariable(std::string const& name, storm::expressions::Type const& variableType) {
            return declareOrGetVariable(name, variableType, false, true);
        }

        Variable ExpressionManager::declareOrGetAuxiliaryVariable(std::string const& name, storm::expressions::Type const& variableType) {
            return declareOrGetVariable(name, variableType, true, true);
        }

        Variable ExpressionManager::declareOrGetVariable(std::string const& name, storm::expressions::Type const& variableType, bool auxiliary, bool checkName) {
            STORM_LOG_THROW(!checkName || isValidVariableName(name), storm::exceptions::InvalidArgumentException, "Invalid variable name '" << name << "'.");
            auto nameIndexPair = nameToIndexMapping.find(name);
            if (nameIndexPair != nameToIndexMapping.end()) {
                return Variable(this->getSharedPointer(), nameIndexPair->second);
            } else {
                std::unordered_map<Type, uint_fast64_t>::iterator typeCountPair;
                if (auxiliary) {
                    typeCountPair = auxiliaryVariableTypeToCountMapping.find(variableType);
                    if (typeCountPair == auxiliaryVariableTypeToCountMapping.end()) {
                        typeCountPair = auxiliaryVariableTypeToCountMapping.insert(typeCountPair, std::make_pair(variableType, 0));
                    }
                } else {
                    typeCountPair = variableTypeToCountMapping.find(variableType);
                    if (typeCountPair == variableTypeToCountMapping.end()) {
                        typeCountPair = variableTypeToCountMapping.insert(typeCountPair, std::make_pair(variableType, 0));
                    }
                }
                
                // Compute the index of the new variable.
                uint_fast64_t newIndex = typeCountPair->second++ | variableType.getMask() | (auxiliary ? auxiliaryMask : 0);
                
                // Properly insert the variable into the data structure.
                nameToIndexMapping[name] = newIndex;
                indexToNameMapping[newIndex] = name;
                indexToTypeMapping[newIndex] = variableType;
                return Variable(this->getSharedPointer(), newIndex);
            }
        }
        
        Variable ExpressionManager::getVariable(std::string const& name) const {
            auto nameIndexPair = nameToIndexMapping.find(name);
            STORM_LOG_THROW(nameIndexPair != nameToIndexMapping.end(),  storm::exceptions::InvalidArgumentException, "Unknown variable '" << name << "'.");
            return Variable(this->getSharedPointer(), nameIndexPair->second);
        }
        
        Expression ExpressionManager::getVariableExpression(std::string const& name) const {
            return Expression(getVariable(name));
        }
        
        bool ExpressionManager::hasVariable(std::string const& name) const {
            return nameToIndexMapping.find(name) != nameToIndexMapping.end();
        }

        Variable ExpressionManager::declareFreshVariable(storm::expressions::Type const& variableType) {
            std::string newName = "__x" + std::to_string(freshVariableCounter++);
            return declareOrGetVariable(newName, variableType, false, false);
        }

        Variable ExpressionManager::declareFreshAuxiliaryVariable(storm::expressions::Type const& variableType) {
            std::string newName = "__x" + std::to_string(freshVariableCounter++);
            return declareOrGetVariable(newName, variableType, true, false);
        }

        uint_fast64_t ExpressionManager::getNumberOfVariables(storm::expressions::Type const& variableType) const {
            auto typeCountPair = variableTypeToCountMapping.find(variableType);
            if (typeCountPair == variableTypeToCountMapping.end()) {
                return 0;
            } else {
                return typeCountPair->second;
            }
        }
        
        uint_fast64_t ExpressionManager::getNumberOfVariables() const {
            return numberOfVariables;
        }
        
        uint_fast64_t ExpressionManager::getNumberOfBooleanVariables() const {
            return getNumberOfVariables(getBooleanType());
        }
        
        uint_fast64_t ExpressionManager::getNumberOfIntegerVariables() const {
            return getNumberOfVariables(getIntegerType());
        }
        
        uint_fast64_t ExpressionManager::getNumberOfRationalVariables() const {
            return getNumberOfVariables(getRationalType());
        }

        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryVariables(storm::expressions::Type const& variableType) const {
            auto typeCountPair = auxiliaryVariableTypeToCountMapping.find(variableType);
            if (typeCountPair == auxiliaryVariableTypeToCountMapping.end()) {
                return 0;
            } else {
                return typeCountPair->second;
            }
        }

        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryVariables() const {
            return numberOfAuxiliaryVariables;
        }

        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryBooleanVariables() const {
            return getNumberOfAuxiliaryVariables(getBooleanType());
        }
        
        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryIntegerVariables() const {
            return getNumberOfAuxiliaryVariables(getIntegerType());
        }
        
        uint_fast64_t ExpressionManager::getNumberOfAuxiliaryRationalVariables() const {
            return getNumberOfAuxiliaryVariables(getRationalType());
        }
        
        std::string const& ExpressionManager::getVariableName(uint_fast64_t index) const {
            auto indexTypeNamePair = indexToNameMapping.find(index);
            STORM_LOG_THROW(indexTypeNamePair != indexToNameMapping.end(), storm::exceptions::InvalidArgumentException, "Unknown variable index '" << index << "'.");
            return indexTypeNamePair->second;
        }
        
        Type const& ExpressionManager::getVariableType(uint_fast64_t index) const {
            auto indexTypePair = indexToTypeMapping.find(index);
            STORM_LOG_ASSERT(indexTypePair != indexToTypeMapping.end(), "Unable to retrieve type of unknown variable index.");
            return indexTypePair->second;
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
        
        std::shared_ptr<ExpressionManager> ExpressionManager::getSharedPointer() {
            return this->shared_from_this();
        }

        std::shared_ptr<ExpressionManager const> ExpressionManager::getSharedPointer() const {
            return this->shared_from_this();
        }
        
    } // namespace expressions
} // namespace storm