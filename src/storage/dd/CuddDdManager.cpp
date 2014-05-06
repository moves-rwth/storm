#include <cmath>
#include <string>
#include <algorithm>

#include "src/storage/dd/CuddDdManager.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/settings/Settings.h"

bool CuddOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
    // Set up option for reordering.
    std::vector<std::string> reorderingTechniques;
    reorderingTechniques.push_back("none");
    reorderingTechniques.push_back("sift");
    reorderingTechniques.push_back("ssift");
    reorderingTechniques.push_back("gsift");
    reorderingTechniques.push_back("win2");
    reorderingTechniques.push_back("win3");
    reorderingTechniques.push_back("win4");
    reorderingTechniques.push_back("annealing");
    reorderingTechniques.push_back("genetic");
    reorderingTechniques.push_back("exact");
	instance->addOption(storm::settings::OptionBuilder("Cudd", "reorder", "", "Sets the reordering technique used by Cudd.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("method", "Sets which technique is used by Cudd's reordering routines. Must be in {\"none\", \"sift\", \"ssift\", \"gsift\", \"win2\", \"win3\", \"win4\", \"annealing\", \"genetic\", \"exact\"}.").setDefaultValueString("gsift").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(reorderingTechniques)).build()).build());
    
    // Set up options for precision and maximal memory available to Cudd.
    instance->addOption(storm::settings::OptionBuilder("Cudd", "cuddprec", "", "Sets the precision used by Cudd.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision up to which to constants are considered to be different.").setDefaultValueDouble(1e-15).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
    
    instance->addOption(storm::settings::OptionBuilder("Cudd", "maxmem", "", "Sets the upper bound of memory available to Cudd in MB.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("mb", "The memory available to Cudd (0 means unlimited).").setDefaultValueUnsignedInteger(2048).build()).build());
    
    return true;
});

namespace storm {
    namespace dd {
        DdManager<DdType::CUDD>::DdManager() : metaVariableMap(), cuddManager() {
            this->cuddManager.SetMaxMemory(storm::settings::Settings::getInstance()->getOptionByLongName("maxmem").getArgument(0).getValueAsUnsignedInteger() * 1024);
            this->cuddManager.SetEpsilon(storm::settings::Settings::getInstance()->getOptionByLongName("cuddprec").getArgument(0).getValueAsDouble());
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getOne() {
            return Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addOne());
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getZero() {
            return Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addZero());
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getConstant(double value) {
            return Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.constant(value));
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getEncoding(std::string const& metaVariableName, int_fast64_t value) {
            // Check whether the meta variable exists.
            if (!this->hasMetaVariable(metaVariableName)) {
                throw storm::exceptions::InvalidArgumentException() << "Unknown meta variable name '" << metaVariableName << "'.";
            }
            
            DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(metaVariableName);
            
            // Check whether the value is legal for this meta variable.
            if (value < metaVariable.getLow() || value > metaVariable.getHigh()) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal value " << value << " for meta variable '" << metaVariableName << "'.";
            }
            
            // Now compute the encoding relative to the low value of the meta variable.
            value -= metaVariable.getLow();
            
            std::vector<Dd<DdType::CUDD>> const& ddVariables = metaVariable.getDdVariables();

            Dd<DdType::CUDD> result;
            if (value & (1ull << (ddVariables.size() - 1))) {
                result = ddVariables[0];
            } else {
                result = !ddVariables[0];
            }
            
            for (std::size_t i = 1; i < ddVariables.size(); ++i) {
                if (value & (1ull << (ddVariables.size() - i - 1))) {
                    result *= ddVariables[i];
                } else {
                    result *= !ddVariables[i];
                }
            }
                        
            return result;
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getRange(std::string const& metaVariableName) {
            // Check whether the meta variable exists.
            if (!this->hasMetaVariable(metaVariableName)) {
                throw storm::exceptions::InvalidArgumentException() << "Unknown meta variable name '" << metaVariableName << "'.";
            }

            storm::dd::DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(metaVariableName);
            
            Dd<DdType::CUDD> result = this->getZero();
            
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result.setValue(metaVariableName, value, static_cast<double>(1));
            }
            return result;
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getIdentity(std::string const& metaVariableName) {
            // Check whether the meta variable exists.
            if (!this->hasMetaVariable(metaVariableName)) {
                throw storm::exceptions::InvalidArgumentException() << "Unknown meta variable name '" << metaVariableName << "'.";
            }
            
            storm::dd::DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(metaVariableName);
            
            Dd<DdType::CUDD> result = this->getZero();
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result.setValue(metaVariableName, value, static_cast<double>(value));
            }
            return result;
        }

        void DdManager<DdType::CUDD>::addMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high) {
            // Check whether a meta variable already exists.
            if (this->hasMetaVariable(name)) {
                throw storm::exceptions::InvalidArgumentException() << "A meta variable '" << name << "' already exists.";
            }

            // Check that the range is legal.
            if (high == low) {
                throw storm::exceptions::InvalidArgumentException() << "Range of meta variable must be at least 2 elements.";
            }
            
            std::size_t numberOfBits = static_cast<std::size_t>(std::ceil(std::log2(high - low + 1)));
            
            std::vector<Dd<DdType::CUDD>> variables;
            for (std::size_t i = 0; i < numberOfBits; ++i) {
                variables.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {name}));
            }
            
            metaVariableMap.emplace(name, DdMetaVariable<DdType::CUDD>(name, low, high, variables, this->shared_from_this()));
        }
        
        void DdManager<DdType::CUDD>::addMetaVariable(std::string const& name) {
            // Check whether a meta variable already exists.
            if (this->hasMetaVariable(name)) {
                throw storm::exceptions::InvalidArgumentException() << "A meta variable '" << name << "' already exists.";
            }
            
            std::vector<Dd<DdType::CUDD>> variables;
            variables.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {name}));
            
            metaVariableMap.emplace(name, DdMetaVariable<DdType::CUDD>(name, variables, this->shared_from_this()));
        }
        
        void DdManager<DdType::CUDD>::addMetaVariablesInterleaved(std::vector<std::string> const& names, int_fast64_t low, int_fast64_t high, bool fixedGroup) {
            // Make sure that at least one meta variable is added.
            if (names.size() == 0) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal to add zero meta variables.";
            }
            
            // Check that there are no duplicate names in the given name vector.
            std::vector<std::string> nameCopy(names);
            std::sort(nameCopy.begin(), nameCopy.end());
            if (std::adjacent_find(nameCopy.begin(), nameCopy.end()) != nameCopy.end()) {
                throw storm::exceptions::InvalidArgumentException() << "Cannot add duplicate meta variables.";
            }
            
            // Check that the range is legal.
            if (high == low) {
                throw storm::exceptions::InvalidArgumentException() << "Range of meta variable must be at least 2 elements.";
            }

            // Check whether a meta variable already exists.
            for (auto const& metaVariableName : names) {
                if (this->hasMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "A meta variable '" << metaVariableName << "' already exists.";
                }
            }
            
            // Add the variables in interleaved order.
            std::size_t numberOfBits = static_cast<std::size_t>(std::ceil(std::log2(high - low + 1)));
            std::vector<std::vector<Dd<DdType::CUDD>>> variables(names.size());
            for (uint_fast64_t bit = 0; bit < numberOfBits; ++bit) {
                for (uint_fast64_t i = 0; i < names.size(); ++i) {
                    variables[i].emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {names[i]}));
                }
            }
            
            // If required, we group the bits on the same layer of the interleaved meta variables.
            if (fixedGroup) {
                for (uint_fast64_t i = 0; i < names.size(); ++i) {
                    this->getCuddManager().MakeTreeNode(variables[i].front().getCuddAdd().NodeReadIndex(), names.size(), MTR_FIXED);
                }
            }
            
            // Now add the meta variables.
            for (uint_fast64_t i = 0; i < names.size(); ++i) {
                metaVariableMap.emplace(names[i], DdMetaVariable<DdType::CUDD>(names[i], low, high, variables[i], this->shared_from_this()));
            }
        }
        
        DdMetaVariable<DdType::CUDD> const& DdManager<DdType::CUDD>::getMetaVariable(std::string const& metaVariableName) const {
            auto const& nameVariablePair = metaVariableMap.find(metaVariableName);
            
            if (!this->hasMetaVariable(metaVariableName)) {
                throw storm::exceptions::InvalidArgumentException() << "Unknown meta variable name '" << metaVariableName << "'.";
            }
            
            return nameVariablePair->second;
        }
        
        std::set<std::string> DdManager<DdType::CUDD>::getAllMetaVariableNames() const {
            std::set<std::string> result;
            for (auto const& nameValuePair : metaVariableMap) {
                result.insert(nameValuePair.first);
            }
            return result;
        }
        
        std::size_t DdManager<DdType::CUDD>::getNumberOfMetaVariables() const {
            return this->metaVariableMap.size();
        }
        
        bool DdManager<DdType::CUDD>::hasMetaVariable(std::string const& metaVariableName) const {
            return this->metaVariableMap.find(metaVariableName) != this->metaVariableMap.end();
        }
        
        Cudd& DdManager<DdType::CUDD>::getCuddManager() {
            return this->cuddManager;
        }
        
        std::vector<std::string> DdManager<DdType::CUDD>::getDdVariableNames() const {
            // First, we initialize a list DD variables and their names.
            std::vector<std::pair<ADD, std::string>> variableNamePairs;
            for (auto const& nameMetaVariablePair : this->metaVariableMap) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = nameMetaVariablePair.second;
                for (uint_fast64_t variableIndex = 0; variableIndex < metaVariable.getNumberOfDdVariables(); ++variableIndex) {
                    variableNamePairs.emplace_back(metaVariable.getDdVariables()[variableIndex].getCuddAdd(), metaVariable.getName() + "." + std::to_string(variableIndex));
                }
            }
            
            // Then, we sort this list according to the indices of the ADDs.
            std::sort(variableNamePairs.begin(), variableNamePairs.end(), [](std::pair<ADD, std::string> const& a, std::pair<ADD, std::string> const& b) { return a.first.NodeReadIndex() < b.first.NodeReadIndex(); });
            
            // Now, we project the sorted vector to its second component.
            std::vector<std::string> result;
            for (auto const& element : variableNamePairs) {
                result.push_back(element.second);
            }
            
            return result;
        }
    }
}