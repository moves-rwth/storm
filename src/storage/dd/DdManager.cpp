#include "src/storage/dd/DdManager.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/utility/macros.h"
#include "src/utility/constants.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "storm-config.h"
#include "src/adapters/CarlAdapter.h"

#include <cmath>
#include <iostream>

namespace storm {
    namespace dd {
        template<DdType LibraryType>
        DdManager<LibraryType>::DdManager() : internalDdManager(), metaVariableMap(), manager(new storm::expressions::ExpressionManager()) {
            // Intentionally left empty.
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> DdManager<LibraryType>::getBddOne() const {
            return Bdd<LibraryType>(*this, internalDdManager.getBddOne());
        }
        
        template<DdType LibraryType>
        template<typename ValueType>
        Add<LibraryType, ValueType> DdManager<LibraryType>::getAddOne() const {
            return Add<LibraryType, ValueType>(*this, internalDdManager.template getAddOne<ValueType>());
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> DdManager<LibraryType>::getBddZero() const {
            return Bdd<LibraryType>(*this, internalDdManager.getBddZero());
        }
        
        template<DdType LibraryType>
        template<typename ValueType>
        Add<LibraryType, ValueType> DdManager<LibraryType>::getAddZero() const {
            return Add<LibraryType, ValueType>(*this, internalDdManager.template getAddZero<ValueType>());
        }
        
        template<DdType LibraryType>
        template<typename ValueType>
        Add<LibraryType, ValueType> DdManager<LibraryType>::getInfinity() const {
            return getConstant(storm::utility::infinity<ValueType>());
        }
        
        template<DdType LibraryType>
        template<typename ValueType>
        Add<LibraryType, ValueType> DdManager<LibraryType>::getConstant(ValueType const& value) const {
            return Add<LibraryType, ValueType>(*this, internalDdManager.getConstant(value));
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> DdManager<LibraryType>::getEncoding(storm::expressions::Variable const& variable, int_fast64_t value) const {
            DdMetaVariable<LibraryType> const& metaVariable = this->getMetaVariable(variable);
            
            STORM_LOG_THROW(value >= metaVariable.getLow() && value <= metaVariable.getHigh(), storm::exceptions::InvalidArgumentException, "Illegal value " << value << " for meta variable '" << variable.getName() << "'.");
            
            // Now compute the encoding relative to the low value of the meta variable.
            value -= metaVariable.getLow();
            
            std::vector<Bdd<LibraryType>> const& ddVariables = metaVariable.getDdVariables();
            
            Bdd<LibraryType> result;
            if (value & (1ull << (ddVariables.size() - 1))) {
                result = ddVariables[0];
            } else {
                result = !ddVariables[0];
            }
            
            for (std::size_t i = 1; i < ddVariables.size(); ++i) {
                if (value & (1ull << (ddVariables.size() - i - 1))) {
                    result &= ddVariables[i];
                } else {
                    result &= !ddVariables[i];
                }
            }
            
            return result;
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> DdManager<LibraryType>::getRange(storm::expressions::Variable const& variable) const {
            storm::dd::DdMetaVariable<LibraryType> const& metaVariable = this->getMetaVariable(variable);
            
            Bdd<LibraryType> result = this->getBddZero();
            
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result |= this->getEncoding(variable, value);
            }
            
            return result;
        }

        template<DdType LibraryType>
        template<typename ValueType>
        Add<LibraryType, ValueType> DdManager<LibraryType>::getIdentity(storm::expressions::Variable const& variable) const {
            storm::dd::DdMetaVariable<LibraryType> const& metaVariable = this->getMetaVariable(variable);
            
            Add<LibraryType, ValueType> result = this->getAddZero<ValueType>();
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result += this->getEncoding(variable, value).template toAdd<ValueType>() * this->getConstant(static_cast<ValueType>(value));
            }
            return result;
        }
		
        template<DdType LibraryType>
        std::pair<storm::expressions::Variable, storm::expressions::Variable> DdManager<LibraryType>::addMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high) {
            // Check whether the variable name is legal.
            STORM_LOG_THROW(name != "" && name.back() != '\'', storm::exceptions::InvalidArgumentException, "Illegal name of meta variable: '" << name << "'.");
            
            // Check whether a meta variable already exists.
            STORM_LOG_THROW(!this->hasMetaVariable(name), storm::exceptions::InvalidArgumentException, "A meta variable '" << name << "' already exists.");
            
            // Check that the range is legal.
            STORM_LOG_THROW(high >= low, storm::exceptions::InvalidArgumentException, "Illegal empty range for meta variable.");
            
            std::size_t numberOfBits = static_cast<std::size_t>(std::ceil(std::log2(high - low + 1)));
            
            // For the case where low and high coincide, we need to have a single bit.
            if (numberOfBits == 0) {
                ++numberOfBits;
            }
            
            storm::expressions::Variable unprimed = manager->declareBitVectorVariable(name, numberOfBits);
            storm::expressions::Variable primed = manager->declareBitVectorVariable(name + "'", numberOfBits);
            
            std::vector<Bdd<LibraryType>> variables;
            std::vector<Bdd<LibraryType>> variablesPrime;
            for (std::size_t i = 0; i < numberOfBits; ++i) {
                auto ddVariablePair = internalDdManager.createNewDdVariablePair();
                variables.emplace_back(Bdd<LibraryType>(*this, ddVariablePair.first, {unprimed}));
                variablesPrime.emplace_back(Bdd<LibraryType>(*this, ddVariablePair.second, {primed}));
            }

            metaVariableMap.emplace(unprimed, DdMetaVariable<LibraryType>(name, low, high, variables));
            metaVariableMap.emplace(primed, DdMetaVariable<LibraryType>(name + "'", low, high, variablesPrime));
            
            return std::make_pair(unprimed, primed);
        }
        
        template<DdType LibraryType>
        std::pair<storm::expressions::Variable, storm::expressions::Variable> DdManager<LibraryType>::addMetaVariable(std::string const& name) {
            // Check whether the variable name is legal.
            STORM_LOG_THROW(name != "" && name.back() != '\'', storm::exceptions::InvalidArgumentException, "Illegal name of meta variable: '" << name << "'.");
            
            // Check whether a meta variable already exists.
            STORM_LOG_THROW(!this->hasMetaVariable(name), storm::exceptions::InvalidArgumentException, "A meta variable '" << name << "' already exists.");
            
            storm::expressions::Variable unprimed = manager->declareBooleanVariable(name);
            storm::expressions::Variable primed = manager->declareBooleanVariable(name + "'");
            
            std::vector<Bdd<LibraryType>> variables;
            std::vector<Bdd<LibraryType>> variablesPrime;
            auto ddVariablePair = internalDdManager.createNewDdVariablePair();
            variables.emplace_back(Bdd<LibraryType>(*this, ddVariablePair.first, {unprimed}));
            variablesPrime.emplace_back(Bdd<LibraryType>(*this, ddVariablePair.second, {primed}));
            
            metaVariableMap.emplace(unprimed, DdMetaVariable<LibraryType>(name, variables));
            metaVariableMap.emplace(primed, DdMetaVariable<LibraryType>(name + "'", variablesPrime));
            
            return std::make_pair(unprimed, primed);
        }
        
        template<DdType LibraryType>
        DdMetaVariable<LibraryType> const& DdManager<LibraryType>::getMetaVariable(storm::expressions::Variable const& variable) const {
            auto const& variablePair = metaVariableMap.find(variable);
            
            // Check whether the meta variable exists.
            STORM_LOG_THROW(variablePair != metaVariableMap.end(), storm::exceptions::InvalidArgumentException, "Unknown meta variable name '" << variable.getName() << "'.");
            
            return variablePair->second;
        }
        
        template<DdType LibraryType>
        std::set<std::string> DdManager<LibraryType>::getAllMetaVariableNames() const {
            std::set<std::string> result;
            for (auto const& variablePair : metaVariableMap) {
                result.insert(variablePair.first.getName());
            }
            return result;
        }
        
        template<DdType LibraryType>
        std::size_t DdManager<LibraryType>::getNumberOfMetaVariables() const {
            return this->metaVariableMap.size();
        }
        
        template<DdType LibraryType>
        bool DdManager<LibraryType>::hasMetaVariable(std::string const& metaVariableName) const {
            return manager->hasVariable(metaVariableName);
        }
        
        template<DdType LibraryType>
        storm::expressions::ExpressionManager const& DdManager<LibraryType>::getExpressionManager() const {
            return *manager;
        }
        
        template<DdType LibraryType>
        storm::expressions::ExpressionManager& DdManager<LibraryType>::getExpressionManager() {
            return *manager;
        }
        
        template<DdType LibraryType>
        std::vector<std::string> DdManager<LibraryType>::getDdVariableNames() const {
            // First, we initialize a list DD variables and their names.
            std::vector<std::pair<uint_fast64_t, std::string>> variablePairs;
            for (auto const& variablePair : this->metaVariableMap) {
                DdMetaVariable<LibraryType> const& metaVariable = variablePair.second;
                // If the meta variable is of type bool, we don't need to suffix it with the bit number.
                if (metaVariable.getType() == MetaVariableType::Bool) {
                    variablePairs.emplace_back(metaVariable.getDdVariables().front().getIndex(), variablePair.first.getName());
                } else {
                    // For integer-valued meta variables, we, however, have to add the suffix.
                    for (uint_fast64_t variableIndex = 0; variableIndex < metaVariable.getNumberOfDdVariables(); ++variableIndex) {
                        variablePairs.emplace_back(metaVariable.getDdVariables()[variableIndex].getIndex(), variablePair.first.getName() + '.' + std::to_string(variableIndex));
                    }
                }
            }
            
            // Then, we sort this list according to the indices of the ADDs.
            std::sort(variablePairs.begin(), variablePairs.end(), [](std::pair<uint_fast64_t, std::string> const& a, std::pair<uint_fast64_t, std::string> const& b) { return a.first < b.first; });
            
            // Now, we project the sorted vector to its second component.
            std::vector<std::string> result;
            for (auto const& element : variablePairs) {
                result.push_back(element.second);
            }
            
            return result;
        }
        
        template<DdType LibraryType>
        std::vector<storm::expressions::Variable> DdManager<LibraryType>::getDdVariables() const {
            // First, we initialize a list DD variables and their names.
            std::vector<std::pair<uint_fast64_t, storm::expressions::Variable>> variablePairs;
            for (auto const& variablePair : this->metaVariableMap) {
                DdMetaVariable<LibraryType> const& metaVariable = variablePair.second;
                // If the meta variable is of type bool, we don't need to suffix it with the bit number.
                if (metaVariable.getType() == MetaVariableType::Bool) {
                    variablePairs.emplace_back(metaVariable.getDdVariables().front().getIndex(), variablePair.first);
                } else {
                    // For integer-valued meta variables, we, however, have to add the suffix.
                    for (uint_fast64_t variableIndex = 0; variableIndex < metaVariable.getNumberOfDdVariables(); ++variableIndex) {
                        variablePairs.emplace_back(metaVariable.getDdVariables()[variableIndex].getIndex(), variablePair.first);
                    }
                }
            }
            
            // Then, we sort this list according to the indices of the ADDs.
            std::sort(variablePairs.begin(), variablePairs.end(), [](std::pair<uint_fast64_t, storm::expressions::Variable> const& a, std::pair<uint_fast64_t, storm::expressions::Variable> const& b) { return a.first < b.first; });
            
            // Now, we project the sorted vector to its second component.
            std::vector<storm::expressions::Variable> result;
            for (auto const& element : variablePairs) {
                result.push_back(element.second);
            }
            
            return result;
        }
        
        template<DdType LibraryType>
        void DdManager<LibraryType>::allowDynamicReordering(bool value) {
            internalDdManager.allowDynamicReordering(value);
        }
        
        template<DdType LibraryType>
        bool DdManager<LibraryType>::isDynamicReorderingAllowed() const {
            return internalDdManager.isDynamicReorderingAllowed();
        }
        
        template<DdType LibraryType>
        void DdManager<LibraryType>::triggerReordering() {
            internalDdManager.triggerReordering();
        }
        
        template<DdType LibraryType>
        std::set<storm::expressions::Variable> DdManager<LibraryType>::getAllMetaVariables() const {
            std::set<storm::expressions::Variable> result;
            for (auto const& variable : this->metaVariableMap) {
                result.insert(variable.first);
            }
            return result;
        }
        
        template<DdType LibraryType>
        std::vector<uint_fast64_t> DdManager<LibraryType>::getSortedVariableIndices() const {
            return this->getSortedVariableIndices(this->getAllMetaVariables());
        }
        
        template<DdType LibraryType>
        std::vector<uint_fast64_t> DdManager<LibraryType>::getSortedVariableIndices(std::set<storm::expressions::Variable> const& metaVariables) const {
            std::vector<uint_fast64_t> ddVariableIndices;
            for (auto const& metaVariable : metaVariables) {
                for (auto const& ddVariable : metaVariableMap.at(metaVariable).getDdVariables()) {
                    ddVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            
            // Next, we need to sort them, since they may be arbitrarily ordered otherwise.
            std::sort(ddVariableIndices.begin(), ddVariableIndices.end());
            return ddVariableIndices;
        }
        
        template<DdType LibraryType>
        InternalDdManager<LibraryType>& DdManager<LibraryType>::getInternalDdManager() {
            return internalDdManager;
        }
        
        template<DdType LibraryType>
        InternalDdManager<LibraryType> const& DdManager<LibraryType>::getInternalDdManager() const {
            return internalDdManager;
        }
        
        template<DdType LibraryType>
        InternalDdManager<LibraryType>* DdManager<LibraryType>::getInternalDdManagerPointer() {
            return &internalDdManager;
        }
        
        template<DdType LibraryType>
        InternalDdManager<LibraryType> const* DdManager<LibraryType>::getInternalDdManagerPointer() const {
            return &internalDdManager;
        }
        
        template class DdManager<DdType::CUDD>;
        
        template Add<DdType::CUDD, double> DdManager<DdType::CUDD>::getAddZero() const;
        template Add<DdType::CUDD, uint_fast64_t> DdManager<DdType::CUDD>::getAddZero() const;

        template Add<DdType::CUDD, double> DdManager<DdType::CUDD>::getAddOne() const;
        template Add<DdType::CUDD, uint_fast64_t> DdManager<DdType::CUDD>::getAddOne() const;

        template Add<DdType::CUDD, double> DdManager<DdType::CUDD>::getInfinity<double>() const;
        template Add<DdType::CUDD, uint_fast64_t> DdManager<DdType::CUDD>::getInfinity<uint_fast64_t>() const;

        template Add<DdType::CUDD, double> DdManager<DdType::CUDD>::getConstant(double const& value) const;
        template Add<DdType::CUDD, uint_fast64_t> DdManager<DdType::CUDD>::getConstant(uint_fast64_t const& value) const;
        
        template Add<DdType::CUDD, double> DdManager<DdType::CUDD>::getIdentity(storm::expressions::Variable const& variable) const;
        template Add<DdType::CUDD, uint_fast64_t> DdManager<DdType::CUDD>::getIdentity(storm::expressions::Variable const& variable) const;
        
        
        template class DdManager<DdType::Sylvan>;
        
        template Add<DdType::Sylvan, double> DdManager<DdType::Sylvan>::getAddZero() const;
        template Add<DdType::Sylvan, uint_fast64_t> DdManager<DdType::Sylvan>::getAddZero() const;
#ifdef STORM_HAVE_CARL
		template Add<DdType::Sylvan, storm::RationalFunction> DdManager<DdType::Sylvan>::getAddZero() const;
#endif
        
        template Add<DdType::Sylvan, double> DdManager<DdType::Sylvan>::getAddOne() const;
        template Add<DdType::Sylvan, uint_fast64_t> DdManager<DdType::Sylvan>::getAddOne() const;
#ifdef STORM_HAVE_CARL
		template Add<DdType::Sylvan, storm::RationalFunction> DdManager<DdType::Sylvan>::getAddOne() const;
#endif

        template Add<DdType::Sylvan, double> DdManager<DdType::Sylvan>::getInfinity<double>() const;
        template Add<DdType::Sylvan, uint_fast64_t> DdManager<DdType::Sylvan>::getInfinity<uint_fast64_t>() const;
#ifdef STORM_HAVE_CARL
		template Add<DdType::Sylvan, storm::RationalFunction> DdManager<DdType::Sylvan>::getInfinity<storm::RationalFunction>() const;
#endif
        
        template Add<DdType::Sylvan, double> DdManager<DdType::Sylvan>::getConstant(double const& value) const;
        template Add<DdType::Sylvan, uint_fast64_t> DdManager<DdType::Sylvan>::getConstant(uint_fast64_t const& value) const;
#ifdef STORM_HAVE_CARL
		template Add<DdType::Sylvan, storm::RationalFunction> DdManager<DdType::Sylvan>::getConstant(storm::RationalFunction const& value) const;
#endif

        template Add<DdType::Sylvan, double> DdManager<DdType::Sylvan>::getIdentity(storm::expressions::Variable const& variable) const;
        template Add<DdType::Sylvan, uint_fast64_t> DdManager<DdType::Sylvan>::getIdentity(storm::expressions::Variable const& variable) const;
#ifdef STORM_HAVE_CARL
		template Add<DdType::Sylvan, storm::RationalFunction> DdManager<DdType::Sylvan>::getIdentity(storm::expressions::Variable const& variable) const;
#endif
    }
}
