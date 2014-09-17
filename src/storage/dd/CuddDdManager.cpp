#include <cmath>
#include <string>
#include <algorithm>

#include "src/storage/dd/CuddDdManager.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/settings/Settings.h"

namespace storm {
    namespace dd {
        DdManager<DdType::CUDD>::DdManager() : metaVariableMap(), cuddManager(), reorderingTechnique(CUDD_REORDER_NONE) {
            this->cuddManager.SetMaxMemory(static_cast<unsigned long>(storm::settings::Settings::getInstance()->getOptionByLongName("cuddmaxmem").getArgument(0).getValueAsUnsignedInteger() * 1024ul * 1024ul));
            this->cuddManager.SetEpsilon(storm::settings::Settings::getInstance()->getOptionByLongName("cuddprec").getArgument(0).getValueAsDouble());
            
            // Now set the selected reordering technique.
            std::string const& reorderingTechnique = storm::settings::Settings::getInstance()->getOptionByLongName("reorder").getArgument(0).getValueAsString();
            if (reorderingTechnique == "none") {
                this->reorderingTechnique = CUDD_REORDER_NONE;
            } else if (reorderingTechnique == "random") {
                this->reorderingTechnique = CUDD_REORDER_RANDOM;
            } else if (reorderingTechnique == "randompivot") {
                this->reorderingTechnique = CUDD_REORDER_RANDOM_PIVOT;
            } else if (reorderingTechnique == "sift") {
                this->reorderingTechnique = CUDD_REORDER_SIFT;
            } else if (reorderingTechnique == "siftconv") {
                this->reorderingTechnique = CUDD_REORDER_SIFT_CONVERGE;
            } else if (reorderingTechnique == "ssift") {
                this->reorderingTechnique = CUDD_REORDER_SYMM_SIFT;
            } else if (reorderingTechnique == "ssiftconv") {
                this->reorderingTechnique = CUDD_REORDER_SYMM_SIFT_CONV;
            } else if (reorderingTechnique == "gsift") {
                this->reorderingTechnique = CUDD_REORDER_GROUP_SIFT;
            } else if (reorderingTechnique == "gsiftconv") {
                this->reorderingTechnique = CUDD_REORDER_GROUP_SIFT_CONV;
            } else if (reorderingTechnique == "win2") {
                this->reorderingTechnique = CUDD_REORDER_WINDOW2;
            } else if (reorderingTechnique == "win2conv") {
                this->reorderingTechnique = CUDD_REORDER_WINDOW2_CONV;
            } else if (reorderingTechnique == "win3") {
                this->reorderingTechnique = CUDD_REORDER_WINDOW3;
            } else if (reorderingTechnique == "win3conv") {
                this->reorderingTechnique = CUDD_REORDER_WINDOW3_CONV;
            } else if (reorderingTechnique == "win4") {
                this->reorderingTechnique = CUDD_REORDER_WINDOW4;
            } else if (reorderingTechnique == "win4conv") {
                this->reorderingTechnique = CUDD_REORDER_WINDOW4_CONV;
            } else if (reorderingTechnique == "annealing") {
                this->reorderingTechnique = CUDD_REORDER_ANNEALING;
            } else if (reorderingTechnique == "genetic") {
                this->reorderingTechnique = CUDD_REORDER_GENETIC;
            } else if (reorderingTechnique == "exact") {
                this->reorderingTechnique = CUDD_REORDER_EXACT;
            }
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
            DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(metaVariableName);
            
            LOG_THROW(value >= metaVariable.getLow() && value <= metaVariable.getHigh(), storm::exceptions::InvalidArgumentException, "Illegal value " << value << " for meta variable '" << metaVariableName << "'.");
            
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
            storm::dd::DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(metaVariableName);
            
            Dd<DdType::CUDD> result = this->getZero();
            
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result.setValue(metaVariableName, value, static_cast<double>(1));
            }
            return result;
        }
        
        Dd<DdType::CUDD> DdManager<DdType::CUDD>::getIdentity(std::string const& metaVariableName) {
            storm::dd::DdMetaVariable<DdType::CUDD> const& metaVariable = this->getMetaVariable(metaVariableName);
            
            Dd<DdType::CUDD> result = this->getZero();
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result.setValue(metaVariableName, value, static_cast<double>(value));
            }
            return result;
        }

        void DdManager<DdType::CUDD>::addMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high) {
            // Check whether the variable name is legal.
            LOG_THROW(name != "" && name.back() != '\'', storm::exceptions::InvalidArgumentException, "Illegal name of meta variable: '" << name << "'.");
            
            // Check whether a meta variable already exists.
            LOG_THROW(!this->hasMetaVariable(name), storm::exceptions::InvalidArgumentException, "A meta variable '" << name << "' already exists.");

            // Check that the range is legal.
            LOG_THROW(high != low, storm::exceptions::InvalidArgumentException, "Range of meta variable must be at least 2 elements.");
            
            std::size_t numberOfBits = static_cast<std::size_t>(std::ceil(std::log2(high - low + 1)));
            
            std::vector<Dd<DdType::CUDD>> variables;
            std::vector<Dd<DdType::CUDD>> variablesPrime;
            for (std::size_t i = 0; i < numberOfBits; ++i) {
                variables.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {name}));
                variablesPrime.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {name + "'"}));
            }
            
            // Now group the non-primed and primed variable.
            for (uint_fast64_t i = 0; i < numberOfBits; ++i) {
                this->getCuddManager().MakeTreeNode(variables[i].getCuddAdd().NodeReadIndex(), 2, MTR_FIXED);
            }
            
            metaVariableMap.emplace(name, DdMetaVariable<DdType::CUDD>(name, low, high, variables, this->shared_from_this()));
            metaVariableMap.emplace(name + "'", DdMetaVariable<DdType::CUDD>(name + "'", low, high, variablesPrime, this->shared_from_this()));
        }
        
        void DdManager<DdType::CUDD>::addMetaVariable(std::string const& name) {
            // Check whether the variable name is legal.
            LOG_THROW(name != "" && name.back() != '\'', storm::exceptions::InvalidArgumentException, "Illegal name of meta variable: '" << name << "'.");
            
            // Check whether a meta variable already exists.
            LOG_THROW(!this->hasMetaVariable(name), storm::exceptions::InvalidArgumentException, "A meta variable '" << name << "' already exists.");
            
            std::vector<Dd<DdType::CUDD>> variables;
            std::vector<Dd<DdType::CUDD>> variablesPrime;
            variables.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {name}));
            variablesPrime.emplace_back(Dd<DdType::CUDD>(this->shared_from_this(), cuddManager.addVar(), {name + "'"}));

            // Now group the non-primed and primed variable.
            this->getCuddManager().MakeTreeNode(variables.front().getCuddAdd().NodeReadIndex(), 2, MTR_FIXED);
            
            metaVariableMap.emplace(name, DdMetaVariable<DdType::CUDD>(name, variables, this->shared_from_this()));
            metaVariableMap.emplace(name + "'", DdMetaVariable<DdType::CUDD>(name + "'", variablesPrime, this->shared_from_this()));
        }
        
        DdMetaVariable<DdType::CUDD> const& DdManager<DdType::CUDD>::getMetaVariable(std::string const& metaVariableName) const {
            auto const& nameVariablePair = metaVariableMap.find(metaVariableName);
            
            // Check whether the meta variable exists.
            LOG_THROW(nameVariablePair != metaVariableMap.end(), storm::exceptions::InvalidArgumentException, "Unknown meta variable name '" << metaVariableName << "'.");
            
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

        Cudd const& DdManager<DdType::CUDD>::getCuddManager() const {
            return this->cuddManager;
        }
        
        std::vector<std::string> DdManager<DdType::CUDD>::getDdVariableNames() const {
            // First, we initialize a list DD variables and their names.
            std::vector<std::pair<ADD, std::string>> variableNamePairs;
            for (auto const& nameMetaVariablePair : this->metaVariableMap) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = nameMetaVariablePair.second;
                // If the meta variable is of type bool, we don't need to suffix it with the bit number.
                if (metaVariable.getType() == DdMetaVariable<storm::dd::DdType::CUDD>::MetaVariableType::Bool) {
                    variableNamePairs.emplace_back(metaVariable.getDdVariables().front().getCuddAdd(), metaVariable.getName());
                } else {
                    // For integer-valued meta variables, we, however, have to add the suffix.
                    for (uint_fast64_t variableIndex = 0; variableIndex < metaVariable.getNumberOfDdVariables(); ++variableIndex) {
                        variableNamePairs.emplace_back(metaVariable.getDdVariables()[variableIndex].getCuddAdd(), metaVariable.getName() + "." + std::to_string(variableIndex));
                    }
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
        
        void DdManager<DdType::CUDD>::allowDynamicReordering(bool value) {
            if (value) {
                this->getCuddManager().AutodynEnable(this->reorderingTechnique);
            } else {
                this->getCuddManager().AutodynDisable();
            }
        }
        
        bool DdManager<DdType::CUDD>::isDynamicReorderingAllowed() const {
            Cudd_ReorderingType type;
            return this->getCuddManager().ReorderingStatus(&type);
        }
        
        void DdManager<DdType::CUDD>::triggerReordering() {
            this->getCuddManager().ReduceHeap(this->reorderingTechnique, 0);
        }
    }
}